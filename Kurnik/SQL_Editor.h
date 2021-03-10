#pragma once
#include "pch.h"
#include "SQL_Data_Model.h"


class SQL_Editor : public wxPanel
{
	SQLite::Database& database;
	std::vector<SQL_Data_Model*> data_models;
	SQL_Data_Model* active_data_model = nullptr;

	wxBoxSizer* main_vertical_sizer = new wxBoxSizer(wxVERTICAL);
	wxDataViewCtrl* data_view = nullptr;

	void init_data_models();
	void recreate_table();
	//GUI
protected:

public:
	SQL_Editor(wxWindow* parent, SQLite::Database& database);
	bool export_CSV(char delimiter = ';');
	bool import_CSV(char delimiter = ';');
};
