#include "pch.h"
#include "MainWindow.h"

int main(int argc, const char** argv)
{
	auto path = std::filesystem::path("Kurnik.db");
	if (!std::filesystem::exists(path)) {
		try {
			auto db = SQLite::Database(path.string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
			//SQL strings for creating database
/*			*/#include "Create_Tables.sql"
			db.exec(sql_create_table_collected_eggs);
			db.exec(sql_create_table_buyers);
			db.exec(sql_create_table_sold_eggs);
			db.exec(sql_create_table_bought_resources);
			db.exec(sql_create_view_sold_eggs_and_buyers);
		}
		catch (std::exception e) {
			fl_alert(e.what());
			return 1;
		}
	}

	auto db = SQLite::Database(path.string(), SQLite::OPEN_READWRITE);
	auto MW = MainWindow(1000, 1000, "Kurnik", db);
	MW.show();

	return Fl::run();
}