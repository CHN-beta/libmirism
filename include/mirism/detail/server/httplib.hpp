# pragma once
# include <mirism/detail/backend/httplib.hpp>
# include <mirism/detail/server/base.hpp>

namespace mirism::server
{
	template <bool UseTls> class Httplib;
	template <> class Httplib<false> : public Base, public Logger::ObjectMonitor<Httplib<false>>
	{
		protected: std::unique_ptr<httplib::Server> Server_;
		protected: std::string Host_;
		protected: int Port_;
		public: Httplib(std::string host, int port);
		public: void operator()(bool async, std::function<Instance::Response(Instance::Request)> handler) override;
	};
	template <> class Httplib<true> : public Httplib<false>
	{
		protected: std::unique_ptr<httplib::Server> Server_;
		protected: std::string Host_;
		protected: int Port_;
		protected: std::string CertPath_, KeyPath_;
		public: Httplib(std::string host, int port, std::string cert_path, std::string key_path);
		public: void operator()(bool async, std::function<Instance::Response(Instance::Request)> handler) override;
	};
}
