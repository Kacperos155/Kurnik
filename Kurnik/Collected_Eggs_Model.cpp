#include "pch.h"
#include "Collected_Eggs_Model.h"

Collected_Eggs_Model::Collected_Eggs_Model(const std::string_view table, const std::string_view view, SQLite::Database& database)
    : SQL_Data_Model(table, view, database)
{
    Reset(rows_amount);
}

void Collected_Eggs_Model::GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const
{
    auto columns = std::array{ "Date", "Amount", "Small eggs" };
    auto query = fmt::format("select \"{}\" from \"{}\" limit 1 offset {}", columns[col], view, rows_amount - row - 1);
    fillCellValue(variant, query);
}

bool Collected_Eggs_Model::SetValueByRow(const wxVariant& variant, unsigned row, unsigned col)
{
    return false;
}
