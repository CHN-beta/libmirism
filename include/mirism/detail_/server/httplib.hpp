# pragma once
# include <mirism/detail_/backend/httplib.hpp>
# include <mirism/detail_/server/base.hpp>

namespace mirism::server
{
	namespace detail_
	{
		class HttplibBase : public Base
		{
			public: virtual ~HttplibBase() = default;
			protected: httplib::Server::HandlerWithResponse create_handler_wrap_
				(std::function<Instance::Response(Instance::Request)> handler);
		};
	}
	template <bool UseTls> class Httplib;
	template <> class Httplib<false> : public detail_::HttplibBase, public Logger::ObjectMonitor<Httplib<false>>
	{
		protected: std::unique_ptr<httplib::Server> Server_;
		protected: std::string Host_;
		protected: int Port_;
		public: Httplib(std::string host, int port);
		public: void operator()(bool async, std::function<Instance::Response(Instance::Request)> handler) override;
	};
	template <> class Httplib<true> : public detail_::HttplibBase, public Logger::ObjectMonitor<Httplib<true>>
	{
		protected: std::unique_ptr<httplib::SSLServer> Server_;
		protected: std::string Host_;
		protected: int Port_;
		protected: std::string CertPath_, KeyPath_;
		public: Httplib(std::string host, int port, std::string cert_path, std::string key_path);
		public: void operator()(bool async, std::function<Instance::Response(Instance::Request)> handler) override;
	};
}
