# pragma once
# include <mirism/instance.hpp>

namespace mirism::server
{
	// An implementation of HTTP server, e.g. poco, cpp-httplib, nghttp2, ...
	class Base
	{

		public: class ServerRequest
		{
			public: std::optional<std::string> Version;
			public: std::string Method;
			public: std::string Host;
			public: std::string Path;
			public: std::multimap<std::string, std::string> Headers;
			public: std::shared_ptr<Pipe> Body;
			public: std::optional<EndPoint> Remote;
		};
		public: class ServerResponse
		{
			public: std::string Status;
			public: std::multimap<std::string, std::string> Headers;
			public: std::shared_ptr<Pipe> Body;
		};

		public: virtual ~Base() = default;
		public: virtual void operator()
			(bool async, std::function<Instance<>::ServerResponse(Instance<>::ServerRequest)> handler) = 0;
	};
}
