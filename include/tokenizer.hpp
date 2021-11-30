#pragma once

#include "defs.hpp"
#include <string>
#include <vector>
#include <set>
#include <unordered_map>

namespace ir {

class Tokenizer {
public:
	void tokenize(std::u8string& text, std::set<std::u8string>& result);
	void tokenize(std::u8string& text, std::set<std::wstring>& result);
	void normalize(const std::u8string& src, std::wstring& dest);
};

} // namespace ir