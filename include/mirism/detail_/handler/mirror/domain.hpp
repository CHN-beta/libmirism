# pragma once
# include <mirism/detail_/handler/mirror/base.hpp>
# include <mirism/detail_/site/base.hpp>

namespace mirism::handler::mirror
{
	class Domain : public mirism::handler::mirror::Base, Logger::ObjectMonitor<Domain>
	{
		public: virtual ~Domain() = default;
		
		public: virtual std::optional<std::string> url_convert
			(std::observer_ptr<const Instance::Request> request, std::string url) = 0;
		public: virtual std::optional<std::string> url_convert
			(std::observer_ptr<const Instance::Response> response, std::string url) = 0;
	}
}
