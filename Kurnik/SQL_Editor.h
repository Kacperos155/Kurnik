#pragma once
#include "pch.h"
#include "SQL table data models/SQL_Data_Model.h"


class SQL_Editor : public wxNotebook
{
	SQLite::Database& database;
	std::unordered_map<std::string, SQL_Data_Model*> data_models;
	SQL_Data_Model* active_data_model = nullptr;

	void init_data_models();
	void init_pages();
	wxSizer* init_buttons(wxPanel* parent, SQL_Data_Model* data_model);
	void recreate_table();
	//GUI
protected:

public:
	SQL_Editor(wxWindow* parent, SQLite::Database& database);
	bool export_CSV(char delimiter = ';');
	bool import_CSV(char delimiter = ';');
};
