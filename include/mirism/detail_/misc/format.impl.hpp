# pragma once
# include <mirism/detail_/misc/format.hpp>

namespace mirism
{
	template <typename Char, Char... c> template <typename... Param> inline
		std::basic_string<Char> detail_::FormatLiteralHelper<Char, c...>::operator() (Param&&... param) const
		{return fmt::format(BasicStaticString<Char, c...>::StringView, std::forward<Param>(param)...);}
	template <typename Char, Char... c> consteval inline
		detail_::FormatLiteralHelper<Char, c...> literals::operator""_f()
		{return {};}

	template <typename Char, typename... Ts> requires (sizeof...(Ts) > 0) inline
		std::basic_ostream<Char>& stream_operators::operator<<(std::basic_ostream<Char>& os, const std::variant<Ts...>& value)
	{
		auto try_print = [&]<typename T>()
		{
			if (std::holds_alternative<T>(value))
			{
				if constexpr (formattable<T, Char>)
					os << "({}: {})"_f(nameof::nameof_full_type<T>(), std::get<T>(value));
				else
					os << "({}: {})"_f(nameof::nameof_full_type<T>(), "non-null unformattable value");
			}
		};
		(try_print.template operator()<Ts>(), ...);
		return os;
	}

	template <typename T> inline constexpr
		auto detail_::FormatterReuseProxy<T>::parse(fmt::format_parse_context& ctx)
		-> std::invoke_result_t<decltype(&fmt::format_parse_context::begin), fmt::format_parse_context>
	{
		if (ctx.begin() != ctx.end() && *ctx.begin() != '}')
			throw fmt::format_error
			(
				"{} do not support to be format, so the wrapper should not have any format syntax."_f
					(nameof::nameof_full_type<T>())
			);
		return ctx.begin();
	}
}

namespace fmt
{
	template <mirism::detail_::OptionalWrap Wrap> template <typename FormatContext> inline
		auto formatter<Wrap>::format(const Wrap& wrap, FormatContext& ctx)
		-> std::invoke_result_t<decltype(&FormatContext::out), FormatContext>
	{
		using namespace mirism::literals;
		using namespace mirism::stream_operators;
		using value_t = typename mirism::detail_::non_cv_value_type<Wrap>::type;
		using char_type = typename FormatContext::char_type;
		auto format_value_type = [&, this](const value_t& value)
		{
			if constexpr (!mirism::formattable<value_t, char_type>)
				return format_to(ctx.out(), "non-null unformattable value");
			else if constexpr (std::default_initializable<formatter<value_t>>)
				mirism::detail_::FormatterReuseProxy<value_t>::format(value, ctx);
			else
				format_to(ctx.out(), "{}", value);
		};
		format_to(ctx.out(), "(");
		if constexpr (mirism::specialization_of<Wrap, std::optional>)
		{
			if (wrap)
				format_value_type(*wrap);
			else
				format_to(ctx.out(), "null");
		}
		else if constexpr (mirism::specialization_of<Wrap, std::weak_ptr>)
		{
			if (auto shared = wrap.lock())
			{
				format_to(ctx.out(), "{} ", ptr(shared.get()));
				format_value_type(*shared);
			}
			else
				format_to(ctx.out(), "null");
		}
		else
		{
			if (wrap)
			{
				format_to(ctx.out(), "{} ", ptr(wrap.get()));
				format_value_type(*wrap);
			}
			else
				format_to(ctx.out(), "null");
		}
		return format_to(ctx.out(), ")");
	}

	template <mirism::enumerable T> inline constexpr
		auto formatter<T>::parse(format_parse_context& ctx)
		-> std::invoke_result_t<decltype(&format_parse_context::begin), format_parse_context>
	{
		auto it = ctx.begin();
		if (it != ctx.end() && *it == 'f')
		{
			full = true;
			it++;
		}
		if (it != ctx.end() && *it != '}')
			throw format_error("syntax error.");
		return it;
	}

	template <mirism::enumerable T> template <typename FormatContext> inline
		auto formatter<T>::format(const T& value, FormatContext& ctx)
		-> std::invoke_result_t<decltype(&FormatContext::out), FormatContext>
	{
		if (full)
			return format_to(ctx.out(), "{}::{}", nameof::nameof_type<T>(), nameof::nameof_enum(value));
		else
			return format_to(ctx.out(), "{}", nameof::nameof_enum(value));
	}
}
