# pragma once
# include <mirism/instance.hpp>

namespace mirism::server
{
	// An implementation of HTTP server, e.g. poco, cpp-httplib, nghttp2, ...
	class Base
	{
		public: virtual ~Base() = default;
		public: virtual void operator()(bool async, std::function<Instance::Response(Instance::Request)> handler) = 0;
	};
}
