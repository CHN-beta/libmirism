# pragma once
# include <mirism/detail_/misc/shutdown_handler.hpp>

namespace mirism
{
	ShutdownHandler::ShutdownHandler(bool could_be_shutdown)
		: Status_(std::make_shared<Atomic<Status>>(could_be_shutdown ? Status::Running : Status::CouldNotShutdown)) {}

	bool ShutdownHandler::request_shutdown()
	{
		Logger::Guard log;
		auto status = Status_->lock();
		if (*status == Status::CouldNotShutdown)
		{
			log.log<Logger::Level::Error>("could not shutdown.");
			return false;
		}
		if (*status == Status::Running)
			*status = Status::ShutdownRequested;
		return *this;
	}
	bool ShutdownHandler::is_shutdown_requested() const
	{
		Logger::Guard log;
		auto status = Status_->lock();
		log.rtn(*status == Status::ShutdownRequested || *status == Status::ShutdownCompleted);
	}
	bool ShutdownHandler::is_shutdown_complete() const
	{
		Logger::Guard log;
		auto status = Status_->lock();
		log.rtn(*status == Status::ShutdownCompleted);
	}

	void ShutdownHandler::wait_for_shutdown_complete() const
	{
		while (Status_ != Status::ShutdownCompleted)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	ShutdownHandler::Status ShutdownHandler::get_status() const
	{
		return Status_;
	}

	ShutdownHandler::Status ShutdownHandler::Status_ = Status::CouldNotShutdown;
}
