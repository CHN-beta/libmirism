# pragma once
# include <mirism/handler/base.hpp>

namespace mirism::handler::subdomain
{
	template <FixedString Subdomain = ""> class Base;
	template <> class Base<> : public handler::Base
	{
		// This function should be virtual static.
		public: constexpr virtual std::string_view get_subdomain() const = 0;
	};
	template <FixedString Subdomain> class Base : public Base<>
		{public: constexpr std::string_view get_subdomain() const override;};
}
