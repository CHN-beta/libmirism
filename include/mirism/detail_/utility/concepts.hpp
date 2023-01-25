# pragma once
# include <concepts>
# include <type_traits>

namespace mirism
{
	template <typename Class> concept ConceptDecayed = std::same_as<std::decay_t<Class>, Class>;

	namespace detail_::specialization_of_detail_
	{
		template <typename Class> struct ClassTemplateDropFirstMemberOfTupleHelper;
		template <typename ClassFirst, typename... ClassOthers> struct
			ClassTemplateDropFirstMemberOfTupleHelper<std::tuple<ClassFirst, ClassOthers...>>
			{using Class = std::tuple<ClassOthers...>;};
		template <typename ClassProvidedArgs, typename ClassActualArgs> consteval bool check_provided_args();
		template <typename Class, template <typename...> typename ClassTemplate>
			struct ClassTemplateSpecializationOfHelper
			{template <typename... ClassProvidedArgs> consteval static bool check_provided_args();};
		template <template <typename...> typename ClassTemplate, typename... ClassActualArgs>
			struct ClassTemplateSpecializationOfHelper<ClassTemplate<ClassActualArgs...>, ClassTemplate>
			{template <typename... ClassProvidedArgs> consteval static bool check_provided_args();};
	}
	template <typename Class, template <typename...> typename ClassTemplate, typename... ClassProvidedArgs>
		concept ConceptSpecializationOf
		= detail_::specialization_of_detail_::ClassTemplateSpecializationOfHelper<std::decay_t<Class>, ClassTemplate>
			::template check_provided_args<ClassProvidedArgs...>();

	template <typename Class> concept ConceptCompleted = sizeof(Class) == sizeof(Class);

	template <typename ClassFrom, typename ClassTo> concept ConceptImplicitlyConvertibleTo
		= std::is_convertible<ClassFrom, ClassTo>::value;
	template <typename ClassTo, typename ClassFrom> concept ConceptImplicitlyConvertibleFrom
		= std::is_convertible<ClassFrom, ClassTo>::value;
	template <typename ClassFrom, typename ClassTo> concept ConceptExplicitlyConvertibleTo
		= std::is_constructible<ClassTo, ClassFrom>::value;
	template <typename ClassTo, typename ClassFrom> concept ConceptExplicitlyConvertibleFrom
		= std::is_constructible<ClassTo, ClassFrom>::value;
	template <typename ClassFrom, typename ClassTo> concept ConceptConvertibleTo
		= ConceptImplicitlyConvertibleTo<ClassFrom, ClassTo> || ConceptExplicitlyConvertibleTo<ClassFrom, ClassTo>;
	template <typename ClassFrom, typename ClassTo> concept ConceptConvertibleFrom
		= ConceptConvertibleTo<ClassFrom, ClassTo>;

	template <typename ClassFunction, auto... Args> concept ConceptConstevalInvokable
		= requires() {typename std::type_identity_t<int[(ClassFunction()(Args...), 1)]>;};

	template <typename Class> concept ConceptEnumerable = std::is_enum_v<Class>;
}
