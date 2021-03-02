#include "pch.h"
#include "MainWindow.h"
#include "Editors/Editor_Bought_Resources.h"
#include "Editors/Editor_Buyers.h"
#include "Editors/Editor_Collected_Eggs.h"
#include "Editors/Editor_Sold_Eggs.h"

int MainWindow::handle(int event)
{
	/*if (event == FL_SHORTCUT && Fl::event_key() == FL_Escape)
		return 1;*/
	return Fl_Double_Window::handle(event);
}

void MainWindow::create_menu()
{
	//menu.color(0xFFFFFFFF);
	menu.box(Fl_Boxtype::FL_FLAT_BOX);

	menu.add("File/Close", nullptr, [](Fl_Widget* w, void* v) {
		reinterpret_cast<Fl_Double_Window*>(v)->hide();
		}, this);
	menu.add("_Database/&Export to CSV ", FL_CTRL + 'e', [](Fl_Widget* w, void* v) {
		reinterpret_cast<MainWindow*>(v)->editor->export_CSV();
		}, this);
	menu.add("Database/&Import from CSV ", FL_CTRL + 'i', [](Fl_Widget* w, void* v) {
		reinterpret_cast<MainWindow*>(v)->editor->import_CSV();
		}, this);

	menu.add("Bought Resources", FL_CTRL + '1', [](Fl_Widget* w, void* v) {
		auto* window = reinterpret_cast<MainWindow*>(v);
		window->change_editor(std::make_unique<Editor_Bought_Resources>(0, 30, window->w(), window->h() - 30, window->db));
		}, this);
	menu.add("Buyers", FL_CTRL + '2', [](Fl_Widget* w, void* v) {
		auto* window = reinterpret_cast<MainWindow*>(v);
		window->change_editor(std::make_unique<Editor_Buyers>(0, 30, window->w(), window->h() - 30, window->db));
		}, this);
	menu.add("Collected Eggs", FL_CTRL + '3', [](Fl_Widget* w, void* v) {
		auto* window = reinterpret_cast<MainWindow*>(v);
		window->change_editor(std::make_unique<Editor_Collected_Eggs>(0, 30, window->w(), window->h() - 30, window->db));
		}, this);
	menu.add("Sold Eggs", FL_CTRL + '4', [](Fl_Widget* w, void* v) {
		auto* window = reinterpret_cast<MainWindow*>(v);
		window->change_editor(std::make_unique<Editor_Sold_Eggs>(0, 30, window->w(), window->h() - 30, window->db));
		}, this);
}

void MainWindow::change_editor(std::unique_ptr<SQL_Editor>&& new_editor)
{
	auto widgets_array = Fl_Group::array();
	remove(editor.get());
	add(new_editor.get());
	editor = std::move(new_editor);
	resizable(editor.get());
	redraw();
}

MainWindow::MainWindow(int W, int H, const char* title, SQLite::Database& db)
	: Fl_Double_Window(W, H, title), width(W), height(H), db(db)
{
	create_menu();
	Fl_Double_Window::size_range(600, 800);
	resizable(this);
	editor = std::make_unique<Editor_Collected_Eggs>(0, 30, W, H - 30, db);
	resizable(editor.get());
}
