#pragma once

#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <set>
#include <map>

namespace ir {

/**
 * Document that have not been processed at all are considered raw document.
 */
using raw_doc = std::u8string;

/**
 * Typedef for an index that holds the id of a document and its raw
 * content.
 */

using doc_term_index = std::map<size_t, std::set<std::wstring>>;

using doc_url_index = std::map<size_t, std::u8string>;

using inv_index_str = std::unordered_map<std::wstring, std::vector<size_t>>;

/**
 * Typedef for a data structure mapping a term to a unique ID.
 */
using term_id_map = std::unordered_map<std::wstring, size_t>;

using u8term_id_map = std::unordered_map<std::u8string, size_t>;

using inv_index_id = std::unordered_map<size_t, std::vector<size_t>>;

using inv_index_opt = std::unordered_map<size_t, std::vector<uint8_t>>;
} // namespace ir
