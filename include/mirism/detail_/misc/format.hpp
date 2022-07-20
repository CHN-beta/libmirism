# pragma once
# include <mirism/detail_/misc/string.hpp>

namespace mirism
{
	namespace detail_
	{
		template <typename Char, Char... c> struct FormatLiteralHelper : protected StaticString<Char, c...>
			{template <typename... Param> std::basic_string<Char> operator()(Param&&... param) const;};
	}
	inline namespace literals
		{template <typename Char, Char... c> consteval detail_::FormatLiteralHelper<Char, c...> operator""_f();}

	inline namespace stream_operators {template <Enum T> std::ostream& operator<<(std::ostream& os, T value);}

	namespace detail_
	{
		template <typename T> concept OptionalWrap
			= specialization_of<T, std::optional> || specialization_of<T, std::shared_ptr>
				|| specialization_of<T, std::weak_ptr> || specialization_of<T, std::unique_ptr>
				|| specialization_of<T, std::experimental::observer_ptr>;
		template <typename T> struct non_cv_value_type {};
		template <OptionalWrap T> requires requires() {typename T::value_type;} struct non_cv_value_type<T>
			{using type = std::remove_cvref_t<typename T::value_type>;};
		template <OptionalWrap T> requires requires() {typename T::element_type;} struct non_cv_value_type<T>
			{using type = std::remove_cvref_t<typename T::element_type>;};
		template <typename T> struct FormatterReuseProxy
		{
			constexpr auto parse(fmt::format_parse_context& ctx)
				-> std::invoke_result_t<decltype(&fmt::format_parse_context::begin), fmt::format_parse_context>;
		};
		template <typename T>
			requires (!specialization_of<T, std::weak_ptr> && std::default_initializable<fmt::formatter<T>>)
			struct FormatterReuseProxy<T> : fmt::formatter<T> {};
	}
}

template <mirism::detail_::OptionalWrap Wrap> struct fmt::formatter<Wrap>
	: mirism::detail_::FormatterReuseProxy<typename mirism::detail_::non_cv_value_type<Wrap>::type>
{
	template <typename FormatContext> auto format(const Wrap& wrap, FormatContext& ctx)
		-> std::invoke_result_t<decltype(&FormatContext::out), FormatContext>;
};

