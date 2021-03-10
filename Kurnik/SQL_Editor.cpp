#include "pch.h"
#include "SQL_Editor.h"
#include "SQL table data models/Collected_Eggs_Model.h"
#include "SQL table data models/Sold_Eggs_Model.h"
#include "SQL table data models/Buyers_Model.h"
#include "SQL table data models/Bought_Resources_Model.h"

SQL_Editor::SQL_Editor(wxWindow* parent, SQLite::Database& database)
	:wxNotebook(parent, wxID_ANY), database(database)
{
	init_data_models();
	init_pages();
}

void SQL_Editor::init_data_models()
{
	{
		std::string_view table{ "Collected Eggs" };
		data_models.emplace(std::make_pair(table,
			new Collected_Eggs_Model(table, table, database)));
	}
	{
		std::string_view table{ "Sold Eggs" };
		data_models.emplace(std::make_pair(table,
			new Sold_Eggs_Model(table, "Sold Eggs + Buyers", database)));
	}
	{
		std::string_view table{ "Buyers" };
		data_models.emplace(std::make_pair(table,
			new Buyers_Model(table, table, database)));
	}
	{
		std::string_view table{ "Bought Resources" };
		data_models.emplace(std::make_pair(table,
			new Bought_Resources_Model(table, table, database)));
	}
}

void SQL_Editor::init_pages()
{
	for (const auto& [model_name, model] : data_models) {
		auto* panel = new wxPanel(this);
		auto* data_view = new wxDataViewCtrl(panel, -1, wxDefaultPosition, wxDefaultSize,
			wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_HORIZ_RULES);
		data_view->SetMinSize(wxSize(600, 500));

		data_view->AssociateModel(model);

		for (const auto& col : model->getColumns()) {
			data_view->AppendColumn(col);
		}

		auto* main_vertical_sizer = new wxBoxSizer(wxVERTICAL);
		main_vertical_sizer->Add(data_view, wxSizerFlags(1).Expand());
		panel->SetSizer(main_vertical_sizer);
		panel->SetName(model_name);
		AddPage(panel, model_name);
	}

	Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, [&](wxBookCtrlEvent& e) {
		std::string page = GetCurrentPage()->GetName().ToStdString();
		active_data_model = data_models[page];
		});
}

void SQL_Editor::recreate_table()
{
/*	*/#include "Create_Tables.sql"
	auto& table = active_data_model->getTableName();
	auto check_table_sql = [&table](std::string_view sql) {
		auto it = sql.find(table);
		return (it != std::string_view::npos) ? true : false;
	};

	if (check_table_sql(sql_create_table_collected_eggs)) {
		database.exec(sql_create_table_collected_eggs);
	}
	else if (check_table_sql(sql_create_table_sold_eggs)) {
		database.exec(sql_create_table_sold_eggs);
	}
	else if (check_table_sql(sql_create_table_buyers)) {
		database.exec(sql_create_table_buyers);
	}
	else if (check_table_sql(sql_create_table_bought_resources)) {
		database.exec(sql_create_table_bought_resources);
	}
}

bool SQL_Editor::export_CSV(char delimiter)
{
	auto table_name = active_data_model->getTableName().data();
	auto file_chooser = wxFileDialog(this, fmt::format("{} - Export", table_name), "",
		table_name, "*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (file_chooser.ShowModal() == wxID_CANCEL)
		return false;

	auto statement = fmt::format("select * from \"{}\";", table_name);
	std::ofstream file(file_chooser.GetPath().ToStdString());
	if (!file.is_open())
		return false;

	auto query = SQLite::Statement(database, statement);
	auto columns_amount = query.getColumnCount();

	for (int i = 0; i < columns_amount; ++i) {
		if (i != 0)
			file << delimiter;
		file << query.getColumnName(i);
	}

	while (query.executeStep()) {
		file << '\n';
		for (int i = 0; i < columns_amount; ++i) {
			if (i != 0)
				file << delimiter;
			file << query.getColumn(i);
		}
	}
	return true;
}

bool SQL_Editor::import_CSV(char delimiter)
{
	auto table_name = active_data_model->getTableName().data();
	auto file_chooser = wxFileDialog(this, fmt::format("{} - Export", table_name), "",
		table_name, "*.csv", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (file_chooser.ShowModal() == wxID_CANCEL)
		return false;

	std::string values{};
	auto columns_amount = 0;
	try
	{
		auto columns = SQLite::Statement(database, fmt::format("select * from \"{}\"", table_name));
		columns_amount = columns.getColumnCount();
		values = columns.getColumnName(0);
		for (int i = 1; i < columns.getColumnCount(); ++i) {
			values += fmt::format(", \"{}\"", columns.getColumnName(i));
		}
	}
	catch (std::exception e) {
		wxMessageBox(e.what());
	}

	auto statement = fmt::format("Insert into \"{}\" ({}) VALUES (", table_name, values);
	for (int i = 0; i < columns_amount - 1; ++i)
		statement += "?, ";
	statement += "?);";

	std::ifstream file(file_chooser.GetPath().ToStdString());
	if (!file.is_open())
		return false;

	SQLite::Transaction T(database);
	recreate_table();
	std::string line;

	for (std::getline(file, line); std::getline(file, line) && line.size();) {
		SQLite::Statement insert(database, statement);

		auto end = size_t{ 0 };
		for (int i = 1; i <= columns_amount; ++i) {
			auto start = end + ((i != 1) ? 1 : 0);
			end = line.find_first_of(delimiter, start);
			insert.bind(i, line.substr(start, end - start));
		}
		try {
			insert.exec();
		}
		catch (std::exception e) {
			wxMessageBox(e.what());
		}
	}
	T.commit();
	active_data_model->Reset();
	return true;
}
