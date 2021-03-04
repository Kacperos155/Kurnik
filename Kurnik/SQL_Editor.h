#pragma once
#include "pch.h"
#include "SQL_Table.h"

class SQL_Editor : public Fl_Group
{
	SQLite::Database& database;
	std::string_view sql_table_name;
	std::string_view sql_view_name;
	int optimal_height{};

	//GUI
	SQL_Table database_view{ 0, 0, 0, 0, sql_view_name, database};
	Fl_Group page_buttons{ 0, 0, 0, 0 };
	Fl_Button
		button_begin{ 0, 0, 0, 0, "@$->|" },
		button_prev{ 0, 0, 0, 0, "@<-" };
	Fl_Box	page_box{ 0, 0, 0, 0, "..." };
	Fl_Button
		button_next{ 0, 0, 0, 0, "@->" },
		button_end{ 0, 0, 0, 0, "@->|" };
	Fl_End end_page_buttons_pack;
	void create_pages_navigation();
protected:
	Fl_Group inputs{ 0, 0, 0, 0 };
	// In childrens classes:
	//Fl_End end_input_pack

	void recreate_table();
	bool export_CSV(std::string_view values, int columns, char delimiter = ';');
	bool import_CSV(std::string_view values, int columns, char delimiter = ';');

public:
	SQL_Editor(int X, int Y, int W, int H, std::string_view sql_table_name, std::string_view sql_view_name, SQLite::Database& db);
	void resize(int X, int Y, int W, int H) override;
	virtual bool add_record() = 0;
	virtual void export_CSV() = 0;
	virtual void import_CSV() = 0;
};

