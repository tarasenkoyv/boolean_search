#pragma once

#include <string>
#include <unordered_map>

#include "defs.hpp"

namespace ir {

/*
* b'\x1a'
*/ 
const char SEP_SYMBOL_URL = '\x1a';
const short N_BYTES_INT_LENGTH = 4;
const short N_SYSTEM_BYTES = 3;

void process_dataset(const std::vector<std::string>& file_list, doc_url_index& doc_url_index, doc_term_index& doc_term_index);

size_t parse_file(std::istream& ifs, doc_url_index& doc_url_index, doc_term_index& doc_term_index,
                  size_t doc_id_start);

size_t get_length(char* length_c, size_t n_bytes);
} // namespace ir