# pragma once
# include <mirism/detail/pipe.hpp>

namespace mirism
{
	namespace server {class Base;}
	namespace client {class Base;}
	namespace handler {class Base;}

	// Framework object of libmirism.
	class Instance : public Logger::ObjectMonitor<Instance>
	{

		// All the content in EndPoint, Request and Response should be legal (e.g. Host only contains legal characters,
		// and so on). Besides, the following restrictions are imposed:
		// 	* Version should be std::nullopt or "1.0", "1.1", "2", "3".
		//  * Method is in upper case, and only these values are allowed:
		// 		"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH"
		//  * If there is no reqeust or response body, Body could be set either as nullptr or a pipe with immediate EOF.
		//  * Domain is in lower case and have no leading or trailing '.'. It may be extracted from Host header or not,
		//      depending on the implementation. It must not be an ip address, and must not have following port number.
		// 	* Keys in Headers are in lower case;
		//  * Status should be in range [100, 599].
		// It is not convinient to check the content meets these restrictions or not at every write time, so that we use
		// function normalize to do some transform to make sure the content meets these restrictions.
		public: struct Request
		{
			std::optional<std::string> Version;
			std::string Method;
			std::optional<std::string> Domain;
			std::string Path;
			std::multimap<std::string, std::string> Headers;
			std::shared_ptr<Pipe> Body;
			std::optional<std::uint32_t> RemoteIP;
			std::optional<std::uint16_t> RemotePort;
			std::optional<std::uint32_t> LocalIP;
			std::optional<std::uint16_t> LocalPort;
		};
		public: struct Response
		{
			std::uint16_t Status;
			std::multimap<std::string, std::string> Headers;
			std::shared_ptr<Pipe> Body;
			std::optional<std::uint32_t> RemoteIP;
			std::optional<std::uint16_t> RemotePort;
			std::optional<std::uint32_t> LocalIP;
			std::optional<std::uint16_t> LocalPort;
		};
		public: static Request normalize(Request request);
		public: static Response normalize(Response response);

		protected: std::unique_ptr<server::Base> Server_;
		protected: std::unique_ptr<client::Base> Client_;
		protected: std::unique_ptr<handler::Base> Handler_;

		public: Instance
		(
			std::unique_ptr<server::Base> server, std::unique_ptr<client::Base> client,
			std::unique_ptr<handler::Base> handler
		);
		public: virtual ~Instance() = default;

		public: void run(bool async = false);
	};
	std::ostream& operator<<(std::ostream& os, const Instance::Request& request);
	std::ostream& operator<<(std::ostream& os, const Instance::Response& response);
}
