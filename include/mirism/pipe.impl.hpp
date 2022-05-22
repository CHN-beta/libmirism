# pragma once

# include <mirism/pipe.hpp>

namespace mirism
{
	inline std::shared_ptr<Pipe> Pipe::create()
		{return std::shared_ptr<Pipe>(new Pipe);}
	inline Pipe::PushResult Pipe::push(std::variant<std::string, Signal> value)
	{
		auto&& lock = Queue_.lock([](auto& queue){return queue.size() < 1024;}, 10s);
		if (lock)
		{
			lock.value()->push(std::move(value));
			return PushResult::Success;
		}
		else
			return PushResult::Failure;
	}
	inline std::optional<std::variant<std::string, Pipe::Signal>> Pipe::pop()
	{
		auto&& lock = Queue_.lock([](auto& queue){return !queue.empty();}, 10s);
		if (lock)
		{
			auto value = std::move(lock.value()->front());
			lock.value()->pop();
			return value;
		}
		else
			return std::nullopt;
	}
	inline std::optional<std::variant<std::string, Pipe::Signal>> Pipe::front() const
	{
		auto&& lock = Queue_.lock([](auto& queue){return !queue.empty();}, 10s);
		if (lock)
			return lock.value()->front();
		else
			return std::nullopt;
	}
	inline bool Pipe::empty() const
		{return Queue_.lock()->empty();}
}
