#pragma once

#include <cstring>
#include <string>

namespace ir {
	void to_lower(const std::u8string& src, std::wstring& dest);
	void to_lower(const std::wstring& src, std::wstring& dest);
    void to_lower(const std::string& src, std::wstring& dest);
}