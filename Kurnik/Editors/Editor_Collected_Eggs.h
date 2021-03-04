#pragma once
#include "../pch.h"
#include "../SQL_Editor.h"

class Editor_Collected_Eggs : public SQL_Editor
{
	Fl_Input input_date{ 0, 0, 0, 0, "Date" };
	Fl_Int_Input input_eggs{ 0, 0, 0, 0, "Eggs" };
	Fl_Int_Input input_small_eggs{ 0, 0, 0, 0, "Small Eggs" };
	Fl_End end_input_pack;
public:
	Editor_Collected_Eggs(int X, int Y, int W, int H, SQLite::Database& db);
	virtual bool add_record() final;
	void export_CSV() final;
	void import_CSV() final;
};

