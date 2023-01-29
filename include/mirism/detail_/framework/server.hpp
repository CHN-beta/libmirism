# pragma once
# include <mirism/utility.hpp>

namespace mirism::server
{
	class Base
	{
		public: virtual ~Base() = default;

		// start server, and handle requests using the callback. Return a function to stop the server, or nullptr if
		//	the server is already running or the call is blocked until the server is stopped.
		public: virtual std::function<void()> operator()
			(std::function<std::unique_ptr<http::Response>(std::unique_ptr<http::Request>)> callback) = 0;
	};
}
