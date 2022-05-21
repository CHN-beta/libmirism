# pragma once

# include <mutex>
# include <condition_variable>
# include <functional>
# include <optional>
# include <queue>
# include <variant>
# include <memory>
# include <concepts>
# include <type_traits>
# include <chrono>
# include <string>
# include <string_view>
# include <algorithm>
# include <regex>
# include <experimental/memory>

# include <fmt/format.h>
# include <fmt/ranges.h>
# include <fmt/ostream.h>
# include <fmt/chrono.h>

# include <nameof.hpp>

# include <boost/container_hash/hash.hpp>

# define BOOST_STACKTRACE_USE_BACKTRACE
# include <boost/stacktrace.hpp>

# include <tgbot/tgbot.h>

# include <boost/core/demangle.hpp>

namespace mirism
{
	template<typename... Ts> std::size_t hash(Ts&&... objs);
	void unused(auto&&...);

	inline namespace literals
	{
		using namespace std::literals;
		using namespace fmt::literals;
		std::regex operator""_re(const char* str, std::size_t len);
	}

	// Store a string in a static member of a class; or, use a class to represent a string.
	template <typename Char, Char... c> struct StaticString
	{
		static constexpr std::array<Char, sizeof...(c)> Array{c...};
		static constexpr std::basic_string_view<Char> StringView{Array.data(), sizeof...(c)};
	};
	inline namespace literals
	{
		template <typename Char, Char... c> consteval StaticString<Char, c...> operator""_ss();
	}

	// Store a string in a fixed-size array
	template <typename Char, std::size_t N> struct FixedString
	{
		Char data[N];
		constexpr FixedString(const Char str[N]);
		constexpr std::basic_string_view<Char> str() const&;
		constexpr auto str() const&& = delete;
		constexpr std::size_t size() const;
	};
	inline namespace literals
	{
		template <FixedString FS> constexpr decltype(FS) operator""_fs();
	}

	// auto str = "{} {}!"_f("Hello", "World"); // str == "Hello World!"
	template <typename Char, Char... c> struct FormatLiteralHelper : protected StaticString<Char, c...>
	{
		template <typename... Param> std::basic_string<Char> operator()(Param&&... param) const;
	};
	inline namespace literals
	{
		template <typename Char, Char... c> consteval FormatLiteralHelper<Char, c...> operator""_f();
	}

	template <typename T> concept not_cvref_type = std::same_as<T, std::remove_cvref_t<T>>;

	// template <specialization_of<std::optional> OptionalWrap> class myclass;
	// template <specialization_of<std::tuple, int> TupleStartWithInt> class myclass;
	// only typename template parameter is considered
	namespace detail_::specialization_of_detail_
	{
		template <typename T> struct DropFirstMemberOfTuple;
		template <typename T, typename... Ts> struct DropFirstMemberOfTuple<std::tuple<T, Ts...>>
			{using type = std::tuple<Ts...>;};
		template <typename T1, typename T2> consteval bool params_is_ok();
		template <typename T, template <typename...> typename Template> struct Helper
			{template <typename... ProvidedArgs> consteval static bool check_provided_args();};
		template <template <typename...> typename Template, typename... Args> struct Helper<Template<Args...>, Template>
			{template <typename... ProvidedArgs> consteval static bool check_provided_args();};
	}
	template <typename T, template <typename...> typename Template, typename... ProvidedArgs>
		concept specialization_of
		= detail_::specialization_of_detail_::Helper<T, Template>::template check_provided_args<ProvidedArgs...>();

	// class myclass;
	// complete_type<myclass>; // false
	// class myclass {};
	// complete_type<myclass>; // true
	namespace detail_
	{
		template <typename, typename = void> struct CompleteTypeHelper : std::false_type {};
		template <typename T> struct CompleteTypeHelper<T, std::void_t<decltype(sizeof(T))>> : std::true_type {};
	}
	template <typename T> concept complete_type = detail_::CompleteTypeHelper<T>::value;

	template <typename From, typename To> concept implicitly_convertible_to = std::is_convertible<From, To>::value;
	template <typename To, typename From> concept implicitly_convertible_from = std::is_convertible<From, To>::value;
	template <typename From, typename To> concept explicitly_convertible_to = std::is_constructible<To, From>::value;
	template <typename To, typename From> concept explicitly_convertible_from = std::is_constructible<To, From>::value;
	template <typename From, typename To> concept convertible_to
		= implicitly_convertible_to<From, To> || explicitly_convertible_to<From, To>;
	template <typename To, typename From> concept convertible_from = convertible_to<From, To>;

	// check if an consteval function could be called by the provided arguments
	template <typename Function, auto... param> concept consteval_invokable
		= requires() {typename std::type_identity_t<int[(Function()(param...), 1)]>;};
	
	// template <specialization_of_static_string<char8_t> str> class myclass; // only accept StaticString with char8_t
	namespace detail_
	{
		template <typename C, typename T> struct SpecializationOfStaticStringHelper : std::false_type {};
		template <typename C, C... c, typename T>
			struct SpecializationOfStaticStringHelper<C, StaticString<T, c...>> : std::true_type
			{};
	}
	template <typename T, typename C> concept specialization_of_static_string
		= detail_::SpecializationOfStaticStringHelper<C, T>::value;

	// Due to the lack of virtual static member function, a member function either cannot be obtained at runtime by
	// checking vtable or cannot be legally used before constructing an object
	// Here we use an gcc extension to convert a virtual member function (that never dereference `this`) to a function
	// pointer which no need to pass `this` as an argument.
	// Then we can use the function pointer to call the member function without constructing an object.
	namespace detail_
	{
		template <typename T> struct StaticizeHelper;
		template <typename T, typename Ret, typename... Params> struct StaticizeHelper<Ret(T::*)(Params...) const>
		{
			using function_type = Ret(*)(const T*, Params...);
			using static_function_type = Ret(*)(Params...);
			template<auto F> static static_function_type staticize();
		};
	}
	template <auto F> inline auto staticize = detail_::StaticizeHelper<decltype(F)>::template staticize<F>;
}

// make smart pointer and std::optional formatable
namespace mirism::detail_
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
template <mirism::detail_::OptionalWrap Wrap> struct fmt::formatter<Wrap>
	: mirism::detail_::FormatterReuseProxy<typename mirism::detail_::non_cv_value_type<Wrap>::type>
{
	template <typename FormatContext> auto format(const Wrap& wrap, FormatContext& ctx)
		-> std::invoke_result_t<decltype(&FormatContext::out), FormatContext>;
};
