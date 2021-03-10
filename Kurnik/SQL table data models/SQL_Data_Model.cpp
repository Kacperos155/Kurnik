#include "../pch.h"
#include "SQL_Data_Model.h"

SQL_Data_Model::SQL_Data_Model(const std::string_view table, const std::string_view view, SQLite::Database& database)
	: table(table), view(view), database(database)
{
	initColumns();
	wxDataViewVirtualListModel::Reset(rows_amount);
}

const unsigned SQL_Data_Model::calcRowsAmount()
{
	auto query_string = fmt::format("select count(*) from \"{}\"", view);
	SQLite::Statement query(database, query_string);
	query.executeStep();
	return query.getColumn(0).getUInt();
}

void SQL_Data_Model::initColumns()
{
	auto query = fmt::format("select * from \"{}\"", view);
	SQLite::Statement query_statement(database, query);

	auto columns_count = query_statement.getColumnCount();
	view_columns.reserve(columns_count);
	for (int i = 0; i < columns_count; ++i) {
		auto* text_renderer = new wxDataViewTextRenderer();
		auto title = query_statement.getColumnName(i);
		auto& col = view_columns.emplace_back(new wxDataViewColumn(title, text_renderer, i));
		col->SetMinWidth(wxDVC_DEFAULT_WIDTH);
	}
}

void SQL_Data_Model::fillCellValue(wxVariant& variant, std::string_view query) const
{
	SQLite::Statement query_statement(database, query.data());
	query_statement.executeStep();
	wxString result = query_statement.getColumn(0).getText();
	try {
		if (query_statement.getColumnDeclaredType(0) == std::string_view("BLOB"))
			variant = "...BLOB...";
		else
			variant = result;
	}
	catch (std::exception e) {
		variant = result;
	}
}

const std::vector<wxDataViewColumn*>& SQL_Data_Model::getColumns() const
{
	return view_columns;
}

const std::string_view SQL_Data_Model::getTableName() const
{
	return table;
}

const std::string_view SQL_Data_Model::getViewName() const
{
	return view;
}

const unsigned SQL_Data_Model::getRowsAmount() const
{
	return rows_amount;
}

void SQL_Data_Model::Reset()
{
	rows_amount = calcRowsAmount();
	wxDataViewVirtualListModel::Reset(rows_amount);
}

unsigned int SQL_Data_Model::GetColumnCount() const
{
	return static_cast<unsigned int>(view_columns.size());
}

wxString SQL_Data_Model::GetColumnType(unsigned int col) const
{
	return view_columns[col]->GetRenderer()->GetVariantType();
}

void SQL_Data_Model::GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const
{
	std::string col_name;
	{
		auto query = fmt::format("select * from \"{}\"", view);
		SQLite::Statement query_statement(database, query);
		col_name = query_statement.getColumnName(col);
	}
	auto query = fmt::format("select \"{}\" from \"{}\" limit 1 offset {}", col_name, view, rows_amount - row - 1);
	fillCellValue(variant, query);
}
