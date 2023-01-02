# pragma once
# include <mirism/utility.hpp>

namespace mirism::client
{
	class Base
	{
		public: virtual ~Base() = default;

		// fetch a request from upstream
		public: virtual std::optional<http::Response_t> operator()(http::Request_t request) = 0;
	};
}
