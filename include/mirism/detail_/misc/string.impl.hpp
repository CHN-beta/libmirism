# pragma once
# include <mirism/detail_/misc/string.hpp>

namespace mirism
{
	template <typename Char, Char... c> inline std::basic_ostream<Char>& stream_operators::operator<<
		(std::basic_ostream<Char>& os, StaticString<Char, c...>)
		{return os << std::basic_string_view{c...};}
	template <typename Char, Char... c> consteval inline StaticString<Char, c...> literals::operator""_ss()
		{return {};}

	template <typename Char, std::size_t N> constexpr inline FixedString<Char, N>::FixedString(const Char (&str)[N])
		{std::copy_n(str, N, data);}
	template <typename Char, std::size_t N> constexpr inline
		std::basic_string_view<Char> FixedString<Char, N>::string_view() const&
	{
		if constexpr (N == 1)
			return {};
		else
			return {data, N - 1};
	}
	template <typename Char, std::size_t N> constexpr inline std::size_t FixedString<Char, N>::size() const
		{return N - 1;}
	template <typename Char, std::size_t N> inline std::basic_ostream<Char>& stream_operators::operator<<
		(std::basic_ostream<Char>& os, const FixedString<Char, N>& str)
		{return os << str.string_view();}
	template <FixedString FS> constexpr inline decltype(FS) literals::operator""_fs()
		{return FS;}
}