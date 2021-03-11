#include "../pch.h"
#include "Bought_Resources_Model.h"

Bought_Resources_Model::Bought_Resources_Model(const std::string_view table, const std::string_view view, SQLite::Database& database)
    : SQL_Data_Model(table, view, database)
{
    ;
}

wxSizer* Bought_Resources_Model::create_inputs(wxWindow* parent) const
{
    return inputs_sizer;
}

bool Bought_Resources_Model::SetValueByRow(const wxVariant& variant, unsigned row, unsigned col)
{
    return false;
}