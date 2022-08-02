# pragma once
# include <mirism/detail_/handler/base.hpp>
# include <mirism/detail_/handler/domain_mirror/base.hpp>

namespace mirism::handler
{
	template <auto Domain, typename Handlers, typename Contents> class DomainMirror;
	template <FixedString Domain, typename... Handlers, typename... Contents>
		class DomainMirror<Domain, std::tuple<Handlers...>, std::tuple<Contents...>>
		: public Base, public Logger::ObjectMonitor<DomainMirror>
	{
		
	};
}