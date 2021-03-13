#include "../pch.h"
#include "Bought_Resources_Model.h"

Bought_Resources_Model::Bought_Resources_Model(SQLite::Database& database)
    : SQL_Data_Model("Bought Resources", "Bought Resources", database) {}

wxSizer* Bought_Resources_Model::create_inputs(wxWindow* parent)
{
    return inputs_sizer;
}

bool Bought_Resources_Model::read_inputs()
{
    return false;
}

bool Bought_Resources_Model::reset_input()
{
    return false;
}

bool Bought_Resources_Model::loadFromSelection()
{
    return false;
}
