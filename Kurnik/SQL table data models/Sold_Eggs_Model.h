#pragma once
#include "../pch.h"
#include "SQL_Data_Model.h"

class Sold_Eggs_Model : public SQL_Data_Model
{
public:
	Sold_Eggs_Model(const std::string_view table, const std::string_view view, SQLite::Database& database);

	// Inherited via wxDataViewVirtualListModel
	bool SetValueByRow(const wxVariant& variant, unsigned row, unsigned col) override;
};