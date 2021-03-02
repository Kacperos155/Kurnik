#include "../pch.h"
#include "Editor_Collected_Eggs.h"

Editor_Collected_Eggs::Editor_Collected_Eggs(int X, int Y, int W, int H, SQLite::Database& db)
	: SQL_Editor(X, Y, W, H, "Collected Eggs", "Collected Eggs", db)
{
	;
}

bool Editor_Collected_Eggs::add_record()
{
	return false;
}

void Editor_Collected_Eggs::export_CSV()
{
	try {
		SQL_Editor::export_CSV("Date, Amount, \"Small eggs\"", 3);
	}
	catch (std::exception e) {
		fl_alert(e.what());
	}
}

void Editor_Collected_Eggs::import_CSV()
{
	try {
		SQL_Editor::import_CSV("Date, Amount, \"Small eggs\"", 3);
	}
	catch (std::exception e) {
		fl_alert(e.what());
	}
}
