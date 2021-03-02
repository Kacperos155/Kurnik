#include "../pch.h"
#include "Editor_Bought_Resources.h"

Editor_Bought_Resources::Editor_Bought_Resources(int X, int Y, int W, int H, SQLite::Database& db)
	: SQL_Editor(X, Y, W, H, "Bought Resources", "Bought Resources", db)
{
}

bool Editor_Bought_Resources::add_record()
{
	return false;
}

void Editor_Bought_Resources::export_CSV()
{
	try {
		SQL_Editor::export_CSV("ID, Resource, Price, Date, Shop", 5);
	}
	catch (std::exception e) {
		fl_alert(e.what());
	}
}

void Editor_Bought_Resources::import_CSV()
{
	try {
		SQL_Editor::import_CSV("ID, Resource, Price, Date, Shop", 5);
	}
	catch (std::exception e) {
		fl_alert(e.what());
	}
}
