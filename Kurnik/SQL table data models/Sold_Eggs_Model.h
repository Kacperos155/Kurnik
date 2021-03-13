#pragma once
#include "../pch.h"
#include "SQL_Data_Model.h"

class Sold_Eggs_Model : public SQL_Data_Model
{
	wxCalendarCtrl* date_input = nullptr;
	wxSpinCtrl
		* eggs_input = nullptr,
		* price_input = nullptr;
	wxListView* buyer_input = nullptr;
public:
	Sold_Eggs_Model(const std::string_view table, const std::string_view view, SQLite::Database& database);
	wxSizer* create_inputs(wxWindow* parent) override;
	bool reset_input() override;
	bool addRow() override;
	bool updateSelectedRow() override;
	bool loadFromSelection() override;
};