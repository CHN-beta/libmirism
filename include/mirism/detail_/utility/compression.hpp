// compression.hpp
//	std::optional<std::string> {de,}compress<CompressMethod>(const std::string& data);

# pragma once
# include <optional>
# include <string>

namespace mirism::string
{
	enum class CompressMethod_t {Gzip, Deflated, Brotli};
	template <CompressMethod_t Method> std::optional<std::string> compress(const std::string& data);
	template <CompressMethod_t Method> std::optional<std::string> decompress(const std::string& data);
}
