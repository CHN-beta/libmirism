# pragma once
# include <mirism/handler/subdomain/base.hpp>

namespace mirism::hander
{
	template <std::derived_from<subdomain::Base<>>... Subdomains> class Subdomain : public Base
	{
		protected: consteval static void check_subdomains_();
		protected: std::map<std::string, std::unique_ptr<subdomain::Base<>>> subdomains_;
		public: Subdomain();
		public: std::optional<Instance::Response>
			operator()(Instance::Request request, std::unique_ptr<client::Base> client) override;
	};
}
