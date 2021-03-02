#pragma once
#include "pch.h"
#include "SQL_Editor.h"

class MainWindow : public Fl_Double_Window
{
	int
		width = 1000,
		height = 1000;

	SQLite::Database& db;
	Fl_Menu_Bar menu{ 0, 0, width, 30 };

	std::unique_ptr<SQL_Editor> editor;

	int handle(int event) override;
	void create_menu();
	void change_editor(std::unique_ptr<SQL_Editor>&& new_editor);
	bool exportCSV();
	bool importCSV();
public:
	MainWindow(int W, int H, const char* title, SQLite::Database& db);
};

