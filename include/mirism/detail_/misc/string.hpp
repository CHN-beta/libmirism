# pragma once
# include <mirism/detail_/misc/concepts.hpp>

namespace mirism
{
	// Store a string in a static member of a class; or, use a class to represent a string.
	template <decayed_type Char, Char... c> struct StaticString
	{
		static constexpr std::array<Char, sizeof...(c)> Array{c...};
		static constexpr std::basic_string_view<Char> StringView{Array.data(), sizeof...(c)};
	};
	inline namespace stream_operators
	{
		template <typename Char, Char... c>
			std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>& os, StaticString<Char, c...>);
	}
	inline namespace literals
		{template <typename Char, Char... c> consteval StaticString<Char, c...> operator""_ss();}

	namespace detail_
	{
		template <typename C, typename T> struct SpecializationOfStaticStringHelper : std::false_type {};
		template <typename C, C... c>
			struct SpecializationOfStaticStringHelper<C, StaticString<C, c...>> : std::true_type {};
		template <typename C, C... c>
			struct SpecializationOfStaticStringHelper<void, StaticString<C, c...>> : std::true_type {};
		template <typename T, typename C> concept specialization_of_static_string
			= SpecializationOfStaticStringHelper<std::decay_t<C>, std::decay_t<T>>::value;
	}
	template <typename T, typename C = void> concept specialization_of_static_string
		= detail_::specialization_of_static_string<T, C> && detail_::specialization_of_static_string<T, void>;

	// Store a string in a fixed-size array
	template <decayed_type Char, std::size_t N> requires std::same_as<Char, std::decay_t<Char>> struct FixedString
	{
		Char data[N];
		constexpr FixedString(const Char (&str)[N]);
		constexpr std::basic_string_view<Char> string_view() const&;
		constexpr auto string_view() const&& = delete;
		constexpr std::size_t size() const;
	};
	inline namespace stream_operators
	{
		template <typename Char, std::size_t N> std::basic_ostream<Char>& operator<<
			(std::basic_ostream<Char>& os, const FixedString<Char, N>& str);
	}
	inline namespace literals
		{template <FixedString FS> constexpr decltype(FS) operator""_fs();}

	namespace detail_
	{
		template <typename C, typename T> struct SpecializationOfFixedStringHelper : std::false_type {};
		template <typename C, std::size_t N>
			struct SpecializationOfFixedStringHelper<C, FixedString<C, N>> : std::true_type {};
		template <typename C, std::size_t N>
			struct SpecializationOfFixedStringHelper<void, FixedString<C, N>> : std::true_type {};
		template <typename T, typename C> concept specialization_of_fixed_string_
			= SpecializationOfFixedStringHelper<std::decay_t<C>, std::decay_t<T>>::value;
	}
	template <typename T, typename C = void> concept specialization_of_fixed_string
		= detail_::specialization_of_fixed_string_<T, C> && detail_::specialization_of_fixed_string_<T, void>;
}