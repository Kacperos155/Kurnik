#include "pch.h"
#include "Collected_Eggs_Model.h"

Collected_Eggs_Model::Collected_Eggs_Model(const std::string_view table, const std::string_view view, SQLite::Database& database)
    : SQL_Data_Model(table, view, database)
{
    ;
}

bool Collected_Eggs_Model::SetValueByRow(const wxVariant& variant, unsigned row, unsigned col)
{
    return false;
}
