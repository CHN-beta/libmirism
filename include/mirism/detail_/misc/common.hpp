# pragma once

# include <mutex>
# include <shared_mutex>
# include <condition_variable>
# include <functional>
# include <optional>
# include <queue>
# include <variant>
# include <memory>
# include <concepts>
# include <type_traits>
# include <chrono>
# include <string>
# include <string_view>
# include <algorithm>
# include <regex>
# include <experimental/memory>

# include <fmt/format.h>
# include <fmt/ranges.h>
# include <fmt/ostream.h>
# include <fmt/chrono.h>

# include <nameof.hpp>

# include <boost/container_hash/hash.hpp>

# define BOOST_STACKTRACE_USE_BACKTRACE
# include <boost/stacktrace.hpp>

# include <tgbot/tgbot.h>

# include <boost/core/demangle.hpp>

# include <magic_enum.hpp>

# include <boost/iostreams/filtering_streambuf.hpp>
# include <boost/iostreams/copy.hpp>
# include <boost/iostreams/filter/gzip.hpp>
# include <boost/iostreams/filter/zlib.hpp>

# include <brotli/encode.h>
# include <brotli/decode.h>

# include <cppcoro/generator.hpp>

namespace mirism
{
	template<typename... Ts> std::size_t hash(Ts&&... objs);
	void unused(auto&&...);

	using uint128_t = __uint128_t;

	inline namespace literals
	{
		using namespace std::literals;
		using namespace fmt::literals;
		std::regex operator""_re(const char* str, std::size_t len);
	}

	inline namespace stream_operators {using namespace magic_enum::iostream_operators;}

	struct CaseInsensitiveStringLess
		{template <typename T> constexpr bool operator()(const T& lhs, const T& rhs) const;};

	template <typename T> struct remove_member_pointer {using type = T;};
	template <typename C, typename T> struct remove_member_pointer<T C::*> {using type = T;};
	template <typename T> using remove_member_pointer_t = typename remove_member_pointer<T>::type;
}
