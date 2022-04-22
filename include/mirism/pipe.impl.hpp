# pragma once

# include <mirism/pipe.hpp>

namespace mirism
{
	inline std::shared_ptr<Pipe> Pipe::create()
		{return std::shared_ptr<Pipe>(new Pipe);}
	inline Pipe::PushResult Pipe::push(std::variant<std::string, Signal> value)
	{
		auto result = Queue_.write
		(
			[&value](auto& queue){queue.push(std::move(value));},
			[](const auto& queue){return queue.size() < 1024;},
			10s
		);
		if (result == decltype(Queue_)::WaitResult::Success)
			return PushResult::Success;
		else
			return PushResult::Failure;
	}
	inline std::optional<std::variant<std::string, Pipe::Signal>> Pipe::pop()
	{
		auto result = Queue_.write
		(
			[](auto& queue)
			{
				auto value = std::move(queue.front());
				queue.pop();
				return value;
			},
			[](const auto& queue){return !queue.empty();},
			10s
		);
		return result;
	}
	inline std::optional<std::variant<std::string, Pipe::Signal>> Pipe::front() const
	{
		auto result = Queue_.read
		(
			[](const auto& queue){return queue.front();},
			[](const auto& queue){return !queue.empty();},
			10s
		);
		return result;
	}
	inline bool Pipe::empty() const
		{return Queue_.read([](const auto& queue){return queue.empty();});}
}
