# pragma once
# include <mirism/detail/server/httplib.hpp>

namespace mirism::server
{
	inline httplib::Server::HandlerWithResponse detail_::HttplibBase::create_handler_wrap_
		(std::function<Instance::Response(Instance::Request)> handler)
	{
		auto handler_wrapper = [handler](const auto& request_httplib, auto& response_httplib)
		{
			Logger::Guard log;
			Instance::Request request;

			// copy information from request_httplib to request
			request.Version = request_httplib.version;
			request.Method = request_httplib.method;
			request.Path = request_httplib.path;
			if (!request_httplib.params.empty())
			{
				request.Path += "?";
				for (auto& param : request_httplib.params)
					request.Path += param.first + "=" + param.second + "&";
				request.Path.pop_back();
			}
			for (auto& header : request_httplib.headers)
				request.Headers.emplace(header.first, header.second);
			if (!request_httplib.body.empty())
			{
				auto pipe = std::make_shared<Pipe>();
				pipe->push(request_httplib.body);
				request.Body = pipe;
			}

			log.log<Logger::Level::Debug>("send request {}."_f(request));
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
