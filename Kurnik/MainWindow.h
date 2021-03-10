#pragma once
#include "pch.h"
#include "SQL_Editor.h"
#include "Collected_Eggs_Model.h"

class MainWindow : public wxFrame
{
	SQLite::Database& db;
	SQL_Editor* editor = nullptr;

	wxMenuBar* menu_bar = new wxMenuBar();
	wxMenu* menu_file = new wxMenu();
	wxMenu* menu_database = new wxMenu();

	void create_menu();
public:
	MainWindow(std::string_view title, SQLite::Database& db);
};

