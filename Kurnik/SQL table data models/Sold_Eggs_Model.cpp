#include "../pch.h"
#include "Sold_Eggs_Model.h"

Sold_Eggs_Model::Sold_Eggs_Model(const std::string_view table, const std::string_view view, SQLite::Database& database)
	: SQL_Data_Model("Sold Eggs", "Sold Eggs + Buyers", database) {}

wxSizer* Sold_Eggs_Model::create_inputs(wxWindow* parent)
{
	auto* date_label = new wxStaticText(parent, wxID_ANY, getColumns()[0]->GetTitle());
	auto* eggs_label = new wxStaticText(parent, wxID_ANY, getColumns()[1]->GetTitle());
	auto* price_label = new wxStaticText(parent, wxID_ANY, getColumns()[2]->GetTitle());
	auto* buyer_label = new wxStaticText(parent, wxID_ANY, getColumns()[3]->GetTitle());
	date_input = new wxCalendarCtrl(parent, wxID_ANY);
	eggs_input = new wxSpinCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL | wxSP_ARROW_KEYS);
	price_input = new wxSpinCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL | wxSP_ARROW_KEYS);
	buyer_input = new wxListView(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING);
	//Create buyers list
	try
	{
		buyer_input->AppendColumn("Name");
		buyer_input->InsertItem(0, "");
		auto statement = SQLite::Statement(database, "SELECT Name FROM Buyers");
		auto index = 0;

		while (statement.executeStep()) {
			auto id = buyer_input->InsertItem(++index, std::move(statement.getColumn(0).getText()));
			buyer_input->SetItemData(id, index);
		}
	}
	catch (SQLite::Exception exception) {
		SQL_Error(exception);
		return inputs_sizer;
	}

	auto& label_position = wxSizerFlags().Center().Border(wxTOP);
	auto& position = wxSizerFlags().Expand().Border(wxALL xor wxTOP);
	inputs_sizer->Add(date_label, label_position);
	inputs_sizer->Add(eggs_label, label_position);
	inputs_sizer->Add(price_label, label_position);
	inputs_sizer->Add(buyer_label, label_position);
	inputs_sizer->Add(date_input, position);
	inputs_sizer->Add(eggs_input, position);
	inputs_sizer->Add(price_input, position);
	inputs_sizer->Add(buyer_input, position);

	auto font = eggs_input->GetFont();
	auto calendar_height = date_input->GetSize().y;
	font.SetPointSize(calendar_height / 2);
	eggs_input->SetFont(font);
	price_input->SetFont(font);

	buyer_input->Bind(wxEVT_SIZE, [&](wxSizeEvent& event) {
		buyer_input->SetColumnWidth(0, event.GetSize().x);
		});
	
	eggs_input->Bind(wxEVT_SPINCTRL, [&](wxSpinEvent& event) {
		if (price_input->GetValue() == 0)
			return;
		price_input->SetValue(event.GetValue());
		});

	return inputs_sizer;
}

bool Sold_Eggs_Model::read_inputs()
{
	auto date = date_input->GetDate();
	auto eggs = eggs_input->GetValue();
	auto price = price_input->GetValue();
	auto buyer = static_cast<long>(buyer_input->GetItemData(buyer_input->GetFirstSelected()));

	prepareChange_ISO_Date("Date", std::move(date));
	prepareChange("Amount", eggs);
	prepareChange("Price", price);
	if (buyer != 0)
		prepareChange("Buyer", buyer);
	return true;
}

bool Sold_Eggs_Model::reset_input()
{
	date_input->SetDate(std::move(wxDateTime{}.SetToCurrent()));
	eggs_input->SetValue(0);
	price_input->SetValue(0);
	buyer_input->Select(buyer_input->GetFirstSelected(), false);
	return true;
}

bool Sold_Eggs_Model::loadFromSelection()
{
	if (selected_row == std::numeric_limits<unsigned>::max())
		return false;

	SQLite::Statement statement(database, fmt::format(R"(SELECT * FROM "{}" LIMIT 1 OFFSET {};)", table, selected_row));
	if (statement.executeStep() == 0)
		return false;

	try {
		std::string_view date_string = statement.getColumn(3).getText();
		auto date = wxDateTime();
		if (date.ParseISODate(date_string.data()))
			date_input->SetDate(std::move(date));

		if (!statement.getColumn(1).isNull())
			eggs_input->SetValue(statement.getColumn(1).getInt());
		if (!statement.getColumn(2).isNull())
			price_input->SetValue(statement.getColumn(2).getInt());
		if (!statement.getColumn(4).isNull()) {
			auto id = statement.getColumn(4).getInt();
			long index{};
			if (id != 0) {
				index = buyer_input->FindItem(0, id);
			}
			buyer_input->Select(index);
		}
	}
	catch (SQLite::Exception exception) {
		SQL_Error(exception);
	}

	return true;
}
