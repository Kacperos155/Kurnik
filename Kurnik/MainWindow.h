#pragma once
#include "pch.h"
#include "SQL_Editor.h"

class MainWindow : public wxFrame
{
	SQLite::Database& database;
	SQL_Editor* editor = nullptr;

	wxMenuBar* menu_bar = new wxMenuBar();
	wxMenu* menu_file = new wxMenu();
	wxMenu* menu_database = new wxMenu();
	wxMenuItem* menu_export_csv = nullptr;
	wxMenuItem* menu_import_csv = nullptr;
	wxStatusBar* status_bar = nullptr;

	wxBoxSizer* main_vertical_sizer = new wxBoxSizer(wxVERTICAL);
	wxIcon* app_icon = new wxIcon();

	void create_menu();
	void load_icon();

public:
	MainWindow(std::string_view title, SQLite::Database& database);
};

