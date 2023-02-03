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

	template <typename T> struct RemoveMemberPointer {using Type = T;};
	template <typename Class, typename Member> struct RemoveMemberPointer<Member Class::*> {using Type = Member;};
	template <typename MemberPointer> using RemoveMemberPointerType = typename RemoveMemberPointer<MemberPointer>::Type;

	[[noreturn]] void block_forever();

	template <typename From, typename To> struct MoveQualifiers
	{
		protected: static constexpr bool Const_ = std::is_const_v<From>;
		protected: static constexpr bool Volatile_ = std::is_volatile_v<From>;
		protected: static constexpr bool Reference_ = std::is_reference_v<From>;
		protected: static constexpr bool Lvalue_ = std::is_lvalue_reference_v<From>;
		protected: using NoCvrefType_ = std::remove_cvref_t<To>;
		protected: using NoCvType_
			= std::conditional_t<Reference_, std::conditional_t<Lvalue_, NoCvrefType_&, NoCvrefType_&&>, NoCvrefType_>;
		protected: using NoConstType_ = std::conditional_t<Volatile_, volatile NoCvType_, NoCvType_>;
		using Type = std::conditional_t<Const_, const NoConstType_, NoConstType_>;
	};
	template <typename From, typename To> using MoveQualifiersType = typename MoveQualifiers<From, To>::Type;

	template <typename T, typename Fallback = void> struct FallbackIfNoTypeDeclared {using Type = Fallback;};
	template <typename T, typename Fallback> requires requires {typename T::Type;}
		struct FallbackIfNoTypeDeclared<T, Fallback> {using Type = typename T::Type;};
	template <typename T, typename Fallback> requires requires {typename T::type;}
		struct FallbackIfNoTypeDeclared<T, Fallback> {using Type = typename T::type;};
	template <typename T> using FallbackIfNoTypeDeclaredType = typename FallbackIfNoTypeDeclared<T>::Type;
}
