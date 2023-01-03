# pragma once
# include <mirism/detail_/framework/handler/base.hpp>

namespace mirism::handler
{
	class Domain : public Base
	{
		struct RouteStrategy
		{
			std::unordered_map<std::string, std::string> CodeToWord, WordToCode;
			std::unordered_map<std::string, std::shared_ptr<site::Base<>>> DomainToSite;
			static RouteStrategy build
				(std::map<std::shared_ptr<site::Base<>>, std::vector<std::string>> site_to_domain);
		};

		
	}
}
