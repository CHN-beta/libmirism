# pragma once
# include <mirism/detail_/instance.hpp>

namespace mirism::server
{
	// An implementation of HTTP server, e.g. poco, cpp-httplib, nghttp2, ...
	class Base
	{
		public: virtual ~Base() = default;

		// Start the server. If async is false, the server will run in sync mode, return value will be nullptr;
		// otherwise, the server will run in async mode. If start failed, return value will be nullptr; otherwise,
		// return value will be a callback function to stop the server.
		public: virtual std::move_only_function<void()> operator()
		(
			bool async = false,
			std::experimental::observer_ptr<handler::Base> handler = nullptr,
			std::experimental::observer_ptr<client::Base> client = nullptr
		) = 0;
	};
}
