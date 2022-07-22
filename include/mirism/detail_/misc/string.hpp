# pragma once
# include <mirism/detail_/misc/concepts.hpp>

namespace mirism
{
	// Store a string in a static member of a class; or, use a class to represent a string.
	template <decayed_type Char, Char... c> struct BasicStaticString
	{
		static constexpr std::array<Char, sizeof...(c)> Array{c...};
		static constexpr std::basic_string_view<Char> StringView{Array.data(), sizeof...(c)};
	};
	template <char... c> using StaticString = BasicStaticString<char, c...>;
	inline namespace stream_operators
	{
		template <typename Char, Char... c>
			std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>& os, BasicStaticString<Char, c...>);
	}
	inline namespace literals
		{template <typename Char, Char... c> consteval BasicStaticString<Char, c...> operator""_ss();}

	namespace detail_
	{
		template <typename C, typename T> struct SpecializationOfBasicStaticStringHelper : std::false_type {};
		template <typename C, C... c>
			struct SpecializationOfBasicStaticStringHelper<C, BasicStaticString<C, c...>> : std::true_type {};
		template <typename C, C... c>
			struct SpecializationOfBasicStaticStringHelper<void, BasicStaticString<C, c...>> : std::true_type {};
		template <typename T, typename C> concept specialization_of_basic_static_string
			= SpecializationOfBasicStaticStringHelper<std::decay_t<C>, std::decay_t<T>>::value;
	}
	template <typename T, typename C = void> concept specialization_of_basic_static_string
		= detail_::specialization_of_basic_static_string<T, C>
			&& detail_::specialization_of_basic_static_string<T, void>;
	template <typename T> concept specialization_of_static_string = specialization_of_basic_static_string<T, char>;

	// Store a string in a fixed-size array
	template <decayed_type Char, std::size_t N> requires std::same_as<Char, std::decay_t<Char>> struct BasicFixedString
	{
		Char data[N];
		constexpr BasicFixedString(const Char (&str)[N]);
		constexpr std::basic_string_view<Char> string_view() const&;
		constexpr auto string_view() const&& = delete;
		constexpr std::size_t size() const;
	};
	template <char... c> using FixedString = BasicFixedString<char, sizeof...(c)>;
	inline namespace stream_operators
	{
		template <typename Char, std::size_t N> std::basic_ostream<Char>& operator<<
			(std::basic_ostream<Char>& os, const BasicFixedString<Char, N>& str);
	}
	inline namespace literals
		{template <BasicFixedString FS> constexpr decltype(FS) operator""_fs();}

	namespace detail_
	{
		template <typename C, typename T> struct SpecializationOfBasicFixedStringHelper : std::false_type {};
		template <typename C, std::size_t N>
			struct SpecializationOfBasicFixedStringHelper<C, BasicFixedString<C, N>> : std::true_type {};
		template <typename C, std::size_t N>
			struct SpecializationOfBasicFixedStringHelper<void, BasicFixedString<C, N>> : std::true_type {};
		template <typename T, typename C> concept specialization_of_basic_fixed_string
			= SpecializationOfBasicFixedStringHelper<std::decay_t<C>, std::decay_t<T>>::value;
	}
	template <typename T, typename C = void> concept specialization_of_basic_fixed_string
		= detail_::specialization_of_basic_fixed_string<T, C>
			&& detail_::specialization_of_basic_fixed_string<T, void>;
	template <typename T> concept specialization_of_fixed_string = specialization_of_basic_fixed_string<T, char>; 
}