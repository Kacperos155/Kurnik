#pragma once
#include "../pch.h"
class SQL_Data_Model : public wxDataViewVirtualListModel
{
	using columns_values = std::unordered_map<std::string, std::string>;
	columns_values values_to_change;
	std::vector<wxDataViewColumn*> view_columns;

	void initColumns();
	const unsigned calcRowsAmount();
	[[nodiscard]] const std::string create_WHERE(columns_values& values) const;

protected:
	SQLite::Database& database;
	std::string_view
		table,
		view;
	unsigned int
		rows_amount = calcRowsAmount(),
		selected_row = std::numeric_limits<unsigned>::max();
	wxFlexGridSizer* inputs_sizer = nullptr;

	[[nodiscard]] const columns_values getRowValues(unsigned row = std::numeric_limits<unsigned>::max()) const;

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

