#include "../pch.h"
#include "Editor_Sold_Eggs.h"

Editor_Sold_Eggs::Editor_Sold_Eggs(int X, int Y, int W, int H, SQLite::Database& db)
	: SQL_Editor(X, Y, W, H, "Sold Eggs", "Sold Eggs + Buyers", db)
{
	;
}

bool Editor_Sold_Eggs::add_record()
{
	return false;
}

void Editor_Sold_Eggs::export_CSV()
{
	try {
		SQL_Editor::export_CSV("ID, Amount, Price, Date, Buyer", 5);
	}
	catch (std::exception e) {
		fl_alert(e.what());
	}
}

void Editor_Sold_Eggs::import_CSV()
{
	try {
		SQL_Editor::import_CSV("ID, Amount, Price, Date, Buyer", 5);
	}
	catch (std::exception e) {
		fl_alert(e.what());
	}
}
