#include "../pch.h"
#include "Sold_Eggs_Model.h"

Sold_Eggs_Model::Sold_Eggs_Model(const std::string_view table, const std::string_view view, SQLite::Database& database)
    : SQL_Data_Model("Sold Eggs", "Sold Eggs + Buyers", database) {}

wxSizer* Sold_Eggs_Model::create_inputs(wxWindow* parent)
{
    return inputs_sizer;
}

bool Sold_Eggs_Model::reset_input()
{
    return false;
}

bool Sold_Eggs_Model::addRow()
{
    return false;
}

bool Sold_Eggs_Model::updateSelectedRow()
{
    return false;
}

bool Sold_Eggs_Model::loadFromSelection()
{
    return false;
}
