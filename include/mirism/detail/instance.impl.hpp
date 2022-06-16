# pragma once
# include <mirism/detail/instance.hpp>
# include <mirism/detail/server/base.hpp>
# include <mirism/detail/client/base.hpp>
# include <mirism/detail/handler/base.hpp>

namespace mirism
{
	inline Instance::Request Instance::normalize(Request request)
	{
		Logger::Guard log(request);
		if (request.Version && !std::set<std::string>{"1.0", "1.1", "2", "3"}.contains(*request.Version))
		{
			auto old_version = request.Version;
			// Version is empty string
			if (std::regex_match(*request.Version, R"(\s*)"_re))
			{
				request.Version = std::nullopt;
				log.log<Logger::Level::Info>("Convert Version from {} to {}."_f(old_version, request.Version));
			}
			// http/x.x or h/x.x or hx.x
			else if (std::smatch match; std::regex_match
			(
				*request.Version, match,
				std::regex(R"(\s*(http|h)\s*(/?)\s*(1\.0|1\.1|1|2\.0|2|3))", std::regex_constants::icase)
			))
			{
				if (match[3].str() == "1")
					request.Version = "1.0";
				else if (match[3].str() == "2.0")
					request.Version = "2";
				else
					request.Version = match[3].str();
				log.log<Logger::Level::Info>("Convert Version from {} to {}."_f(old_version, request.Version));
			}
			// quic to 3
			else if (std::regex_match(*request.Version, std::regex(R"(\s*quic\s*)", std::regex_constants::icase)))
			{
				request.Version = "3";
				log.log<Logger::Level::Info>("Convert Version from {} to {}."_f(old_version, request.Version));
			}
			else
			{
				log.log<Logger::Level::Error>("Invalid Version {}, remove it."_f(request.Version));
				request.Version = std::nullopt;
			}
		}
		
	}
	inline Instance::Instance
	(
		std::unique_ptr<server::Base> server, std::unique_ptr<client::Base> client,
		std::unique_ptr<handler::Base> handler
	)
	: Server_(std::move(server)), Client_(std::move(client)), Handler_(std::move(handler))
	{
		Logger::Guard guard(Server_, Client_, Handler_);
		if (!Server_ || !Client_ || !Handler_)
			guard.log<Logger::Level::Error>("nullptr is not allowed, will run anyway");
	}
	inline void Instance::run(bool async)
		{(*Server_)(async, [this](auto request){return (*Handler_)(request, *Client_);});}
	inline std::ostream& operator<<(std::ostream& os, const Instance::Request& request)
	{
		return os << "{} {} {} {} {} {} {} {} {} {}"_f
		(
			request.Version, request.Method, request.Path, request.Headers, request.Body,
			request.RemoteIP, request.RemotePort, request.LocalIP, request.LocalPort
		);
	}
	inline std::ostream& operator<<(std::ostream& os, const Instance::Response& response)
	{
		return os << "{} {} {} {} {}"_f
		(
			response.Status, response.Headers, response.Body,
			response.RemoteIP, response.RemotePort, response.LocalIP, response.LocalPort
		);
	}
}
