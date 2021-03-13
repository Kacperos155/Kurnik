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
#include <wx/notebook.h>
#include <wx/calctrl.h>
#include <wx/spinctrl.h>
#include <wx/listctrl.h>
//Other libs
#include <fmt/format.h>
#include <SQLiteCpp/SQLiteCpp.h>
#pragma warning(pop)
//STL
#include <limits>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <charconv>
#include <array>
#include <map>
#include <vector>
#include <memory>
