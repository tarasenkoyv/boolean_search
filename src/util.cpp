#include <util.hpp>

#include <algorithm>
#include <cstring>
#include <locale>
#include <clocale>
#include <vector>

void ir::to_lower(const std::u8string& src, std::wstring& dest) {
	std::setlocale(LC_ALL, "ru_RU.utf8");
	std::mbtowc(nullptr, 0, 0); // reset the conversion state
	auto ptr = reinterpret_cast<const char*>(src.data());
	const char* end = ptr + src.size();
	int ret;
	for (wchar_t wc; (ret = std::mbtowc(&wc, ptr, end - ptr)) > 0; ptr += ret) {
		dest += wc;
	}
	std::transform(dest.begin(), dest.end(), dest.begin(), [](wchar_t c) {return std::tolower(c, std::locale("ru_RU.utf8")); });
}

void ir::to_lower(const std::string& src, std::wstring& dest) {
	std::mbtowc(nullptr, 0, 0); // reset the conversion state
	auto ptr = reinterpret_cast<const char*>(src.data());
	const char* end = ptr + src.size();
	int ret;
	for (wchar_t wc; (ret = std::mbtowc(&wc, ptr, end - ptr)) > 0; ptr += ret) {
		dest += wc;
	}
	std::transform(dest.begin(), dest.end(), dest.begin(), [](wchar_t c) {return std::tolower(c, std::locale("ru_RU.utf8")); });
}

void ir::to_lower(const std::wstring& src, std::wstring& dest) {
	std::transform(dest.begin(), dest.end(), dest.begin(), [](wchar_t c) {return std::tolower(c); });
}