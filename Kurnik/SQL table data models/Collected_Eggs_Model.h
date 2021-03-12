#pragma once
#include "../pch.h"
#include "SQL_Data_Model.h"

class Collected_Eggs_Model : public SQL_Data_Model
{
	wxCalendarCtrl* date_input = nullptr;
	wxSpinCtrl* eggs_input = nullptr;
	wxSpinCtrl* small_eggs_input = nullptr;
public:
	Collected_Eggs_Model(SQLite::Database& database);
	wxSizer* create_inputs(wxWindow* parent) override;
	bool reset_input() override;
	bool addRow() override;
	bool updateSelectedRow() override;
	bool loadFromSelection() override;
};

