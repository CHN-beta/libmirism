# pragma once
# include <mirism/detail_/client/base.hpp>
# include <mirism/detail_/backend/httplib.hpp>

namespace mirism::client
{
	class Httplib : public Base, public Logger::ObjectMonitor<Httplib>
	{
		public: Atomic<bool> DisableServerCertificateVerification = false;
		public: std::unique_ptr<Instance::Response> operator()
			(std::unique_ptr<Instance::Request> request = nullptr) override;
	};
}
