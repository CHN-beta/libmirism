# pragma once
# include <mirism/detail_/backend/httplib.hpp>
# include <mirism/detail_/server/base.hpp>

namespace mirism::server
{
	namespace detail_
	{
		class HttplibBase : public Base
		{
			protected: httplib::Server::HandlerWithResponse create_handler_wrap_
			(
				Instance::HttpScheme scheme, std::experimental::observer_ptr<handler::Base> handler,
				std::experimental::observer_ptr<client::Base> client
			);
		};
	}
	template <bool UseTls> class Httplib;
	template <> class Httplib<false> : public detail_::HttplibBase, public Logger::ObjectMonitor<Httplib<false>>
	{
		protected: std::unique_ptr<httplib::Server> Server_;
		protected: std::string Host_;
		protected: int Port_;
		public: Httplib(std::string host, int port);
		public: std::move_only_function<void()> operator()
		(
			std::experimental::observer_ptr<handler::Base> handler, std::experimental::observer_ptr<client::Base> client
		) override;
	};
	template <> class Httplib<true> : public detail_::HttplibBase, public Logger::ObjectMonitor<Httplib<true>>
	{
		protected: std::unique_ptr<httplib::SSLServer> Server_;
		protected: std::string Host_;
		protected: int Port_;
		protected: std::string CertPath_, KeyPath_;
		public: Httplib(std::string host, int port, std::string cert_path, std::string key_path);
		public: std::move_only_function<void()> operator()
		(
			std::experimental::observer_ptr<handler::Base> handler, std::experimental::observer_ptr<client::Base> client
		) override;
	};
}
