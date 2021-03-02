#include "pch.h"
#include "SQL_Editor.h"

void SQL_Editor::create_pages_navigation()
{
	page_buttons_pack.type(FL_HORIZONTAL);
	page_buttons_pack.resizable(main_vertical_pack);
	button_begin.take_focus();

	button_begin.callback([](Fl_Widget*, void* v) {
		reinterpret_cast<SQL_Table*>(v)->prev_page(true);
		}, & database_view);
	button_prev.callback([](Fl_Widget*, void* v) {
		reinterpret_cast<SQL_Table*>(v)->prev_page();
		}, & database_view);
	button_next.callback([](Fl_Widget*, void* v) {
		reinterpret_cast<SQL_Table*>(v)->next_page();
		}, & database_view);
	button_end.callback([](Fl_Widget*, void* v) {
		reinterpret_cast<SQL_Table*>(v)->next_page(true);
		}, & database_view);

	button_begin.shortcut(FL_Home);
	button_prev.shortcut(FL_Page_Up);
	button_next.shortcut(FL_Page_Down);
	button_end.shortcut(FL_End);
}

SQL_Editor::SQL_Editor(int X, int Y, int W, int H, std::string_view sql_table_name, std::string_view sql_view_name, SQLite::Database& db)
	:Fl_Group(X, Y, W, H, sql_view_name.data()), sql_table_name(sql_table_name), sql_view_name(sql_view_name), database(db)
{
	create_pages_navigation();
	labeltype(Fl_Labeltype::FL_NO_LABEL);
}

void SQL_Editor::recreate_table()
{
/*	*/#include "Create_Tables.sql"
	auto& table = sql_table_name;
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

bool SQL_Editor::export_CSV(std::string_view values, int columns, char delimiter)
{
	Fl_Native_File_Chooser file_chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	file_chooser.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
	file_chooser.title(fmt::format("{} - Export", sql_table_name).c_str());
	auto default_file = fmt::format("{}.csv", sql_table_name);
	file_chooser.preset_file(default_file.c_str());
	file_chooser.filter("*.csv");
	if (file_chooser.show() != 0)
		return false;

	auto statement = fmt::format("select {} from \"{}\";", values, sql_table_name);
	std::ofstream file(file_chooser.filename());
	if (!file.is_open())
		return false;

	auto query = SQLite::Statement(database, statement);
	auto columns_amount = query.getColumnCount();

	for (int i = 0; i < columns_amount; ++i) {
		if (i != 0)
			file << ';';
		file << query.getColumnName(i);
	}

	while (query.executeStep()) {
		file << '\n';
		for (int i = 0; i < columns_amount; ++i) {
			if (i != 0)
				file << ';';
			file << query.getColumn(i);
		}
	}
	return true;
}

bool SQL_Editor::import_CSV(std::string_view values, int columns, char delimiter)
{
	Fl_Native_File_Chooser file_chooser(Fl_Native_File_Chooser::BROWSE_FILE);
	file_chooser.title(fmt::format("{} - Import", sql_table_name).c_str());
	auto default_file = fmt::format("{}.csv", sql_table_name);
	file_chooser.preset_file(default_file.c_str());
	file_chooser.filter("*.csv");
	if (file_chooser.show() != 0)
		return false;

	auto statement = fmt::format("Insert into \"{}\" ({}) VALUES (", sql_table_name, values);
	for (int i = 0; i < columns - 1; ++i)
		statement += "?, ";
	statement += "?);";

	std::ifstream file(file_chooser.filename());
	if (!file.is_open())
		return false;

	recreate_table();
	SQLite::Transaction T(database);
	std::string line;

	for (std::getline(file, line); std::getline(file, line) && line.size();) {
		SQLite::Statement insert(database, statement);

		auto end = size_t{ 0 };
		for (int i = 1; i <= columns; ++i) {
			auto start = end + ((i != 1) ? 1 : 0);
			end = line.find_first_of(delimiter, start);
			insert.bind(i, line.substr(start, end - start));
		}
		insert.exec();
	}
	T.commit();
	database_view.reload();
	return true;
}