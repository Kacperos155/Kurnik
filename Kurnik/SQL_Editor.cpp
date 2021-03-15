#include "pch.h"
#include "SQL_Editor.h"
#include "SQL table data models/Collected_Eggs_Model.h"
#include "SQL table data models/Sold_Eggs_Model.h"
#include "SQL table data models/Buyers_Model.h"
#include "SQL table data models/Bought_Resources_Model.h"

SQL_Editor::SQL_Editor(wxWindow* parent, SQLite::Database& database)
	:wxNotebook(parent, wxID_ANY), database(database)
{
	init_data_models();
	init_pages();
	wxPostEvent(this, wxBookCtrlEvent(wxEVT_NOTEBOOK_PAGE_CHANGED));
}

void SQL_Editor::init_data_models()
{
	data_models.emplace(std::make_pair("Collected Eggs",
		new Collected_Eggs_Model(database)));
	data_models.emplace(std::make_pair("Sold Eggs",
		new Sold_Eggs_Model("Sold Eggs", "Sold Eggs + Buyers", database)));
	data_models.emplace(std::make_pair("Buyers",
		new Buyers_Model(database)));
	data_models.emplace(std::make_pair("Bought Resources",
		new Bought_Resources_Model(database)));
}

void SQL_Editor::init_pages()
{
	for (const auto& [model_name, model] : data_models) {
		auto* panel = new wxPanel(this);
		auto* data_view = new wxDataViewCtrl(panel, -1, wxDefaultPosition, wxDefaultSize,
			wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_HORIZ_RULES);

		data_view->AssociateModel(model);
		data_view->SetMinSize(wxSize(100, 400));

		for (const auto& col : model->getColumns()) {
			data_view->AppendColumn(col);
		}

		auto* main_vertical_sizer = new wxBoxSizer(wxVERTICAL);
		main_vertical_sizer->Add(data_view, wxSizerFlags(2).Expand());
		panel->SetName(model_name);

		main_vertical_sizer->Add(model->create_inputs(panel), wxSizerFlags().Center().Border());

		main_vertical_sizer->Add(init_buttons(panel, data_models[model_name]), wxSizerFlags().Right());
		panel->SetSizerAndFit(main_vertical_sizer);

		AddPage(panel, model_name);

		data_view->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, [&](wxDataViewEvent& event) {
			auto* data_view = reinterpret_cast<wxDataViewCtrl*>(event.GetEventObject());
			auto selection_ID = data_view->GetSelection();
			if (!selection_ID.IsOk())
				return;
			auto selection = reinterpret_cast<std::size_t>(selection_ID.GetID());

			wxLogInfo(fmt::format("Selected row: {}", selection).c_str());
			if (selection == wxNOT_FOUND)
				return;

			reinterpret_cast<SQL_Data_Model*>(data_view->GetModel())->selectRow(static_cast<unsigned>(selection));
			});
	}

	Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, [&](wxBookCtrlEvent&) {
		std::string page = GetCurrentPage()->GetName().ToStdString();
		active_data_model = data_models[page];
		auto status = fmt::format("Selected view: {} - {} rows", page, data_models[page]->getRowsAmount());
		static_cast<wxFrame*>(GetParent())->SetStatusText(status);
		});
}

wxSizer* SQL_Editor::init_buttons(wxPanel* parent, SQL_Data_Model* data_model)
{
	auto* buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
	auto* add_button = new wxButton(parent, wxID_ADD);
	auto* modify_button = new wxButton(parent, wxID_APPLY);
	auto* cancel_button = new wxButton(parent, wxID_CANCEL);
	auto* delete_button = new wxButton(parent, wxID_DELETE);

	buttons_sizer->Add(add_button);
	buttons_sizer->Add(modify_button);
	buttons_sizer->Add(cancel_button);
	buttons_sizer->Add(delete_button);

	add_button->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		reinterpret_cast<SQL_Data_Model*>(event.GetEventUserData())->addRow();
		}, wxID_ADD, wxID_ADD, reinterpret_cast<wxObject*>(data_model));

	modify_button->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		reinterpret_cast<SQL_Data_Model*>(event.GetEventUserData())->updateSelectedRow();
		}, wxID_APPLY, wxID_APPLY, reinterpret_cast<wxObject*>(data_model));
	
	cancel_button->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		reinterpret_cast<SQL_Data_Model*>(event.GetEventUserData())->reset_input();
		}, wxID_CANCEL, wxID_CANCEL, reinterpret_cast<wxObject*>(data_model));

	delete_button->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		reinterpret_cast<SQL_Data_Model*>(event.GetEventUserData())->deleteSelectedRow();
		}, wxID_DELETE, wxID_DELETE, reinterpret_cast<wxObject*>(data_model));

	return buttons_sizer;
}

