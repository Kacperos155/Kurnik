#pragma once
#include "pch.h"
#include "SQL_Table.h"

class SQL_Editor : public wxPanel
{
	SQLite::Database& database;

	//GUI
protected:
	bool export_CSV(std::string_view values, int columns, char delimiter = ';');
	bool import_CSV(std::string_view values, int columns, char delimiter = ';');

public:
	SQL_Editor(wxWindow* parent, SQLite::Database& database);
	bool add_record();
	void export_CSV();
	void import_CSV();
};

