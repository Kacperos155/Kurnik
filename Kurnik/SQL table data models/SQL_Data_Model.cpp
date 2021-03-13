#include "../pch.h"
#include "SQL_Data_Model.h"

SQL_Data_Model::SQL_Data_Model(const std::string_view table, const std::string_view view, SQLite::Database& database)
	: table(table), view(view), database(database)
{
	initColumns();
	wxDataViewVirtualListModel::Reset(rows_amount);
	inputs_sizer = new wxFlexGridSizer(2, static_cast<int>(view_columns.size()), 0, 10);
	inputs_sizer->SetFlexibleDirection(wxVERTICAL);
}

const unsigned SQL_Data_Model::calcRowsAmount()
{
	auto query_string = fmt::format("select count(*) from \"{}\"", view);
	SQLite::Statement query(database, query_string);
	query.executeStep();
	rows_amount = query.getColumn(0).getUInt();
	return rows_amount;
}

void SQL_Data_Model::initColumns()
{
	SQLite::Statement query_statement(database, fmt::format("select * from \"{}\"", view));

	auto columns_count = query_statement.getColumnCount();
	view_columns.reserve(columns_count);
	for (int i = 0; i < columns_count; ++i) {
		auto* text_renderer = new wxDataViewTextRenderer();
		auto title = query_statement.getColumnName(i);
		auto& col = view_columns.emplace_back(new wxDataViewColumn(title, text_renderer, i));
		col->SetMinWidth(wxDVC_DEFAULT_WIDTH);
	}
	key_value_changes.reserve(columns_count);
}

/*!
 * @brief Runs SQL SELECT on row
 * @param row: Table row in descending order
 * @return Pair of std::string. {
 * row as SQL WHERE conditions;
 * column name: value keys }
*/
const std::pair<std::string, std::string> SQL_Data_Model::getRowValues(unsigned row) const
{
	auto empty_return = std::make_pair(std::string{}, std::string{});
	if (row == std::numeric_limits<unsigned>::max())
		row = selected_row;
	if (row == std::numeric_limits<unsigned>::max())
		return empty_return;

	SQLite::Statement statement(database, fmt::format(R"(SELECT * FROM "{}" LIMIT 1 OFFSET {};)", table, selected_row));
	if (statement.executeStep() == 0)
		return empty_return;

	auto sql_where = fmt::format(R"(WHERE "{}" == '{}')", statement.getColumnName(0), statement.getColumn(0).getText());
	auto columns_value_pairs = fmt::format("{}: {}", statement.getColumnName(0), statement.getColumn(0).getText());
	for (int i = 1; i < statement.getColumnCount(); ++i) {
		if (!statement.getColumn(i).isNull())
		{
			sql_where += fmt::format(R"( AND "{}" == '{}')", statement.getColumnName(i), statement.getColumn(i).getText());
			columns_value_pairs += fmt::format("\n{}: {}", statement.getColumnName(i), statement.getColumn(i).getText());
		}
	}
	return std::make_pair(sql_where, columns_value_pairs);
}

void SQL_Data_Model::prepareChange(std::string&& key, std::string&& value)
{
	key_value_changes.push_back(std::make_pair(std::move(key), std::move(value)));
}

void SQL_Data_Model::prepareChange_ISO_Date(std::string&& key, wxDateTime&& iso_date)
{
	auto date_str = fmt::format("{}-{:0>2}-{:0>2}", iso_date.GetYear(), iso_date.GetMonth() + 1, iso_date.GetDay());
	prepareChange(std::move(key), std::move(date_str));
}

