#pragma once
#include "pch.h"

class SQL_Table
{
	std::vector<std::string> data;
	SQLite::Database& database;
	SQLite::Statement query;
	long long
		rows_amount = 0,
		offset = 0;
	int
		columns_amount = 0;
	unsigned short visible_rows_limit = 30;

	long long count_rows();
	bool load_data();
	void optimal_size();
	bool change_page();
public:
	SQL_Table(std::string_view sql_view_name, SQLite::Database& db);

	void reload();
	void set_visible_rows(unsigned short visible_rows);
	bool prev_page(bool to_first = false);
	bool next_page(bool to_end = false);
};

