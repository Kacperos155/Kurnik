#pragma once
#pragma warning(push)
#pragma warning(disable : 26812)
#pragma warning(disable : 6308)
#pragma warning(disable : 1256)
//wxWidgets
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "rpcrt4.lib")
#include <wx/wx.h>
#include <wx/dataview.h>
//Other libs
#include <fmt/format.h>
#include <SQLiteCpp/SQLiteCpp.h>
#pragma warning(pop)
//STL
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <memory>
