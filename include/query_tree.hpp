#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <srell/srell.hpp>
#include <defs.hpp>

namespace ir {

class query_tree {
public:
	
	query_tree(const std::wstring query) : query_(query), current_doc_id_(0), max_doc_id_(0) { 
		priorities_[L"!"] = 3;
		priorities_[L"&"] = 2;
		priorities_[L"|"] = 1;
	}
	~query_tree() = default;
	void get_urls(doc_url_index& doc_url_index, std::vector<std::u8string>& urls);
	
	void build_tree();
	void build_posting_lists(inv_index_opt& index, term_id_map& dict);

	const std::wstring query_;
private:
	using tree_node = struct tree_node {
		tree_node(std::wstring key) : key_(key) {}
		std::wstring key_{L""};
		std::unique_ptr<tree_node> left_;
		std::unique_ptr<tree_node> right_;
	};
	std::unique_ptr<tree_node> root_;
	size_t current_doc_id_;
	size_t max_doc_id_;
	std::unordered_map<std::wstring, std::vector<size_t>> posting_lists_;
	std::unordered_map<std::wstring, size_t> posting_list_offsets_;
	std::unordered_map<std::wstring, size_t> priorities_;

	tree_node* build(std::vector<std::wstring>& tokens, size_t first, size_t last);
	int get_priority(std::wstring& token);
	void set_posting_list_offset(size_t doc_id);
	size_t compute_doc_id(tree_node* node);
};

}