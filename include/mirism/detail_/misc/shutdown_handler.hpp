# pragma once
# include <mirism/detail_/misc/logger.hpp>

namespace mirism
{
	class ShutdownHandler : public Logger::ObjectMonitor<ShutdownHandler>
	{
		public: enum class Status
		{
			CouldNotShutdown,
			Running,
			ShutdownRequested,
			ShutdownCompleted
		};
		protected: std::shared_ptr<Atomic<Status>> Status_;

		public: explicit ShutdownHandler(bool could_be_shutdown = false);
		public: ShutdownHandler(const ShutdownHandler&) = default;
		public: ShutdownHandler(ShutdownHandler&&) = default;

		public: ShutdownHandler& request_shutdown();
		public: bool is_shutdown_requested() const;
		public: bool 
		public: bool is_shutdown_complete() const;
		public: ShutdownHandler& wait_for_shutdown_complete() const;
		public: Status get_status() const;
	};
}
