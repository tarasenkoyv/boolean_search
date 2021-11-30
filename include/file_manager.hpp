#pragma once

#include "defs.hpp"
#include "codec.hpp"

#include <string>
#include <vector>

namespace ir {
const std::string DATASET_DIR = "../../dataset";

const std::string DICT_PATH = "dict.txt";

/**
 * Relative path from executable to the positional inverted index built
 * by indexer and used by searcher.
 */
const std::string INDEX_PATH = "index.txt";

const std::string DOC_URL_PATH = "doc_url.txt";

std::vector<std::string> get_data_file_list();

void write_dict_file(const u8term_id_map& dict, const std::string& filename, codec_base& codec);

void write_wdict_file(const term_id_map& dict, const std::string& filename);

void write_index_file(const inv_index_opt& inverted_index, const std::string& filename, codec_base& codec);

void write_doc_url_file(const doc_url_index& doc_url_index, const std::string& filename, codec_base& codec);

void read_dict_file(u8term_id_map& dict, const std::string& filename, codec_base& codec);

void read_wdict_file(term_id_map& dict, const std::string& filename);

void read_index_file(inv_index_opt& inverted_index, const std::string& filename, codec_base& codec);

void read_doc_url_file(doc_url_index& doc_url_index, const std::string& filename, codec_base& codec);

void serialize_int(size_t src, char* dest, size_t n_bytes);
}