# pragma once
# include <mirism/detail/instance.hpp>

namespace mirism::client
{
	// An implementation of HTTP client, e.g. poco, cpp-httplib, nghttp2, ...
	class Base
	{
		public: virtual ~Base() = default;
		public: virtual std::unique_ptr<Instance::Response> operator()
			(std::unique_ptr<Instance::Request> request = nullptr) = 0;
	};
};
