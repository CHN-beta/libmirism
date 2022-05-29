# pragma once
# include <mirism/pipe.hpp>

namespace mirism
{
	namespace server {class Base;}
	namespace client {class Base;}
	namespace handler {class Base;}

	// Framework object of libmirism.
	class Instance : public Logger::ObjectMonitor<Instance>
	{

		// All the content in EndPoint, Request and Response should be legal (e.g. Host only contains legal characters,
		// and so on)
		// Besides, the following restrictions are imposed:
		// 	* Version should be std::nullopt or "1.0", "1.1", "2", "3";
		// 	* Host is in lower case and have no leading or trailing '.';
		// 	* Keys in Headers are in lower case;
		//  * Method is in upper case, and only these values are allowed:
		// 		"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH" 
		//  * For methods that do not allowed to have content, Body should be set as nullptr; if method is allowed to
		// 		have content, Body should be set as non-nullptr.
		public: struct EndPoint
		{
			std::variant<std::uint32_t, uint128_t> IP;
			std::uint16_t Port;
		};
		struct Request
		{
			std::optional<std::string> Version;
			std::string Method;
			std::string Host;
			std::string Path;
			std::multimap<std::string, std::string> Headers;
			std::shared_ptr<Pipe> Body;
			std::optional<EndPoint> Remote;
		};
		struct Response
		{
			std::string Status;
			std::multimap<std::string, std::string> Headers;
			std::shared_ptr<Pipe> Body;
			const std::optional<Instance::EndPoint> Remote;
		};

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
}
