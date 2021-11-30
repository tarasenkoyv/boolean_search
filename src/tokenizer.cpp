#include <tokenizer.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <vector>
#include <set>
#include <memory>

#include <util.hpp>
#include <srell/srell.hpp>

void ir::Tokenizer::tokenize(std::u8string& text, std::set<std::u8string>& result) {
    static const srell::u8regex re(u8R"([\p{L}\p{N}]+)");
    srell::u8sregex_token_iterator it{text.cbegin(), text.cend(), re, 0};
    std::set<std::u8string> tokenized{ it, {} };
    result.insert(tokenized.cbegin(), tokenized.cend());
}

void ir::Tokenizer::tokenize(std::u8string& text, std::set<std::wstring>& result) {
    static const srell::u8regex re(u8R"([\p{L}\p{N}]+)");
    srell::u8sregex_token_iterator it{ text.cbegin(), text.cend(), re, 0 };
    std::set<std::u8string> tokenized{ it, {} };
    for (auto& u8str : tokenized) {
        std::wstring dest;
        normalize(u8str, dest);
        result.emplace(std::move(dest));
    }
}

void ir::Tokenizer::normalize(const std::u8string& src, std::wstring& dest) {
    ir::to_lower(src, dest);
}

