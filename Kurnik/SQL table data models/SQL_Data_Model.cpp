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

const std::string SQL_Data_Model::create_WHERE(columns_values& values) const
{
	auto it = values.cbegin();
	std::string  where_sql{ "WHERE" };
	where_sql.reserve(values.size() * 50);

	auto insert_condition = [&]() {
		if (it->second.size())
			where_sql += fmt::format(R"( "{}" == '{}' )", it->first, it->second);
		else
			where_sql += fmt::format(R"( "{}" IS NULL )", it->first);
	};

	insert_condition();
	while (++it != values.cend()) {
		where_sql += "AND";
		insert_condition();
	}

	return where_sql;
}

const std::unordered_map<std::string, std::string> SQL_Data_Model::getRowValues(unsigned row) const
{
	if (row == std::numeric_limits<unsigned>::max()) {
		if (selected_row == std::numeric_limits<unsigned>::max())
			throw std::out_of_range{ "Selected row is above the limit of rows" };
		row = selected_row;
	}

	SQLite::Statement statement(database, fmt::format(R"(SELECT * FROM "{}" LIMIT 1 OFFSET {};)", table, selected_row));
	if (statement.executeStep() == 0)
		return columns_values{};

	std::unordered_map<std::string, std::string> key_value;
	key_value.reserve(statement.getColumnCount());

	for (int i = 0; i < statement.getColumnCount(); ++i) {
		auto pair = std::make_pair(std::move(statement.getColumnName(i)), std::move(statement.getColumn(i).getText()));
		key_value.insert(pair);
	}
	return key_value;
}

void SQL_Data_Model::prepareChange(std::string&& key, std::string&& value)
{
	key_value_changes.insert(std::make_pair(std::move(key), std::move(value)));
}

void SQL_Data_Model::prepareChange_ISO_Date(std::string&& key, wxDateTime&& iso_date)
{
	auto date_str = fmt::format("{}-{:0>2}-{:0>2}", iso_date.GetYear(), iso_date.GetMonth() + 1, iso_date.GetDay());
	prepareChange(std::move(key), std::move(date_str));
}

void SQL_Data_Model::prepareChange_NULL(std::string&& key)
{
	key_value_changes.insert(std::make_pair(std::move(key), std::string{}));
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

	auto it = key_value_changes.begin();
	auto columns = fmt::format("\"{}\"", it->first);
	auto values = fmt::format("'{}'", it->second);

	while (++it != key_value_changes.end()) {
		columns += fmt::format(", \"{}\"", it->first);
		if (it->second.empty())
			values += fmt::format(", NULL", std::move(it->second));
		else
			values += fmt::format(", '{}'", std::move(it->second));
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
	if (!read_inputs()
		|| selected_row == std::numeric_limits<unsigned>::max()
		|| key_value_changes.size() == 0)
		return false;

	auto key_value_to_string = [&](std::string& str, auto it) {
		if (it->second.empty())
			str += fmt::format(" \"{}\" = NULL ", it->first);
		else
			str += fmt::format(" \"{}\" = '{}' ", it->first, it->second);
	};

	auto changes = std::string{};
	key_value_to_string(changes, key_value_changes.begin());
	for (auto it = std::next(key_value_changes.cbegin()); it != key_value_changes.cend(); ++it) {
		changes += ',';
		key_value_to_string(changes, it);
	}

	try {
		auto values = getRowValues();
		auto where_sql = create_WHERE(values);

		auto to_update = fmt::format(R"(UPDATE "{}" SET {} {})", table, changes, create_WHERE(values));
		auto confirmation = fmt::format("Do you realy want to update row {} ?\n", selected_row);

		std::string before{}, now{};
		for (auto& change : key_value_changes) {
			before += fmt::format("\n{}: {}", change.first, values.at(change.first));
			now += fmt::format("\n{}: {}", change.first, change.second);
		}
		confirmation += fmt::format("{}\nTO\n{}", std::move(before), std::move(now));

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
		auto values = getRowValues();
		auto to_delete = fmt::format(R"(DELETE FROM "{}" {})", table, create_WHERE(values));

		auto confirmation = fmt::format("Do you realy want to delete row {} ?\n", selected_row);
		confirmation.reserve(values.size() * 50);

		for (auto it = values.begin(); it != values.end(); ++it) {
			if (it->second.size())
				confirmation += fmt::format("{}: {}\n", it->first, it->second);
			else
				confirmation += fmt::format("{}: null\n", it->first);
		}
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
		auto query = fmt::format("select * from \"{}\" limit 1 offset {}", view, rows_amount - row - 1);
		SQLite::Statement statement(database, query);
		statement.executeStep();
		if (statement.getColumn(col).isBlob())
			variant = "...BLOB...";
		else
			variant = statement.getColumn(col).getText();
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
