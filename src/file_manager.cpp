#include <file_manager.hpp>

#include <algorithm>
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

#include <codec.hpp>
#include <parser.hpp>

std::vector<std::string> ir::get_data_file_list() {
    std::vector<std::string> file_list;

    for (auto& filename : std::filesystem::recursive_directory_iterator(DATASET_DIR.c_str())) {
        file_list.push_back(filename.path().generic_string());
    }
    std::sort(file_list.begin(), file_list.end());
    return file_list;
}

void ir::write_dict_file(const ir::u8term_id_map& dict, const std::string& filename, ir::codec_base& codec) {
    std::ofstream ofs;
    ofs.open(filename, std::ios::binary);

    for (const auto& [key, value] : dict) {
        ofs.put(key.size());
        ofs.write(reinterpret_cast<const char*>(key.data()), key.size());
        std::vector<uint8_t> value_encoded{};
        codec.encode(value, value_encoded);
        ofs.put(value_encoded.size());
        ofs.write(reinterpret_cast<const char*>(value_encoded.data()), value_encoded.size());
        ofs.put('\n');
    }
}

void ir::write_wdict_file(const ir::term_id_map& dict, const std::string& filename) {
    std::locale::global(std::locale(""));
    std::wofstream ofs;
    ofs.open(filename, std::ios::out | std::ios::binary);
    
    for (const auto& [word, term_id] : dict) {
        ofs.put(static_cast<wchar_t>(word.size()));
        ofs << word;
        auto wterm_id = std::to_wstring(term_id);
        ofs.put(static_cast<wchar_t>(wterm_id.size()));
        ofs << wterm_id;
        ofs << '\n';
    }
}

void ir::write_index_file(const ir::inv_index_opt& inverted_index, const std::string& filename, ir::codec_base& codec) {
    std::ofstream ofs;
    ofs.open(filename, std::ios::binary);

    for (const auto& [term_id, posting_list] : inverted_index) {
        std::vector<uint8_t> key_encoded;
        codec.encode(term_id, key_encoded);
        ofs.put(key_encoded.size());
        ofs.write(reinterpret_cast<const char*>(key_encoded.data()), key_encoded.size());

        char pl_size_c[ir::N_BYTES_INT_LENGTH];
        auto pl_size = posting_list.size();
        ir::serialize_int(pl_size, pl_size_c, ir::N_BYTES_INT_LENGTH);
        ofs.write(pl_size_c, ir::N_BYTES_INT_LENGTH);
        ofs.write(reinterpret_cast<const char*>(posting_list.data()), pl_size);

        ofs.put('\n');
    }
}

void ir::write_doc_url_file(const ir::doc_url_index& doc_url_index, const std::string& filename, ir::codec_base& codec) {
    std::ofstream ofs;
    ofs.open(filename, std::ios::binary);

    for (const auto& [doc_id, url] : doc_url_index) {
        ofs.put(url.size());
        ofs.write(reinterpret_cast<const char*>(url.data()), url.size());

        char doc_id_c[ir::N_BYTES_INT_LENGTH];
        ir::serialize_int(doc_id, doc_id_c, ir::N_BYTES_INT_LENGTH);
        ofs.write(doc_id_c, ir::N_BYTES_INT_LENGTH);
        
        ofs.put('\n');
    }
}

void ir::read_dict_file(ir::u8term_id_map& dict, const std::string& filename, ir::codec_base& codec) {
    std::ifstream ifs;
    ifs.open(filename, std::ios::binary);

    while (true) {
        auto c = ifs.get();
        if (ifs.eof()) break;
        auto word_size = static_cast<uint8_t>(c);
        auto word_data = new char8_t[word_size];
        ifs.read(reinterpret_cast<char*>(word_data), word_size);
        auto word_id_size = static_cast<uint8_t>(ifs.get());
        auto word_id_data = new uint8_t[word_id_size];
        ifs.read(reinterpret_cast<char*>(word_id_data), word_id_size);

        std::vector<uint8_t> word_id_encoded{ word_id_data , word_id_data + word_id_size };
        std::vector<size_t> word_id_decoded;
        codec.decode(word_id_encoded, word_id_decoded);
        auto word_id = word_id_decoded[0];
        std::u8string word{ word_data, word_size };
        
        dict.emplace(std::move(word), std::move(word_id));
        // skip '\n'
        ifs.get();
    }
}

