# pragma once
# include <mirism/client/base.hpp>

namespace mirism::handler
{
	// exact rules to handle the requests
	class Base
	{
		public: virtual ~Base() = default;
		public: virtual std::optional<Instance::Response>
			operator()(Instance::Request request, std::unique_ptr<client::Base> client) = 0;
	};
}
