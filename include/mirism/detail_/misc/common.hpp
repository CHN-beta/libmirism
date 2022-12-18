# pragma once
# include <regex>
# include <fmt/format.h>
# include <magic_enum.hpp>

namespace mirism
{
	template<typename... Ts> [[gnu::visibility("hidden")]] std::size_t hash(Ts&&... objs);
	[[using gnu: always_inline, visibility("hidden")]] void unused(auto&&...);

	using uint128_t = __uint128_t;

	inline namespace literals
	{
		using namespace std::literals;
		using namespace fmt::literals;
		std::regex operator""_re(const char* str, std::size_t len);
	}

	inline namespace stream_operators {using namespace magic_enum::iostream_operators;}

	struct CaseInsensitiveStringLess
	{
		template <typename T> [[gnu::visibility("hidden")]] constexpr
			bool operator()(const T& lhs, const T& rhs) const;
	};

	template <typename T> struct remove_member_pointer {using type = T;};
	template <typename C, typename T> struct remove_member_pointer<T C::*> {using type = T;};
	template <typename T> using remove_member_pointer_t = typename remove_member_pointer<T>::type;

	[[noreturn, gnu::visibility("hidden")]] void block_forever();
}
