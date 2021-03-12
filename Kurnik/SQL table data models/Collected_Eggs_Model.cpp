#include "../pch.h"
#include "Collected_Eggs_Model.h"

Collected_Eggs_Model::Collected_Eggs_Model(SQLite::Database& database)
	: SQL_Data_Model("Collected Eggs", "Collected Eggs", database) {}

wxSizer* Collected_Eggs_Model::create_inputs(wxWindow* parent)
{
	auto* date_label = new wxStaticText(parent, wxID_ANY, getColumns()[0]->GetTitle());
	auto* eggs_label = new wxStaticText(parent, wxID_ANY, getColumns()[1]->GetTitle());
	auto* small_eggs_label = new wxStaticText(parent, wxID_ANY, getColumns()[2]->GetTitle());
	date_input = new wxCalendarCtrl(parent, wxID_ANY);
	eggs_input = new wxSpinCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL | wxSP_ARROW_KEYS);
	small_eggs_input = new wxSpinCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL | wxSP_ARROW_KEYS);

	auto& label_position = wxSizerFlags().Center().Border(wxTOP);
	auto& position = wxSizerFlags().Center().Shaped();
	inputs_sizer->Add(date_label, label_position);
	inputs_sizer->Add(eggs_label, label_position);
	inputs_sizer->Add(small_eggs_label, label_position);
	inputs_sizer->Add(date_input, position);
	inputs_sizer->Add(eggs_input, position);
	inputs_sizer->Add(small_eggs_input, position);

	auto font = eggs_input->GetFont();
	auto calendar_height = date_input->GetSize().y;
	font.SetPointSize(calendar_height / 2);
	eggs_input->SetFont(font);
	small_eggs_input->SetFont(font);
	return inputs_sizer;
}

bool Collected_Eggs_Model::reset_input()
{
	auto date = wxDateTime();
	date.SetToCurrent();
	date_input->SetDate(date);
	eggs_input->SetValue(0);
	small_eggs_input->SetValue(0);
	return true;
}

bool Collected_Eggs_Model::addRow()
{
	auto date = date_input->GetDate();
	auto eggs = eggs_input->GetValue();
	auto small_eggs = small_eggs_input->GetValue();

	auto statement = SQLite::Statement(database, R"(INSERT INTO "Collected Eggs"(Date, Amount, "Small eggs") VALUES (?,?,?);)");
	try {
		statement.bind(1, fmt::format("{}-{:0>2}-{:0>2}", date.GetYear(), date.GetMonth() + 1, date.GetDay()));
		statement.bind(2, eggs);
		if (small_eggs != 0)
			statement.bind(3, small_eggs);
		wxLogInfo(statement.getExpandedSQL().c_str());
		statement.exec();
	}
	catch (std::exception e) {
		SQL_Error(e);
		return false;
	}
	++rows_amount;
	Reset();
	return true;
}

bool Collected_Eggs_Model::updateSelectedRow()
{
	auto date = date_input->GetDate();
	auto eggs = eggs_input->GetValue();
	auto small_eggs = small_eggs_input->GetValue();

	auto changes = fmt::format(R"("{}" = '{}-{:0>2}-{:0>2}')", "Date", date.GetYear(), date.GetMonth() + 1, date.GetDay());
	if(eggs != 0)
		changes+= fmt::format(",\n\"{}\" = '{}'", "Amount", eggs);
	if(small_eggs != 0)
		changes+= fmt::format(",\n\"{}\" = '{}'", "Amount", small_eggs);
	return updateSelectedRow_(std::move(changes));
}

bool Collected_Eggs_Model::loadFromSelection()
{
	if (selected_row == std::numeric_limits<unsigned>::max())
		return false;

	auto string_view_to_short = [](std::string_view sv) {
		short x;
		std::from_chars(sv.data(), sv.data() + sv.size(), x);
		return x;
	};

	SQLite::Statement statement(database, fmt::format(R"(SELECT * FROM "{}" LIMIT 1 OFFSET {};)", table, selected_row));
	if (statement.executeStep() == 0)
		return false;

	try {
		std::string_view date_string = statement.getColumn(0).getText();
		auto date = wxDateTime();
		if(date.ParseISODate(date_string.data()))
			date_input->SetDate(std::move(date));
		/*date.SetYear(string_view_to_short(date_string.substr(0, 4)));
		date.SetMonth(string_view_to_short(reinterpret_cast<wxDateTime::Month>(date_string.substr(0, 4))));
		date.SetDay(string_view_to_short(date_string.substr(0, 4)));*/

		if (!statement.getColumn(1).isNull())
			eggs_input->SetValue(statement.getColumn(1).getInt());
		if (!statement.getColumn(2).isNull())
			small_eggs_input->SetValue(statement.getColumn(2).getInt());
	}
	catch (std::exception e) {
		SQL_Error(e);
	}

	return true;
}
