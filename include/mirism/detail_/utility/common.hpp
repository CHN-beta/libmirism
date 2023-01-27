# pragma once
# include <regex>
# include <fmt/format.h>
# include <magic_enum.hpp>

namespace mirism
{
	std::size_t hash(auto&&... objs);
	[[gnu::always_inline]] void unused(auto&&...);

	using uint128_t = __uint128_t;

	inline namespace literals
	{
		using namespace std::literals;
		using namespace fmt::literals;
		std::regex operator""_re(const char* str, std::size_t len);
	}

	inline namespace stream_operators {using namespace magic_enum::iostream_operators;}

	struct CaseInsensitiveStringLess_t
	{
		template <typename String_t> [[gnu::visibility("hidden")]] constexpr
			bool operator()(const String_t& lhs, const String_t& rhs) const;
	};

	namespace detail_
	{
		template <typename Self_t> struct RemoveMemberPointerHelper_T {using type = Self_t;};
		template <typename Class_t, typename Member_t> struct RemoveMemberPointerHelper_T<Member_t Class_t::*>
			{using type = Member_t;};
	}
	template <typename MemberPointer_t> using RemoveMemberPointer_T
		= typename detail_::RemoveMemberPointerHelper_T<MemberPointer_t>::type;

	[[noreturn]] void block_forever();
}
