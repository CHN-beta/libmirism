# pragma once
# include <mirism/logger.hpp>

namespace mirism
{
	namespace server {class Base;}
	namespace client {class Base;}
	namespace domain {class Base;}

	// Framework object of libmirism.
	class Instance : public Logger::ObjectMonitor<Instance>
	{
		public: struct EndPoint
		{
			std::variant<std::uint32_t, std::uint128_t> IP;
			std::uint16_t Port;
		};

		protected: std::unique_ptr<server::Base> Server_;
		protected: std::unique_ptr<client::Base> Client_;
		protected: std::unique_ptr<handler::Base> Handler_;

		public: Instance
		(
			std::unique_ptr<server::Base> server, std::unique_ptr<client::Base> client,
			std::unique_ptr<handler::Base> handler
		);
		public: virtual ~Instance();

		public: void run(bool async = false);
	};
}
