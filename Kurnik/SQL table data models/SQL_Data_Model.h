#pragma once
#include "../pch.h"
class SQL_Data_Model : public wxDataViewVirtualListModel
{
	std::vector<wxDataViewColumn*> view_columns;
	std::vector<std::pair<std::string, std::string>> key_value_changes;

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
	void prepareChange(std::string&& key, std::string&& value);
	void prepareChange_ISO_Date(std::string&& key, wxDateTime&& iso_date);
	void prepareChange_NULL(std::string&& key);

	template<typename Number>
	void prepareChange(std::string&& key, const Number value) noexcept {
		std::string number;
		number.resize(25);
		auto result = std::to_chars(number.data(), number.data() + number.size(), value);
		number = std::string(number.data(), result.ptr);
		prepareChange(std::move(key), std::move(number));
	}

public:
	SQL_Data_Model(const std::string_view table, const std::string_view view, SQLite::Database& database);
	[[nodiscard]] const std::vector<wxDataViewColumn*>& getColumns() const;
	[[nodiscard]] const std::string_view getTableName() const;
	[[nodiscard]] const std::string_view getViewName() const;
	[[nodiscard]] const unsigned getRowsAmount() const;
	void Reset(bool recalc = false);
	bool selectRow(unsigned int row);
	bool addRow();
	bool updateSelectedRow();
	bool deleteSelectedRow();

	[[nodiscard]] virtual wxSizer* create_inputs(wxWindow* parent) = 0;
	virtual bool read_inputs() = 0;
	virtual bool reset_input() = 0;
	virtual bool loadFromSelection() = 0;

	// Inherited via wxDataViewVirtualListModel
	unsigned int GetColumnCount() const override;
	wxString GetColumnType(unsigned int col) const override;
	void GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const override;
	bool SetValueByRow(const wxVariant& variant, unsigned row, unsigned col) override;

	static void SQL_Error(SQLite::Exception exception) noexcept;
};