void SQL_Data_Model::prepareChange_NULL(std::string&& key)
{
	key_value_changes.push_back(std::make_pair(std::move(key), std::string{}));
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

void SQL_Data_Model::Reset(bool recalc)
{
	if (recalc)
		calcRowsAmount();
	wxDataViewVirtualListModel::Reset(rows_amount);
}

bool SQL_Data_Model::selectRow(unsigned int row)
{
	if (row > rows_amount)
		return false;
	selected_row = rows_amount - row;
	loadFromSelection();
	return true;
}

bool SQL_Data_Model::addRow()
{
	if (!read_inputs())
		return false;

	auto columns = fmt::format("\"{}\"", key_value_changes[0].first);
	auto values = fmt::format("'{}'", key_value_changes[0].second);
	for (auto it = std::next(key_value_changes.begin()); it != key_value_changes.end(); ++it) {
		columns += fmt::format(",\"{}\"", std::move(it->first));
		if (it->second.empty())
			values += fmt::format(", NULL", std::move(it->second));
		else
			values += fmt::format(",'{}'", std::move(it->second));
	}
	key_value_changes.clear();

	try {
		auto statement = SQLite::Statement(database, fmt::format(R"(INSERT INTO "{}"({}) VALUES ({});)", table, std::move(columns), std::move(values)));
		wxLogInfo(statement.getExpandedSQL().c_str());
		statement.exec();
	}
	catch (SQLite::Exception exception) {
		SQL_Error(exception);
		return false;
	}
	++rows_amount;
	Reset();
	return true;
}

bool SQL_Data_Model::updateSelectedRow()
{
	if (!read_inputs())
		return false;

	if (selected_row == std::numeric_limits<unsigned>::max())
		return false;

	if (key_value_changes.size() == 0)
		return false;

	auto changes = fmt::format("\"{}\" = '{}'", key_value_changes[0].first, key_value_changes[0].second);
	for (auto it = std::next(key_value_changes.cbegin()); it != key_value_changes.cend(); ++it) {
		if (it->second.empty())
			changes += fmt::format(",\n\"{}\" = NULL", it->first);
		else
			changes += fmt::format(",\n\"{}\" = '{}'", it->first, it->second);
	}

	try {
		auto [to_update, confirmation] = getRowValues();
		to_update = fmt::format(R"(UPDATE "{}" SET {} {})", table, changes, std::move(to_update));
		confirmation = fmt::format("Do you realy want to update row {}?\n{}\n\nTO\n", selected_row, std::move(confirmation));
		for (auto& change : key_value_changes) {
			confirmation += fmt::format("\n{}: {}", std::move(change.first), std::move(change.second));
		}

		key_value_changes.clear();
		if (wxNO == wxMessageBox(std::move(confirmation), "Update Confirmation", wxYES_NO | wxICON_WARNING | wxNO_DEFAULT))
			return false;
		wxLogInfo(to_update.c_str());
		if (!database.exec(std::move(to_update)))
			return false;
	}
	catch (SQLite::Exception exception) {
		SQL_Error(exception);
		return false;
	}
	Reset();
	return true;
}

bool SQL_Data_Model::deleteSelectedRow()
{
	if (selected_row == std::numeric_limits<unsigned>::max())
		return false;
	try {
		auto [to_delete, confirmation] = getRowValues();
		to_delete = fmt::format(R"(DELETE FROM "{}" {})", table, std::move(to_delete));
		confirmation = fmt::format("Do you realy want to delete row {}?\n{}", selected_row, std::move(confirmation));
		if (wxNO == wxMessageBox(std::move(confirmation), "Delete Confirmation", wxYES_NO | wxICON_WARNING | wxNO_DEFAULT))
			return false;
		wxLogInfo(to_delete.c_str());
		if (!database.exec(std::move(to_delete)))
			return false;
	}
	catch (SQLite::Exception exception) {
		SQL_Error(exception);
		return false;
	}
	--rows_amount;
	Reset();
	return true;
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
	try {
		std::string col_name;
		{
			auto query = fmt::format("select * from \"{}\"", view);
			SQLite::Statement query_statement(database, query);
			col_name = query_statement.getColumnName(col);
		}
		auto query = fmt::format("select \"{}\" from \"{}\" limit 1 offset {}", col_name, view, rows_amount - row - 1);
		SQLite::Statement statement(database, query.data());
		statement.executeStep();
		wxString result = statement.getColumn(0).getText();
		if (statement.getColumn(0).isBlob())
			variant = "...BLOB...";
		else
			variant = result;
	}
	catch (SQLite::Exception exception) {
		SQL_Error(exception);
	}
}

bool SQL_Data_Model::SetValueByRow(const wxVariant& variant, unsigned row, unsigned col)
{
	return false;
}

void SQL_Data_Model::SQL_Error(SQLite::Exception exception) noexcept
{
	wxMessageBox(exception.what(), "SQL Error", wxICON_ERROR);
}