void ir::read_wdict_file(ir::term_id_map& dict, const std::string& filename) {
    std::locale::global(std::locale(""));
    std::wifstream ifs;
    try {
        ifs.open(filename, std::ios::in | std::ios::binary);
        if (!ifs.is_open()) {
            std::cout << "File " << filename << " is not opened: " << std::string(strerror(errno)) << std::endl;
            return;
        }
        
        while (true) {
                auto c = ifs.get();
                if (ifs.eof()) break;
                auto word_size = static_cast<size_t>(c);

                auto word_data = new wchar_t[word_size];
                ifs.read(word_data, word_size);
                std::wstring word{ word_data, word_size };
                auto word_id_size = static_cast<size_t>(ifs.get());
                auto word_id_s = new wchar_t[word_id_size];
                ifs.read(word_id_s, word_id_size);
                auto word_id = std::stoull(std::wstring(word_id_s, word_id_size));

                dict.emplace(std::move(word), std::move(word_id));
                // skip '\n'
                ifs.get();
        }
    }
    catch (std::exception& e) {
		std::cerr << "Fatal error" << e.what() << std::endl;
	}
    catch (...) { }
    
}

void ir::read_index_file(ir::inv_index_opt& inverted_index, const std::string& filename, ir::codec_base& codec) {
    std::ifstream ifs;
    try {
        ifs.open(filename, std::ios::binary);

        if (!ifs.is_open()) {
            std::cout << "File " << filename << " is not opened: " << std::string(strerror(errno)) << std::endl;
            return;
        }

        while (true) {
            auto c = ifs.get();
            if (ifs.eof()) break;
            auto term_id_size = static_cast<uint8_t>(c);
            auto term_id_data = new uint8_t[term_id_size];
            ifs.read(reinterpret_cast<char*>(term_id_data), term_id_size);

            std::vector<uint8_t> term_id_encoded{ term_id_data , term_id_data + term_id_size };
            std::vector<size_t> term_id_decoded;
            codec.decode(term_id_encoded, term_id_decoded);
            auto term_id = term_id_decoded[0];
            
            char posting_list_size_c[ir::N_BYTES_INT_LENGTH];
            ifs.read(posting_list_size_c, ir::N_BYTES_INT_LENGTH);
            auto posting_list_size = ir::get_length(posting_list_size_c, ir::N_BYTES_INT_LENGTH);
            auto posting_list_data = new uint8_t[posting_list_size];
            ifs.read(reinterpret_cast<char*>(posting_list_data), posting_list_size);

            auto posting_list_encoded_ptr = new std::vector<uint8_t>{ posting_list_data , posting_list_data + posting_list_size };

            inverted_index.emplace(std::move(term_id), *posting_list_encoded_ptr);
            // skip '\n'
            ifs.get();
        }
    }
    catch (std::exception& e) {
		std::cerr << "Fatal error" << e.what() << std::endl;
	}
    catch (...) { }
}

void ir::read_doc_url_file(ir::doc_url_index& doc_url_index, const std::string& filename, ir::codec_base& codec) {
    std::ifstream ifs;
    try {
        ifs.open(filename, std::ios::binary);

        if (!ifs.is_open()) {
            std::cout << "File " << filename << " is not opened: " << std::string(strerror(errno)) << std::endl;
            return;
        }
    
        while (true) {
            auto c = ifs.get();
            if (ifs.eof()) break;
            auto doc_url_size = static_cast<uint8_t>(c);
            auto doc_url_data = new char8_t[doc_url_size];
            ifs.read(reinterpret_cast<char*>(doc_url_data), doc_url_size);

            char doc_id_c[ir::N_BYTES_INT_LENGTH];
            ifs.read(doc_id_c, ir::N_BYTES_INT_LENGTH);
            auto doc_id = ir::get_length(doc_id_c, ir::N_BYTES_INT_LENGTH);

            doc_url_index.emplace(std::move(doc_id), std::u8string{ doc_url_data, doc_url_size });
            // skip '\n'
            ifs.get();
        }
    }
    catch (std::exception& e) {
		std::cerr << "Fatal error" << e.what() << std::endl;
	}
    catch (...) { }
}

void ir::serialize_int(size_t src, char* dest, size_t n_bytes) {
    for (auto i = 0; i < n_bytes; ++i) {
        auto rem = src % 256;
        dest[i] = rem;
        src = (src - rem) / 256;
    }
}