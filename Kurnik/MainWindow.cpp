#include "pch.h"
#include "MainWindow.h"

void MainWindow::create_menu()
{
	enum Menu_ID {
		Export_CSV = wxID_LOWEST + 1,
		Import_CSV = wxID_LOWEST + 2
	};
	menu_file->Append(wxID_EXIT);
	Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		Close(true);
		}, wxID_EXIT);

	menu_export_csv = new wxMenuItem(nullptr, Menu_ID::Export_CSV, "&Export CSV");
	menu_database->Append(menu_export_csv);
	Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		if (editor != nullptr)
			editor->export_CSV();
		}, Menu_ID::Export_CSV);

	auto* menu_import_csv = new wxMenuItem(nullptr, Menu_ID::Import_CSV, "&Import CSV");
	menu_database->Append(menu_import_csv);
	Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		if (editor != nullptr)
			editor->import_CSV();
		}, Menu_ID::Import_CSV);

	menu_bar->Append(menu_file, "&File");
	menu_bar->Append(menu_database, "&Database");
	SetMenuBar(menu_bar);
}

void MainWindow::load_icon()
{
	if (app_icon == nullptr)
		app_icon = new wxIcon;

	app_icon->LoadFile("../Resources/Kurnik.png", wxBITMAP_TYPE_PNG);
	SetIcon(*app_icon);
}

MainWindow::MainWindow(std::string_view title, SQLite::Database& database)
	: wxFrame(nullptr, wxID_ANY, title.data()), database(database)
{
	wxInitAllImageHandlers();
	create_menu();

	editor = new SQL_Editor(this, database);
	main_vertical_sizer->Add(editor, wxSizerFlags(1).Expand());

	SetSizerAndFit(main_vertical_sizer);
	status_bar = CreateStatusBar();
	Center();
	SetFocus();
	load_icon();
}
