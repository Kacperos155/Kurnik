#pragma once
#include "../pch.h"
#include "../SQL_Editor.h"

class Editor_Sold_Eggs : public SQL_Editor
{
	;
public:
	Editor_Sold_Eggs(int X, int Y, int W, int H, SQLite::Database& db);
	bool add_record() final;
	void export_CSV() final;
	void import_CSV() final;
};

