# pragma once
# include <mirism/detail_/client/httplib.hpp>
# include <mirism/detail_/misc/format.hpp>

namespace mirism::client
{
	inline std::unique_ptr<Instance::Response> Httplib::operator()(std::unique_ptr<Instance::Request> request)
	{
		Logger::Guard log(request);

		if (!request) [[unlikely]]
			return nullptr;

		std::string host, scheme;

		// find out host
		auto hosts = request->Headers.equal_range("Host");
		if (hosts.first == hosts.second) [[unlikely]]
		{
			log.log<Logger::Level::Error>("Host header is not found.");
			return nullptr;
		}
		else if (std::distance(hosts.first, hosts.second) > 1) [[unlikely]]
			log.log<Logger::Level::Error>("Host header is duplicated, use the first one");
		host = hosts.first->second;

		// find out scheme
		if (!request->Scheme) [[unlikely]]
		{
			log.log<Logger::Level::Error>("Scheme is not found, use https as default");
			scheme = "https://";
		}
		else if (request->Scheme.value() == Instance::HttpScheme::Http)
			scheme = "http://";
		else if (request->Scheme.value() == Instance::HttpScheme::Https)
			scheme = "https://";
		else [[unlikely]]
		{
			log.log<Logger::Level::Error>("Scheme is invalid, use https as default");
			scheme = "https://";
		}

		// construct the client object
		auto client = httplib::Client(scheme + host);
		if (DisableServerCertificateVerification)
			client.enable_server_certificate_verification(false);

		// read the whole body
		std::string body;
		if (request->Body)
			while (auto chunk = request->Body->pop())
			{
				if (std::holds_alternative<std::string>(*chunk))
					body += std::get<std::string>(*chunk);
				else
					break;
			}

		// check if the request is cancelled
		if (request->Cancelled && request->Cancelled->get())
			return nullptr;

		// construct the request object
		httplib::Request httplib_request;
		httplib_request.method = "{}"_f(request->Method);
		for (auto& c : httplib_request.method)
			c = std::toupper(c);
		httplib_request.path = request->Path;
		for (auto [name, value] : request->Headers)
			httplib_request.headers.emplace(name, value);
		httplib_request.body = body;

		// send the request
		httplib::Response httplib_response;
		httplib::Error httplib_error;
		client.send(httplib_request, httplib_response, httplib_error);
		if (httplib_error != httplib::Error::Success) [[unlikely]]
		{
			log.log<Logger::Level::Error>("httplib failed {}"_f(httplib_error));
			return nullptr;
		}

		auto response = std::make_unique<Instance::Response>();
		response->Status = httplib_response.status;
		for (auto& [name, value] : httplib_response.headers)
			response->Headers.emplace(name, value);
		if (!httplib_response.body.empty())
		{
			response->Body = std::make_shared<Pipe>();
			response->Body->push(httplib_response.body);
			response->Body->push(Pipe::Signal::EndOfFile);
		}
		return response;
	}
}
