#include "../pch.h"
#include "Sold_Eggs_Model.h"

Sold_Eggs_Model::Sold_Eggs_Model(const std::string_view table, const std::string_view view, SQLite::Database& database)
    : SQL_Data_Model(table, view, database)
{
    ;
}

wxSizer* Sold_Eggs_Model::create_inputs(wxWindow* parent) const
{
    return inputs_sizer;
}

bool Sold_Eggs_Model::SetValueByRow(const wxVariant& variant, unsigned row, unsigned col)
{
    return false;
}