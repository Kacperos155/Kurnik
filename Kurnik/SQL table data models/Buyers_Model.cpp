#include "../pch.h"
#include "Buyers_Model.h"

Buyers_Model::Buyers_Model(SQLite::Database& database)
    : SQL_Data_Model("Buyers", "Buyers", database) {}

wxSizer* Buyers_Model::create_inputs(wxWindow* parent)
{
    return inputs_sizer;
}

bool Buyers_Model::reset_input()
{
    return false;
}

bool Buyers_Model::addRow()
{
    return false;
}

bool Buyers_Model::updateSelectedRow()
{
    return false;
}

bool Buyers_Model::loadFromSelection()
{
    return false;
}
