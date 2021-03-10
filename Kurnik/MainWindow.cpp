#include "pch.h"
#include "MainWindow.h"
//#include "Editors/Editor_Bought_Resources.h"
//#include "Editors/Editor_Buyers.h"
//#include "Editors/Editor_Collected_Eggs.h"
//#include "Editors/Editor_Sold_Eggs.h"

void MainWindow::create_menu()
{
	menu_file->Append(wxID_EXIT);
	Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		Close(true);
		}, GetId(), wxID_EXIT);

	auto* menu_export_csv = new wxMenuItem(nullptr, wxID_ANY, "&Export CSV");
	menu_database->Append(menu_export_csv);
	Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		if (editor != nullptr)
			;//editor->export_CSV();
		}, GetId(), menu_export_csv->GetId());

	auto* menu_import_csv = new wxMenuItem(nullptr, wxID_ANY, "&Import CSV");
	menu_database->Append(menu_import_csv);
	Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		if (editor != nullptr)
			;//editor->import_CSV();
		}, GetId(), menu_import_csv->GetId());

	menu_bar->Append(menu_file, "&File");
	menu_bar->Append(menu_database, "&Database");
	SetMenuBar(menu_bar);
}

MainWindow::MainWindow(std::string_view title, SQLite::Database& db)
	: wxFrame(nullptr, wxID_ANY, title.data()), db(db)
{
	Center();
	create_menu();
	auto view = new wxDataViewCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_HORIZ_RULES);
	view->SetMinSize(wxSize(400, 400));
	auto data = new Collected_Eggs_Model("Collected Eggs", "Collected Eggs", db);

	view->AssociateModel(data);
	for (const auto& col : data->getColumns()) {
		view->AppendColumn(col);
	}
	auto sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(view, wxSizerFlags(1).Expand().Border());

	SetSizerAndFit(sizer);
}
