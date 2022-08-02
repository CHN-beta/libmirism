# pragma once
# include <mirism/detail_/misc/string.hpp>

namespace mirism
{
	template <typename Char, Char... c> inline std::basic_ostream<Char>& stream_operators::operator<<
		(std::basic_ostream<Char>& os, BasicStaticString<Char, c...>)
		{return os << std::basic_string_view{c...};}
	template <typename Char, Char... c> consteval inline BasicStaticString<Char, c...> literals::operator""_ss()
		{return {};}

	template <typename Char, std::size_t N> constexpr inline
		BasicFixedString<Char, N>::BasicFixedString(const Char (&str)[N])
		{std::copy_n(str, N, Data);}
	template <typename Char, std::size_t N> inline std::basic_ostream<Char>& stream_operators::operator<<
		(std::basic_ostream<Char>& os, const BasicFixedString<Char, N>& str)
		{return os << std::basic_string_view<Char>(str.Data, str.Size);}
	template <BasicFixedString FS> constexpr inline decltype(FS) literals::operator""_fs()
		{return FS;}

	template <typename Char, std::size_t N> template <std::size_t M> requires (M<=N) constexpr inline
		BasicVariableString<Char, N>::BasicVariableString(const Char (&str)[M]) : Size(M)
	{
		std::fill(Data, Data + N, '\0');
		std::copy_n(str, M, Data);
	}
	template <typename Char, std::size_t N> inline std::basic_ostream<Char>& stream_operators::operator<<
		(std::basic_ostream<Char>& os, const BasicVariableString<Char, N>& str)
		{return os << std::basic_string_view<Char>(str.Data, str.Size);}
}
