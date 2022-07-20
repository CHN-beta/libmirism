# pragma once
# include <mirism/detail_/misc/staticize.hpp>

namespace mirism
{
	template <typename T, typename Ret, typename... Params> template <auto F> inline
		auto detail_::StaticizeHelper<Ret(T::*)(Params...) const>::staticize() -> static_function_type
	{
		return [](Params... ps) -> Ret
		{
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wpmf-conversions"
			return static_function_type(F)(reinterpret_cast<const T*>(NULL), std::forward<Params>(ps)...);
# pragma GCC diagnostic pop
		};
	}
}