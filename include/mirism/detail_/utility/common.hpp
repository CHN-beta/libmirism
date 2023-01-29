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

	struct CaseInsensitiveStringLessComparator
	{
		template <typename String> constexpr bool operator()(const String& s1, const String& s2) const;
	};

	namespace detail_
	{
		template <typename T> struct RemoveMemberPointerHelper {using Type = T;};
		template <typename Class, typename Member> struct RemoveMemberPointerHelper<Member Class::*>
			{using Type = Member;};
	}
	template <typename MemberPointer> using RemoveMemberPointer
		= typename detail_::RemoveMemberPointerHelper<MemberPointer>::Type;

	[[noreturn]] void block_forever();
}
