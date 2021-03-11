#pragma once
#include "../pch.h"
#include "SQL_Data_Model.h"

class Buyers_Model : public SQL_Data_Model
{
public:
	Buyers_Model(const std::string_view table, const std::string_view view, SQLite::Database& database);
	wxSizer* create_inputs(wxWindow* parent) const override;

	// Inherited via wxDataViewVirtualListModel
	bool SetValueByRow(const wxVariant& variant, unsigned row, unsigned col) override;
};

