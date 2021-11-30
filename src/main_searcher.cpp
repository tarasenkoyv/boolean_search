#include <algorithm>
#include <fstream>
#include <iostream>
#include <clocale>
#include <locale>
#include <regex>

#include <tokenizer.hpp>
#include <file_manager.hpp>
#include <parser.hpp>
#include <codec.hpp>
#include <util.hpp>
#include <query_tree.hpp>

#ifndef MS_STDLIB_BUGS // Allow overriding the autodetection.
/* The Microsoft C and C++ runtime libraries that ship with Visual Studio, as
 * of 2017, have a bug that neither stdio, iostreams or wide iostreams can
 * handle Unicode input or output.  Windows needs some non-standard magic to
 * work around that.  This includes programs compiled with MinGW and Clang
 * for the win32 and win64 targets.
 *
 * NOTE TO USERS OF TDM-GCC: This code is known to break on tdm-gcc 4.9.2. As
 * a workaround, "-D MS_STDLIB_BUGS=0" will at least get it to compile, but
 * Unicode output will still not work.
 */
#  if ( _MSC_VER || __MINGW32__ || __MSVCRT__ )
 /* This code is being compiled either on MS Visual C++, or MinGW, or
  * clang++ in compatibility mode for either, or is being linked to the
  * msvcrt (Microsoft Visual C RunTime) library.
  */
#    define MS_STDLIB_BUGS 1
#  else
#    define MS_STDLIB_BUGS 0
#  endif
#endif

#if MS_STDLIB_BUGS
#include <Windows.h>
#endif

int main(int argc, char** argv) {
	try {
		ir::varbyte_codec codec;
		ir::term_id_map dict;
		ir::doc_url_index doc_url_index;
		ir::inv_index_opt inv_index_opt;

		ir::read_wdict_file(dict, ir::DICT_PATH);
		ir::read_index_file(inv_index_opt, ir::INDEX_PATH, codec);
		ir::read_doc_url_file(doc_url_index, ir::DOC_URL_PATH, codec);

#if MS_STDLIB_BUGS
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);
#endif
		while (true) {
			std::string query;
			std::getline(std::cin, query);
            if (query.empty()) return 0;
			std::wstring query_lower;
			ir::to_lower(query, query_lower);
			ir::query_tree q(query_lower);
			std::vector<std::u8string> urls;
			q.build_tree();
			q.build_posting_lists(inv_index_opt, dict);
			q.get_urls(doc_url_index, urls);
			std::cout << query << std::endl;
			std::cout << urls.size() << std::endl;
			for (auto& url : urls) {
				std::cout << std::string(url.cbegin(), url.cend()) << std::endl;
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << "Fatal error" << e.what() << std::endl;
	}
	return 0;
}