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
	unsigned int
		rows_amount = calcRowsAmount(),
		selected_row = std::numeric_limits<unsigned>::max();
	wxFlexGridSizer* inputs_sizer = nullptr;

	[[nodiscard]] const std::pair<std::string, std::string> getRowValues(unsigned row = std::numeric_limits<unsigned>::max()) const;
	bool updateSelectedRow_(std::string&& changes);

public:
	SQL_Data_Model(const std::string_view table, const std::string_view view, SQLite::Database& database);
	[[nodiscard]] const std::vector<wxDataViewColumn*>& getColumns() const;
	[[nodiscard]] const std::string_view getTableName() const;
	[[nodiscard]] const std::string_view getViewName() const;
	[[nodiscard]] const unsigned getRowsAmount() const;
	void Reset(bool recalc = false);
	[[nodiscard]] virtual wxSizer* create_inputs(wxWindow* parent) = 0;
	bool selectRow(unsigned int row);
	virtual bool reset_input() = 0;
	virtual bool addRow() = 0;
	virtual bool updateSelectedRow() = 0;
	virtual bool loadFromSelection() = 0;
	bool deleteSelectedRow();

	// Inherited via wxDataViewVirtualListModel
	unsigned int GetColumnCount() const override;
	wxString GetColumnType(unsigned int col) const override;
	void GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const override;
	bool SetValueByRow(const wxVariant& variant, unsigned row, unsigned col) override;

	static void SQL_Error(SQLite::Exception exception) noexcept;
};

