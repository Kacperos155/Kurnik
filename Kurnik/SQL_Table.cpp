#include "pch.h"
#include "SQL_Table.h"
#include <charconv>

long long SQL_Table::count_rows()
{
	columns_amount = query.getColumnCount();

	auto rows_query_s = "select count(*) from ( " + query.getQuery() + " );";
	auto rows_count_q = SQLite::Statement(database, rows_query_s);
	if (!rows_count_q.executeStep())
		return false;

	rows_amount = rows_count_q.getColumn(0).getInt64();

	rows(visible_rows_limit);
	cols(columns_amount);

	if (rows_amount <= visible_rows_limit)
		change_page();
	else
		next_page(true);
	return rows_amount;
}

bool SQL_Table::load_data()
{
	data.clear();
	auto visible_rows = rows_amount - offset;
	if (visible_rows > visible_rows_limit)
		visible_rows = visible_rows_limit;
	data.resize(visible_rows * static_cast<long long>(columns_amount));

	auto limit_s = " limit " + std::to_string(visible_rows_limit) + " offset " + std::to_string(offset);
	auto limited_query = SQLite::Statement(database, query.getQuery() + limit_s);
	auto j = 0ll;
	while (limited_query.executeStep()) {
		for (int i = 0; i < columns_amount; ++i) {
			data[j + i] = limited_query.getColumn(i).getString();
		}
		j += columns_amount;
	}
	return true;
}

void SQL_Table::optimal_size(int X, int Y, int width)
{
	fl_font(FL_HELVETICA + FL_BOLD, FL_NORMAL_SIZE);
	{
		auto max_row_index = offset + visible_rows_limit;
		int
			W = 0,
			H = 0;
		fl_measure(std::to_string(max_row_index).c_str(), W, H);
		W += FL_NORMAL_SIZE / 2;
		row_header_width(W);
		width -= W + 1;
	}

	row_height_all(FL_NORMAL_SIZE);
	std::vector<double> rows_size(columns_amount);

	for (int i = 0; i < columns_amount; ++i) {
		int
			W = 0,
			H = 0;
		auto name = std::string_view(query.getColumnName(i));
		fl_measure(name.data(), W, H);
		row_height_all((std::max)(row_height(0), H + 4));
		rows_size[i] = W;
	}

	for (int i = 0; i < columns_amount; ++i) {
		for (int j = i; j < data.size(); j += columns_amount) {
			int
				W = 0,
				H = 0;
			auto name = std::string_view(data[j]);
			fl_measure(name.data(), W, H);
			if (W > rows_size[i])
				rows_size[i] = W;
		}
	}

	double sum = 0.0;
	for (const auto& n : rows_size) {
		sum += n;
	}

	auto used_width = 0;
	for (int i = 0; i < columns_amount - 1; ++i) {
		auto column_width = static_cast<int>(rows_size[i] / sum * width);
		used_width += column_width;
		col_width(i, column_width);
	}
	col_width(columns_amount - 1, width - used_width - 1);
	Fl_Table::resize(X, Y, width + row_header_width() + 1, row_height(0) * (visible_rows_limit + 1));
}

bool SQL_Table::change_page()
{
	if (!load_data())
		return false;
	optimal_size(x(), y(), visible_width);
	return true;
}

void SQL_Table::draw_cell(int R, int C, int X, int Y, int W, int H)
{
	fl_push_clip(X, Y, W, H);
	fl_color(28 + ((R % 2 == 1) ? 0 : 1));
	fl_rectf(X, Y, W, H);

	auto index = R * columns_amount + C;
	if (index < data.size()) {
		auto text = std::string_view(data[index]);
		fl_color(FL_BLACK);
		fl_draw(text.data(), X, Y, W, H, FL_ALIGN_CENTER);
	}

	fl_color(8);
	fl_line(X + W - 1, Y, X + W - 1, Y + H);
	fl_pop_clip();
}

void SQL_Table::draw_header(std::string_view s, int X, int Y, int W, int H)
{
	fl_font(fl_font() + FL_BOLD, FL_NORMAL_SIZE);
	fl_push_clip(X, Y, W, H);
	fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
	fl_color(FL_BLACK);

	int row = -1;
	std::from_chars(s.data(), s.data() + s.size(), row);
	if (row < rows_amount)
		fl_draw(s.data(), X, Y, W, H, FL_ALIGN_CENTER);
	fl_pop_clip();
	fl_font(fl_font() - FL_BOLD, FL_NORMAL_SIZE);
}

int SQL_Table::handle(int event)
{
	return Fl_Table::handle(event);
}

SQL_Table::SQL_Table(int X, int Y, int W, int H, std::string_view sql_view_name, SQLite::Database& db)
	: Fl_Table(X, Y, W, H, sql_view_name.data()), database(db)
	, query(db, fmt::format("select * from \"{}\"", sql_view_name)), visible_width(W)
{
	labeltype(Fl_Labeltype::FL_NO_LABEL);
	clear_visible_focus(); //to_do
	count_rows();
	row_header(true);
	row_resize(false);

	col_header(true);
	col_resize(true);
	end();
}

void SQL_Table::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
	switch (context) {
	case TableContext::CONTEXT_STARTPAGE:
		fl_font(FL_HELVETICA, FL_NORMAL_SIZE);
		return;
	case TableContext::CONTEXT_CELL:
		draw_cell(R, C, X, Y, W, H);
		return;
	case TableContext::CONTEXT_COL_HEADER:
		draw_header(query.getColumnName(C), X, Y, W, H);
		return;
	case TableContext::CONTEXT_ROW_HEADER:
		draw_header(std::to_string(R + offset), X, Y, W, H);
		return;
	default:
		return;
	}
}

void SQL_Table::resize(int X, int Y, int W, int H)
{
	if (data.size())
		optimal_size(X, Y, W);
	else
		Fl_Table::resize(X, Y, W, H);
}

void SQL_Table::reload()
{
	count_rows();
	change_page();
	redraw();
}

void SQL_Table::set_visible_rows(unsigned short visible_rows)
{
	visible_rows_limit = visible_rows;
	count_rows();
}

bool SQL_Table::prev_page(bool to_first)
{
	if (offset <= 0)
		return false;
	if (to_first)
		offset = 0;
	else
		offset -= visible_rows_limit;
	return change_page();
}

bool SQL_Table::next_page(bool to_end)
{
	if (offset + visible_rows_limit >= rows_amount)
		return false;
	if (to_end) {
		offset = rows_amount - rows_amount % visible_rows_limit;
		if (offset == rows_amount && offset)
			offset -= visible_rows_limit;
	}
	else
		offset += visible_rows_limit;
	return change_page();
}