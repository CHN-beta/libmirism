# pragma once
# include <mirism/utility.hpp>

namespace mirism::client
{
	class Base
	{
		public: virtual ~Base() = default;

		// fetch a request from upstream
		public: virtual std::unique_ptr<http::Response> operator()(std::unique_ptr<http::Request> request) = 0;
	};
}
