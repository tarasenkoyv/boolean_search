#pragma once

#include <cmath>
#include <vector>

namespace ir {

class codec_base {
public:
	virtual void encode(std::vector<size_t>& src, std::vector<uint8_t>& dest) = 0;
	virtual void encode(size_t value, std::vector<uint8_t>& dest) = 0;
	virtual void decode(const std::vector<uint8_t>& src, std::vector<size_t>& dest) = 0;
};

class varbyte_codec : public codec_base {
public:
	virtual void encode(std::vector<size_t>& src, std::vector<uint8_t>& dest) override {
		for (auto& val : src) {
			encode(val, dest);
		}
	}

	virtual void encode(size_t value, std::vector<uint8_t>& dest) override {
		while (value >= 128) {
			uint8_t rem = value % 128;
			value = (value - rem) / 128;
			dest.push_back(rem + 128);
		}
		dest.push_back(value);
	}

	virtual void decode(const std::vector<uint8_t>& src, std::vector<size_t>& dest) override {
		size_t new_val{ 0 };
		size_t p{ 0 };
		for (auto val : src) {
			if (val >> 7) {
				new_val += (val - 128) * std::pow(128, p);
				++p;
			}
			else {
				new_val += val * std::pow(128, p);
				dest.push_back(new_val);
				new_val = 0;
				p = 0;
			}
		}
	}
};

class fibonacci_codec : public codec_base {
public:
	virtual void encode(std::vector<size_t>& src, std::vector<uint8_t>& dest) override {

	}

	virtual void encode(size_t value, std::vector<uint8_t>& dest) override {

	}

	virtual void decode(const std::vector<uint8_t>& src, std::vector<size_t>& dest) override {

	}
};
}