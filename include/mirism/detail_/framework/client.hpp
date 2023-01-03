# pragma once
# include <mirism/utility.hpp>

namespace mirism::client
{
	class Base
	{
		public: virtual ~Base() = default;

		// fetch a request from upstream
		public: virtual std::unique_ptr<http::Response_t> operator()(std::unique_ptr<http::Request_t> request) = 0;
	};
}
