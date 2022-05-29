# pragma once
# include <mirism/client/base.hpp>

namespace mirism::handler
{
	// exact rules to handle the requests
	class Base
	{
		public: virtual ~Base() = default;
		public: virtual Instance::Response operator()(Instance::Request request, client::Base& client) = 0;
	};
}
