#include "pch.h"
#include "MainWindow.h"

class Kurnik_App : public wxApp {
	bool database_connection_good = true;
	SQLite::Database database;
public:
	bool OnInit() override {
		if (!database_connection_good)
			return false;
#ifdef _DEBUG
		auto* log = new wxLogWindow(nullptr, "Log");
		log->PassMessages(false);
#else
		auto* log = new wxLogChain(nullptr);
		log->PassMessages(false);
#endif // _DEBUG
		auto* MW = new MainWindow("Kurnik", database);
		MW->Show();
		return true;
	}

	Kurnik_App()
		: database(":memory:")
	{
		auto path = std::filesystem::path("Kurnik.db");
		try {
			if (!std::filesystem::exists(path)) {
				auto db = SQLite::Database(path.string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
				//SQL strings for creating database
/*				*/#include "Create_Tables.sql"
				db.exec(sql_create_table_collected_eggs);
				db.exec(sql_create_table_buyers);
				db.exec(sql_create_table_sold_eggs);
				db.exec(sql_create_table_bought_resources);
				db.exec(sql_create_view_sold_eggs_and_buyers);
			}
			database = SQLite::Database(path.string(), SQLite::OPEN_READWRITE);
		}
		catch (std::exception e) {
			wxMessageBox(e.what());
			database_connection_good = false;
		}
	}
};

wxIMPLEMENT_APP(Kurnik_App);