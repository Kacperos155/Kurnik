#include "../pch.h"
#include "Editor_Buyers.h"

Editor_Buyers::Editor_Buyers(int X, int Y, int W, int H, SQLite::Database& db)
	: SQL_Editor(X, Y, W, H, "Buyers", "Buyers", db)
{
}

bool Editor_Buyers::add_record()
{
	return false;
}

void Editor_Buyers::export_CSV()
{
	try {
		SQL_Editor::export_CSV("ID, Name", 2);
	}
	catch (std::exception e) {
		fl_alert(e.what());
	}
}

void Editor_Buyers::import_CSV()
{
	try {
		SQL_Editor::import_CSV("ID, Name", 2);
	}
	catch (std::exception e) {
		fl_alert(e.what());
	}
}
