# pragma once
# include <mirism/instance.hpp>

namespace mirism::client
{
	// An implementation of HTTP client, e.g. poco, cpp-httplib, nghttp2, ...
	class Base
	{
		public: virtual ~Base() = default;
		public: virtual std::optional<Response> operator()(Request) = 0;
	};
};
