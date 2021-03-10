#include "pch.h"
#include "SQL_Editor.h"

SQL_Editor::SQL_Editor(wxWindow* parent, SQLite::Database& database)
	:wxPanel(parent), database(database)
{
}

bool SQL_Editor::export_CSV(std::string_view values, int columns, char delimiter)
{
	/*Fl_Native_File_Chooser file_chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
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
	}*/
	return true;
}

bool SQL_Editor::import_CSV(std::string_view values, int columns, char delimiter)
{
	/*Fl_Native_File_Chooser file_chooser(Fl_Native_File_Chooser::BROWSE_FILE);
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
	database_view.reload();*/
	return true;
}
