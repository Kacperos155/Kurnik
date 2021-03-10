#pragma once
#pragma warning(push)
#pragma warning(disable : 26812) // unscoped enum
#pragma warning(disable : 26495) // uninitialized variable
#pragma warning(disable : 26451) // arithmetic overflow
#pragma warning(disable : 26439) // function may not throw
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
#include <map>
#include <vector>
#include <memory>
