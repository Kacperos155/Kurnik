#pragma once
class SQL_Data_Model : public wxDataViewVirtualListModel
{
	std::vector<wxDataViewColumn*> view_columns;

	const long long getRowsAmount();
protected:
	SQLite::Database& database;
	std::string_view
		table,
		view;
	const long long rows_amount = getRowsAmount();

	void initColumns();
	void fillCellValue(wxVariant& variant, std::string_view query) const;
public:
	SQL_Data_Model(const std::string_view table, const std::string_view view, SQLite::Database& database);
	const std::vector<wxDataViewColumn*>& getColumns() const;

	// Inherited via wxDataViewVirtualListModel
	unsigned int GetColumnCount() const override;
	wxString GetColumnType(unsigned int col) const override;
	void GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const override = 0;
	bool SetValueByRow(const wxVariant& variant, unsigned row, unsigned col) override = 0;
};

