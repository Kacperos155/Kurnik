#pragma once
#include "../pch.h"
#include "SQL_Data_Model.h"

class Bought_Resources_Model : public SQL_Data_Model
{
public:
	Bought_Resources_Model(SQLite::Database& database);
	wxSizer* create_inputs(wxWindow* parent) override;
	bool read_inputs() override;
	bool reset_input() override;
	bool loadFromSelection() override;
};

