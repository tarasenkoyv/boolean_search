#include <query_tree.hpp>


#include <map>
#include <regex>
#include <set>
#include <queue>

#include <codec.hpp>

namespace ir {

void query_tree::build_tree() {
	//static const srell::u8regex re{ u8R"([\p{L}\p{N}]+|[\(\)&\|!])" };
	//srell::u8sregex_token_iterator it{ query_.cbegin(), query_.cend(), re, 0 };
	//auto tokens = std::vector<std::u8string>{ it, {} };
	static const std::wregex re{ LR"(\w+|[\(\)&\|!])" };
	std::wsregex_token_iterator it{ query_.cbegin(), query_.cend(), re, 0 };
	auto tokens = std::vector<std::wstring>{ it, {} };
	auto root_node = build(tokens, 0, tokens.size() - 1);
	root_.reset(root_node);
}

int query_tree::get_priority(std::wstring& token) {
	if (priorities_.contains(token)) return priorities_[token];
	return 100;
}

query_tree::tree_node* query_tree::build(std::vector<std::wstring>& tokens, size_t first, size_t last) {
	auto current_tree = new query_tree::tree_node{ L"" };
	if (last == first) {
		current_tree->key_ = tokens[first];
		return current_tree;
	}

	// minimum priority
	size_t min_prt{ 100 }, min_prt_pos{ 0 };
	// counter open brackets
	size_t nest{ 0 };
	for (size_t i = first; i <= last; ++i) {
		if (tokens[i] == L"(") {
			nest++;
			continue;
		}
		if (tokens[i] == L")") {
			nest--;
			continue;
		}
		if (nest > 0) continue;
		auto prt = get_priority(tokens[i]);
		if (prt <= min_prt) {
			min_prt = prt;
			min_prt_pos = i;
		}
	}
	// все выражение взято в скобки
	if (min_prt == 100 && tokens[first] == L"(" && tokens[last] == L")") {
		delete current_tree;
		return build(tokens, first + 1, last - 1);
	}
	current_tree->key_ = tokens[min_prt_pos];
	if (tokens[min_prt_pos] != L"!") {
		current_tree->left_.reset(build(tokens, first, min_prt_pos - 1));
	}
	current_tree->right_.reset(build(tokens, min_prt_pos + 1, last));
	
	return current_tree;
}

void query_tree::build_posting_lists(inv_index_opt& index, term_id_map& dict) {
	if (root_ == nullptr) return;
	static varbyte_codec codec;
	std::queue<tree_node*> vertex;
	vertex.push(root_.get());
	while (!vertex.empty()) {
		auto current = vertex.front();
		vertex.pop();
		
		auto& value = current->key_;
		if (value != L"!" && value != L"|" && value != L"&") {
			if (dict.contains(value)) {
				auto& term_id = dict[value];
				std::vector<size_t> posting_list;
				codec.decode(index[term_id], posting_list);
				for (size_t i = 1; i < posting_list.size(); ++i) {
					posting_list[i] += posting_list[i - 1];
				}
				posting_lists_.emplace(value, std::move(posting_list));
				posting_list_offsets_.emplace(value, 0);
			}
			else {
				posting_lists_.emplace(value, std::vector<size_t>{});
				posting_list_offsets_.emplace(value, 0);
			}
		}

		if (current->left_ != nullptr) {
			vertex.push(current->left_.get());
		}
		if(current->right_ != nullptr) {
			vertex.push(current->right_.get());
		}
	}
}

void query_tree::get_urls(doc_url_index& doc_url_index, std::vector<std::u8string>& urls) {
	if (root_ == nullptr) return;
	current_doc_id_ = 1;
	max_doc_id_ = doc_url_index.size() + 1;

	while (current_doc_id_ <= max_doc_id_) {
		try {
			set_posting_list_offset(current_doc_id_);
			auto new_doc_id = compute_doc_id(root_.get());
			if (current_doc_id_ == new_doc_id) {
				urls.push_back(doc_url_index[current_doc_id_]);
				current_doc_id_ += 1;
			}
			else {
				current_doc_id_ = new_doc_id;
			}
		}
		catch (...) {}
	}
}

void query_tree::set_posting_list_offset(size_t doc_id) {
	for (auto& [key, value] : posting_lists_) {
		size_t offset{ posting_list_offsets_[key] };
		while (offset < value.size() && value[offset] < doc_id) {
			++offset;
		}
		posting_list_offsets_[key] = offset;
	}
}

size_t query_tree::compute_doc_id(query_tree::tree_node* node) {
	if (node->key_ == L"&") {
		auto left = compute_doc_id(node->left_.get());
		auto right = compute_doc_id(node->right_.get());
		return std::max(left, right);
	}
	else if (node->key_ == L"|") {
		auto left = compute_doc_id(node->left_.get());
		auto right = compute_doc_id(node->right_.get());
		return std::min(left, right);
	}
	else if (node->key_ == L"!") {
		auto value = compute_doc_id(node->right_.get());
		if (value != current_doc_id_) {
			return current_doc_id_;
		}
		else {
			return current_doc_id_ + 1;
		}
	}
	auto& token = node->key_;
	auto& posting_list = posting_lists_[token];
	auto offset = posting_list_offsets_[token];
	if (offset < posting_list.size()) {
		return posting_list[offset];
	}
	else {
		return max_doc_id_ + 1;
	}
}
}