#pragma once
#include "../pch.h"
class SQL_Data_Model : public wxDataViewVirtualListModel
{
	std::vector<wxDataViewColumn*> view_columns;

	void initColumns();
	const unsigned calcRowsAmount();

protected:
	SQLite::Database& database;
	std::string_view
		table,
		view;
	unsigned int rows_amount = calcRowsAmount();

	void fillCellValue(wxVariant& variant, std::string_view query) const;

public:
	SQL_Data_Model(const std::string_view table, const std::string_view view, SQLite::Database& database);
	const std::vector<wxDataViewColumn*>& getColumns() const;
	const std::string_view getTableName() const;
	const std::string_view getViewName() const;
	const unsigned getRowsAmount() const;
	void Reset();

	// Inherited via wxDataViewVirtualListModel
	unsigned int GetColumnCount() const override;
	wxString GetColumnType(unsigned int col) const override;
	void GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const override;
	bool SetValueByRow(const wxVariant& variant, unsigned row, unsigned col) override = 0;
};

