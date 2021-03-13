#pragma once
#include "../pch.h"
#include "SQL_Data_Model.h"

class Bought_Resources_Model : public SQL_Data_Model
{
public:
	Bought_Resources_Model(SQLite::Database& database);
	wxSizer* create_inputs(wxWindow* parent) override;
	bool reset_input() override;
	bool addRow() override;
	bool updateSelectedRow() override;
	bool loadFromSelection() override;
};
