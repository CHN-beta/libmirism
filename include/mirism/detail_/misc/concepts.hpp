# pragma once
# include <concepts>
# include <type_traits>

namespace mirism
{
	template <typename T> concept decayed_type = std::same_as<std::decay_t<T>, T>;

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
		= detail_::specialization_of_detail_::Helper<std::decay_t<T>, Template>
			::template check_provided_args<ProvidedArgs...>();

	template <typename T> concept complete_type = sizeof(T) == sizeof(T);

	template <typename From, typename To> concept implicitly_convertible_to = std::is_convertible<From, To>::value;
	template <typename To, typename From> concept implicitly_convertible_from = std::is_convertible<From, To>::value;
	template <typename From, typename To> concept explicitly_convertible_to = std::is_constructible<To, From>::value;
	template <typename To, typename From> concept explicitly_convertible_from = std::is_constructible<To, From>::value;
	template <typename From, typename To> concept convertible_to
		= implicitly_convertible_to<From, To> || explicitly_convertible_to<From, To>;
	template <typename To, typename From> concept convertible_from = convertible_to<From, To>;

	template <typename Function, auto... param> concept consteval_invokable
		= requires() {typename std::type_identity_t<int[(Function()(param...), 1)]>;};

	template <typename T> concept enumerable = std::is_enum_v<T>;
}
