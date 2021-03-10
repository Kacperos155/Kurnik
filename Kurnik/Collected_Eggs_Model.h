#pragma once
#include "pch.h"
#include "SQL_Data_Model.h"
class Collected_Eggs_Model : public SQL_Data_Model
{
public:
	Collected_Eggs_Model(const std::string_view table, const std::string_view view, SQLite::Database& database);
	~Collected_Eggs_Model() {
		wxMessageBox("kek");
	}

	// Inherited via wxDataViewVirtualListModel
	void GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const override;
	bool SetValueByRow(const wxVariant& variant, unsigned row, unsigned col) override;
};

