#include <parser.hpp>

#include <cassert>
#include <iostream>
#include <fstream>
#include <cmath>
#include <locale>
#include <sstream>
#include <iomanip>
#include <cstring>

#include <tokenizer.hpp>

void ir::process_dataset(const std::vector<std::string>& file_list, doc_url_index& doc_url_index, doc_term_index& doc_term_index) {
    std:: cout << "Start processing dataset." << std::endl;
    size_t doc_id_start{1};
    auto cnt_files = file_list.size();
    for (size_t i = 0; i < cnt_files; ++i) {
        std::ifstream ifs(file_list[i], std::ios_base::binary);
        if(!ifs.is_open()) {
            std::cout << "File " << file_list[i] << " is not opened: " << std::string(strerror(errno)) << std::endl;
            continue;
        }
        auto processed_docs = ir::parse_file(ifs, doc_url_index, doc_term_index, doc_id_start);
        doc_id_start += processed_docs;
        std:: cout << (i + 1) << "/" << cnt_files << std::endl;
    }
}

size_t ir::parse_file(std::istream& ifs, doc_url_index& doc_url_index, doc_term_index& doc_term_index,
                      size_t doc_id_start) {
    static ir::Tokenizer tokenizer{};
    size_t processed_docs{0};
    size_t doc_id{doc_id_start};
    while (true) {
        if (!ifs.good()) break;
        char doc_lenght_info_c[ir::N_BYTES_INT_LENGTH];
        for (auto i = 0; i < ir::N_BYTES_INT_LENGTH; ++i) {
            doc_lenght_info_c[i] = ifs.get();
        }
        auto doc_info_lenght = ir::get_length(doc_lenght_info_c, ir::N_BYTES_INT_LENGTH);

        // skip '\n'
        ifs.get();
        size_t url_length{static_cast<char8_t>(ifs.get())};
        auto doc_url_c = new char8_t[url_length];
        ifs.read(reinterpret_cast<char*>(doc_url_c), url_length);
        
        // skip '\x1a'
        ifs.get();

        int all_text_length = doc_info_lenght - url_length - ir::N_SYSTEM_BYTES;
        if (all_text_length > 1) {
            // skip 2 bytes
            ifs.get();
            ifs.get();

            auto text_length = all_text_length - 2;
            auto doc_text_c = new char8_t[text_length];

            ifs.read(reinterpret_cast<char*>(doc_text_c), text_length);
            
            std::u8string doc_text(doc_text_c, text_length);

            std::set<std::wstring> terms;
            tokenizer.tokenize(doc_text, terms);
            doc_term_index.emplace(std::move(doc_id), std::move(terms));
        }
        else {
            ifs.get();
            //doc_term_index[doc_id] = nullptr;
        }

        doc_url_index.emplace(std::move(doc_id), std::u8string{ doc_url_c, url_length });
        ++doc_id;
        ++processed_docs;
    }

    return processed_docs;
}

size_t ir::get_length(char* lenght_c, size_t n_bytes) {
    size_t result{ 0 };
    for (auto i = 0; i < n_bytes; ++i) {
        result += static_cast<uint8_t>(lenght_c[i]) * std::pow(2, 8 * i);
    }
    return result;
}
