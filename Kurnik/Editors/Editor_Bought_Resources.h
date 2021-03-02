#pragma once
#include "../pch.h"
#include "../SQL_Editor.h"

class Editor_Bought_Resources : public SQL_Editor
{
	;
public:
	Editor_Bought_Resources(int X, int Y, int W, int H, SQLite::Database& db);
	virtual bool add_record() final;
	void export_CSV() final;
	void import_CSV() final;
};

