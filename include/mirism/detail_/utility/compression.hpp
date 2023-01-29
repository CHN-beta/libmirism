// compression.hpp
//	std::optional<std::string> {de,}compress<CompressMethod>(const std::string& data);

# pragma once
# include <optional>
# include <string>

namespace mirism::string
{
	enum class CompressMethod {Gzip, Deflated, Brotli};
	template <CompressMethod Method> std::optional<std::string> compress(const std::string& data);
	template <CompressMethod Method> std::optional<std::string> decompress(const std::string& data);
}
