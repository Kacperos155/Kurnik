#pragma once
#include "pch.h"

class SQL_Table : public Fl_Table
{
	std::vector<std::string> data;
	SQLite::Database& database;
	SQLite::Statement query;
	long long
		rows_amount = 0,
		offset = 0;
	int
		columns_amount = 0,
		visible_width = 0;
	unsigned short visible_rows_limit = 30;

	long long count_rows();
	bool load_data();
	void optimal_size(int X, int Y, int width);
	bool change_page();
	void draw_cell(int R, int C, int X, int Y, int W, int H);
	void draw_header(std::string_view s, int X, int Y, int W, int H);
	int handle(int event) override;
public:
	SQL_Table(int X, int Y, int W, int H, std::string_view sql_view_name, SQLite::Database& db);
	void draw_cell(TableContext context, int R = 0, int C = 0, int X = 0, int Y = 0, int W = 0, int H = 0) final;
	void resize(int X, int Y, int W, int H) final;

	void reload();
	void set_visible_rows(unsigned short visible_rows);
	bool prev_page(bool to_first = false);
	bool next_page(bool to_end = false);
};

