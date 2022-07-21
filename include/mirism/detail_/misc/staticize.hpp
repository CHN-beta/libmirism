# pragma once
# include <mirism/detail_/misc/common.hpp>

namespace mirism
{
	namespace detail_
	{
		template <typename T> struct StaticizeHelper {};
		template <typename T, typename Ret, typename... Params> struct StaticizeHelper<Ret(T::*)(Params...) const>
		{
			using function_type = Ret(*)(const T*, Params...);
			using static_function_type = Ret(*)(Params...);
			template <auto F> static static_function_type staticize();
		};
	}
	template <auto F> inline auto staticize() -> typename detail_::StaticizeHelper<decltype(F)>::static_function_type;
}