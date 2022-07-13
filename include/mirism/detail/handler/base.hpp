# pragma once
# include <mirism/detail/client/base.hpp>

namespace mirism::handler
{
	// exact rules to handle the requests
	class Base
	{
		public: virtual ~Base() = default;
		public: virtual std::unique_ptr<Instance::Response> operator()
		(
			std::unique_ptr<Instance::Request> request = nullptr,
			std::experimental::observer_ptr<client::Base> client = nullptr
		) = 0;
	};
}
