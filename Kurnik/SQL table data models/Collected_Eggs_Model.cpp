#include "../pch.h"
#include "Collected_Eggs_Model.h"

Collected_Eggs_Model::Collected_Eggs_Model(const std::string_view table, const std::string_view view, SQLite::Database& database)
    : SQL_Data_Model(table, view, database)
{
    ;
}

wxSizer* Collected_Eggs_Model::create_inputs(wxWindow* parent) const
{
	auto* date_label = new wxStaticText(parent, wxID_ANY, getColumns()[0]->GetTitle());
	auto* eggs_label = new wxStaticText(parent, wxID_ANY, getColumns()[1]->GetTitle());
	auto* small_eggs_label = new wxStaticText(parent, wxID_ANY, getColumns()[2]->GetTitle());
	auto* date_input = new wxCalendarCtrl(parent, wxID_ANY);
	auto* eggs_input = new wxSpinCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL | wxSP_ARROW_KEYS);
	auto* small_eggs_input = new wxSpinCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL | wxSP_ARROW_KEYS);

	auto& label_position = wxSizerFlags().Center().Border(wxTOP);
	auto& position = wxSizerFlags().Expand().Center().Shaped();
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

bool Collected_Eggs_Model::SetValueByRow(const wxVariant& variant, unsigned row, unsigned col)
{
    return false;
}
