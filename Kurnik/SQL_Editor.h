#pragma once
#include "pch.h"
#include "SQL_Table.h"

class SQL_Editor : public Fl_Group
{
	int
		x = Fl_Group::x(),
		y = Fl_Group::y(),
		width = Fl_Group::w(),
		height = Fl_Group::h();

	SQLite::Database& database;
	std::string_view sql_table_name;
	std::string_view sql_view_name;

	//GUI
	Fl_Pack main_vertical_pack{ x, y, width, height };
	SQL_Table database_view{0, 0, width, height, sql_view_name, database};
	Fl_Pack page_buttons_pack{ 0, 0, width, 20 };
	Fl_Button
		button_begin{ 0, 0, 50, 0, "@$->|" },
		button_prev{ 0, 0, 50, 0, "@<-" };
	Fl_Box	page_box{ 0, 0, width - 4 * 50, 0, "..." };
	Fl_Button
		button_next{ 0, 0, 50, 0, "@->" },
		button_end{ 0, 0, 50, 0, "@->|" };
	Fl_End end_page_buttons_pack;
	Fl_End end_main_vertical_pack;
	//GUI - end

	void create_pages_navigation();
protected:
	void recreate_table();
	bool export_CSV(std::string_view values, int columns, char delimiter = ';');
	bool import_CSV(std::string_view values, int columns, char delimiter = ';');

public:
	SQL_Editor(int X, int Y, int W, int H, std::string_view sql_table_name, std::string_view sql_view_name, SQLite::Database& db);
	virtual bool add_record() = 0;
	virtual void export_CSV() = 0;
	virtual void import_CSV() = 0;
};

