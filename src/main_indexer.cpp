#include <algorithm>
#include <fstream>
#include <iostream>

#include <tokenizer.hpp>
#include <file_manager.hpp>
#include <parser.hpp>
#include <codec.hpp>

void build_inv_index_str(const ir::doc_term_index& doc_term_index, ir::inv_index_str& inv_index) {
	for (auto& [doc_id, terms] : doc_term_index) {
		if (terms.size() == 0) continue;
		for (auto& term : terms) {
			if (!inv_index.contains(term)) {
				inv_index.emplace(term, std::vector<size_t>{});
			}
			inv_index[term].push_back(doc_id);
		}
	}
}

void inv_index_optimization(ir::inv_index_str& src, ir::inv_index_opt& dest, ir::term_id_map& dict, ir::codec_base& codec) {
	size_t term_id{1};
	ir::inv_index_id inv_index_delta;
	for (auto& [term, docs] : src) {
		dict.emplace(term, size_t(term_id));
		std::vector<size_t> posting_list;
		std::vector<size_t>::const_iterator it;
		it = docs.cbegin();
		posting_list.push_back(*it);
		++it;
		while (it != docs.end()) {
			posting_list.push_back(*it - *(it - 1));
			++it;
		}
		inv_index_delta.emplace(size_t(term_id), std::move(posting_list));
		++term_id;
	}

	for (auto& [term_id, docs] : inv_index_delta) {
		std::vector<uint8_t> encoded_posting_list;
		codec.encode(docs, encoded_posting_list);
		dest.emplace(size_t(term_id), std::move(encoded_posting_list));
	}
}

int main() {
	ir::varbyte_codec codec;
	ir::doc_url_index doc_url_index;
	ir::doc_term_index doc_term_index;

	ir::process_dataset(ir::get_data_file_list(), doc_url_index, doc_term_index);

	ir::inv_index_str inv_index_str;
	ir::inv_index_opt inv_index_opt;
	ir::term_id_map dict;
    
    std::cout << "Building inverted index." << std::endl;
	build_inv_index_str(doc_term_index, inv_index_str);

    std::cout << "Start index optimization and build dict." << std::endl;
	inv_index_optimization(inv_index_str, inv_index_opt, dict, codec);

    std::cout << "Wrire dict." << std::endl;
	ir::write_wdict_file(dict, ir::DICT_PATH);
    std::cout << "Wrire index." << std::endl;
	ir::write_index_file(inv_index_opt, ir::INDEX_PATH, codec);
	ir::write_doc_url_file(doc_url_index, ir::DOC_URL_PATH, codec);

	return 0;
}