void SQL_Editor::recreate_table()
{
/*	*/#include "Create_Tables.sql"
	auto& table = active_data_model->getTableName();
	auto check_table_sql = [&table](std::string_view sql) {
		auto it = sql.find(table);
		return (it != std::string_view::npos) ? true : false;
	};

	if (check_table_sql(sql_create_table_collected_eggs)) {
		database.exec(sql_create_table_collected_eggs);
	}
	else if (check_table_sql(sql_create_table_sold_eggs)) {
		database.exec(sql_create_table_sold_eggs);
	}
	else if (check_table_sql(sql_create_table_buyers)) {
		database.exec(sql_create_table_buyers);
	}
	else if (check_table_sql(sql_create_table_bought_resources)) {
		database.exec(sql_create_table_bought_resources);
	}
}

bool SQL_Editor::export_CSV(char delimiter)
{
	if (active_data_model == nullptr)
		return false;
	auto table_name = active_data_model->getTableName().data();
	auto file_chooser = wxFileDialog(this, fmt::format("{} - Export", table_name), "",
		table_name, "*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (file_chooser.ShowModal() == wxID_CANCEL)
		return false;

	auto statement = fmt::format("select * from \"{}\";", table_name);
	std::ofstream file(file_chooser.GetPath().ToStdString());
	if (!file.is_open())
		return false;

	try {
		auto query = SQLite::Statement(database, statement);
		auto columns_amount = query.getColumnCount();

		for (int i = 0; i < columns_amount; ++i) {
			if (i != 0)
				file << delimiter;
			file << query.getColumnName(i);
		}

		while (query.executeStep()) {
			file << '\n';
			for (int i = 0; i < columns_amount; ++i) {
				if (i != 0)
					file << delimiter;
				file << query.getColumn(i);
			}
		}
	}
	catch (SQLite::Exception exception) {
		SQL_Data_Model::SQL_Error(exception);
	}
	return true;
}

bool SQL_Editor::import_CSV(char delimiter)
{
	if (active_data_model == nullptr)
		return false;
	auto table_name = active_data_model->getTableName().data();
	auto file_chooser = wxFileDialog(this, fmt::format("{} - Export", table_name), "",
		table_name, "*.csv", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (file_chooser.ShowModal() == wxID_CANCEL)
		return false;

	std::string values{};
	auto columns_amount = 0;
	try
	{
		auto columns = SQLite::Statement(database, fmt::format("select * from \"{}\"", table_name));
		columns_amount = columns.getColumnCount();
		values = fmt::format("\"{}\"", columns.getColumnName(0));
		for (int i = 1; i < columns.getColumnCount(); ++i) {
			values += fmt::format(", \"{}\"", columns.getColumnName(i));
		}
	}
	catch (SQLite::Exception exception) {
		SQL_Data_Model::SQL_Error(exception);
	}

	auto statement = fmt::format("Insert into \"{}\" ({}) VALUES (", table_name, std::move(values));
	for (int i = 0; i < columns_amount - 1; ++i)
		statement += "?, ";
	statement += "?);";

	std::ifstream file(file_chooser.GetPath().ToStdString());
	if (!file.is_open())
		return false;

	SQLite::Transaction T(database);
	recreate_table();
	std::string line;

	for (std::getline(file, line); std::getline(file, line) && line.size();) {
		SQLite::Statement insert(database, statement);

		auto end = size_t{ 0 };
		for (int i = 1; i <= columns_amount; ++i) {
			auto start = end + ((i != 1) ? 1 : 0);
			end = line.find_first_of(delimiter, start);
			auto fragment = line.substr(start, end - start);
			if (fragment.size())
				insert.bind(i, std::move(fragment));
		}
		try {
			insert.exec();
		}
		catch (SQLite::Exception exception) {
			SQL_Data_Model::SQL_Error(exception);
		}
	}
	T.commit();
	active_data_model->Reset(true);
	return true;
}
