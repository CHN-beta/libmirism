# pragma once
# include <mirism/detail_/misc/concepts.hpp>

namespace mirism
{
	template <typename T1, typename T2> consteval inline bool detail_::specialization_of_detail_::params_is_ok()
	{
		if constexpr (std::tuple_size_v<T1> == 0)
			return true;
		else if constexpr (std::tuple_size_v<T2> == 0)
			return false;
		else if constexpr (std::same_as<std::tuple_element_t<0, T1>, std::tuple_element_t<0, T2>>)
			return params_is_ok<typename DropFirstMemberOfTuple<T1>::type, typename DropFirstMemberOfTuple<T2>::type>();
		else
			return false;
	}
	template <typename T, template <typename...> typename Template> template <typename... ProvidedArgs> consteval inline
		bool detail_::specialization_of_detail_::Helper<T, Template>::check_provided_args()
		{return false;}
	template <template <typename...> typename Template, typename... Args> template <typename... ProvidedArgs>
		consteval inline
		bool detail_::specialization_of_detail_::Helper<Template<Args...>, Template>::check_provided_args()
		{return params_is_ok<std::tuple<ProvidedArgs...>, std::tuple<Args...>>();}
}