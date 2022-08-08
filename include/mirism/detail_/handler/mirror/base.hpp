# pragma once
# include <mirism/detail_/handler/base.hpp>

namespace mirism::handler::mirror
{
	class Base : public mirism::handler::Base
	{
		public: virtual std::optional<std::string> url_convert
			(std::observer_ptr<const Instance::Request> request, std::string url) = 0;
		public: virtual std::optional<std::string> url_convert
			(std::observer_ptr<const Instance::Response> response, std::string url) = 0;
	}
}
