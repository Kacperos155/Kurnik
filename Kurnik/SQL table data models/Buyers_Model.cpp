#include "../pch.h"
#include "Buyers_Model.h"

Buyers_Model::Buyers_Model(const std::string_view table, const std::string_view view, SQLite::Database& database)
    : SQL_Data_Model(table, view, database)
{
    ;
}

bool Buyers_Model::SetValueByRow(const wxVariant& variant, unsigned row, unsigned col)
{
    return false;
}