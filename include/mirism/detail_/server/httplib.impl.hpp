# pragma once
# include <mirism/detail_/server/httplib.hpp>

namespace mirism::server
{
	inline httplib::Server::HandlerWithResponse detail_::HttplibBase::create_handler_wrap_
	(
		Instance::HttpScheme scheme, std::experimental::observer_ptr<handler::Base> handler,
		std::experimental::observer_ptr<client::Base> client
	)
	{
		auto handler_wrapper = [=](const auto& request_httplib, auto& response_httplib)
		{
			Logger::Guard log;

			if (!handler) [[unlikely]]
				log.log<Logger::Level::Error>("No handler.");
			else
			{
				auto request = std::make_unique<Instance::Request>();

				// copy information from request_httplib to request
				if (request_httplib.version == "HTTP/1.0")
					request->Version = Instance::HttpVersion::v1_0;
				else if (request_httplib.version == "HTTP/1.1")
					request->Version = Instance::HttpVersion::v1_1;
				else
					log.log<Logger::Level::Error>("Unknown HTTP version: {}"_f(request_httplib.version));
				if
				(
					std::map<std::string, Instance::HttpMethod> map
					{
						{"GET", Instance::HttpMethod::Get},
						{"HEAD", Instance::HttpMethod::Head},
						{"POST", Instance::HttpMethod::Post},
						{"PUT", Instance::HttpMethod::Put},
						{"DELETE", Instance::HttpMethod::Delete},
						{"CONNECT", Instance::HttpMethod::Connect},
						{"OPTIONS", Instance::HttpMethod::Options},
						{"TRACE", Instance::HttpMethod::Trace},
						{"PATCH", Instance::HttpMethod::Patch}
					};
					map.contains(request_httplib.method)
				)
					request->Method = map[request_httplib.method];
				else
				{
					log.log<Logger::Level::Error>("Unknown HTTP method: {}, using GET."_f(request_httplib.method));
					request->Method = Instance::HttpMethod::Get;
				}
				request->Path = request_httplib.path;
				if (!request_httplib.params.empty())
				{
					request->Path += "?";
					for (auto& param : request_httplib.params)
						request->Path += param.first + "=" + param.second + "&";
					request->Path.pop_back();
				}
				for (auto& header : request_httplib.headers)
					request->Headers.emplace(header.first, header.second);
				if (!request_httplib.body.empty())
				{
					auto pipe = std::make_shared<Pipe>();
					pipe->push(request_httplib.body);
					request->Body = pipe;
				}
				log.log<Logger::Level::Debug>("Read request {}."_f(request));

				auto response = (*handler)(std::move(request), client);
				if (!response) [[unlikely]]
					log.log<Logger::Level::Error>("No response.");
				else
				{
					response_httplib.status = response->Status;
					
					for (auto& header : response->Headers)
						response_httplib.headers.emplace(header.first, header.second);
					if (response->Body)
						response_httplib.body = response->Body->pop();
					log.log<Logger::Level::Debug>("Write response {}."_f(response));
				}
				else
					log.log<Logger::Level::Error>("No response.");

				auto response = handler(request);
				log.log<Logger::Level::Debug>("receive response {}."_f(response));

				response_httplib.status = response.Status;
				for (auto& header : response.Headers)
					response_httplib.headers.emplace(header.first, header.second);
				if (response.Body && !response.Body->empty())
					while (auto value = response.Body->pop())
					{
						if (!value)
						{
							log.log<Logger::Level::Error>("Failed to pop from pipe.");
							break;
						}
						else if (std::holds_alternative<Pipe::Signal>(*value))
						{
							if (std::get<Pipe::Signal>(*value) == Pipe::Signal::Break)
								log.log<Logger::Level::Debug>("httplib does not support cancel.");
							break;
						}
						else
							response_httplib.body += std::get<std::string>(*value);
					}
			}
			else
				
			return httplib::Server::HandlerResponse::Handled;
		};
		return handler_wrapper;
	}
	inline Httplib<false>::Httplib(std::string host, int port)
		: Host_(host), Port_(port) {Logger::Guard guard(Host_, Port_);}
	inline void Httplib<false>::operator()
		(bool async, std::function<Instance::Response(Instance::Request)> handler)
	{
		httplib::Server svr;
		svr.set_pre_routing_handler(create_handler_wrap_(handler));
		if 
		svr.listen(Host_.c_str(), Port_);
	}
}
