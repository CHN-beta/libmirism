# pragma once
# include <mirism/detail_/utility/concepts.hpp>

namespace mirism
{
	template <typename ClassProvidedArgs, typename ClassActualArgs> consteval bool
		detail_::specialization_of_detail_::check_provided_args()
	{
		if constexpr (std::tuple_size_v<ClassProvidedArgs> == 0)
			return true;
		else if constexpr (std::tuple_size_v<ClassActualArgs> == 0)
			return false;
		else if constexpr
			(std::same_as<std::tuple_element_t<0, ClassProvidedArgs>, std::tuple_element_t<0, ClassActualArgs>>)
			return check_provided_args
			<
				typename ClassTemplateDropFirstMemberOfTupleHelper<ClassProvidedArgs>::Class,
				typename ClassTemplateDropFirstMemberOfTupleHelper<ClassActualArgs>::Class
			>();
		else
			return false;
	}
	template <typename Class, template <typename...> typename ClassTemplate> template <typename... ClassProvidedArgs>
		consteval bool detail_::specialization_of_detail_::ClassTemplateSpecializationOfHelper
			<Class, ClassTemplate>::check_provided_args()
		{return false;}
	template <template <typename...> typename ClassTemplate, typename... ClassActualArgs>
		template <typename... ClassProvidedArgs> consteval
		bool detail_::specialization_of_detail_::ClassTemplateSpecializationOfHelper
			<ClassTemplate<ClassActualArgs...>, ClassTemplate>::check_provided_args()
		{
			return specialization_of_detail_::check_provided_args
				<std::tuple<ClassProvidedArgs...>, std::tuple<ClassActualArgs...>>();
		}
}
