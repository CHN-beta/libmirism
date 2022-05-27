# pragma once
# include <mirism/pipe.hpp>

namespace mirism
{
	inline Pipe::PushResult Pipe::push(std::variant<std::string, Signal> value)
	{
		Logger::Guard guard;
		auto&& lock = Queue_.lock([](auto& queue){return queue.size() < 1024;}, 10s);
		if (lock)
		{
			guard.log<Logger::Level::Debug>("lock successed.");
			lock.value()->push(std::move(value));
			return PushResult::Success;
		}
		else
		{
			guard.log<Logger::Level::Error>("lock failed.");
			return PushResult::Failure;
		}
	}
	inline std::optional<std::variant<std::string, Pipe::Signal>> Pipe::pop()
	{
		Logger::Guard guard;
		auto&& lock = Queue_.lock([](auto& queue){return !queue.empty();}, 10s);
		if (lock)
		{
			guard.log<Logger::Level::Debug>("lock successed.");
			auto value = std::move(lock.value()->front());
			lock.value()->pop();
			return value;
		}
		else
		{
			guard.log<Logger::Level::Error>("lock failed.");
			return std::nullopt;
		}
	}
	inline std::optional<std::variant<std::string, Pipe::Signal>> Pipe::front() const
	{
		Logger::Guard guard;
		auto&& lock = Queue_.lock([](auto& queue){return !queue.empty();}, 10s);
		if (lock)
		{
			guard.log<Logger::Level::Debug>("lock successed.");
			return lock.value()->front();
		}
		else
		{
			guard.log<Logger::Level::Error>("lock failed.");
			return std::nullopt;
		}
	}
	inline bool Pipe::empty() const
		{return Queue_.lock()->empty();}
}
