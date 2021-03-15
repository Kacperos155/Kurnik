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

	auto& label_position = wxSizerFlags().Center();
	auto& position = wxSizerFlags().Expand();
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

bool Collected_Eggs_Model::read_inputs()
{
	auto date = date_input->GetDate();
	auto eggs = eggs_input->GetValue();
	auto small_eggs = small_eggs_input->GetValue();

	prepareChange_ISO_Date("Date", std::move(date));
	prepareChange("Amount", eggs);
	if (small_eggs != 0)
		prepareChange("Small eggs", small_eggs);
	else
		prepareChange_NULL("Small eggs");

	return true;
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

bool Collected_Eggs_Model::loadFromSelection()
{
	if (selected_row == std::numeric_limits<unsigned>::max())
		return false;

	SQLite::Statement statement(database, fmt::format(R"(SELECT * FROM "{}" LIMIT 1 OFFSET {};)", table, selected_row));
	if (statement.executeStep() == 0)
		return false;

	try {
		std::string_view date_string = statement.getColumn(0).getText();
		auto date = wxDateTime();
		if(date.ParseISODate(date_string.data()))
			date_input->SetDate(std::move(date));

		if (!statement.getColumn(1).isNull())
			eggs_input->SetValue(statement.getColumn(1).getInt());
		if (!statement.getColumn(2).isNull())
			small_eggs_input->SetValue(statement.getColumn(2).getInt());
	}
	catch (SQLite::Exception exception) {
		SQL_Error(exception);
	}

	return true;
}
