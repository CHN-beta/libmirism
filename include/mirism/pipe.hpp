# pragma once

# include <mirism/atomic.hpp>

namespace mirism
{
	class Pipe
	{
		public:
			enum class Signal
			{
				EndOfFile,
				Break
			};

		protected:
			Atomic<std::queue<std::variant<std::string, Signal>>> Queue_;

			Pipe() = default;
		
		public:
			static std::shared_ptr<Pipe> create();

			Pipe(const Pipe&) = delete;
			Pipe(Pipe&&) = delete;
			Pipe& operator=(const Pipe&) = delete;
			Pipe& operator=(Pipe&&) = delete;

			// Try to push a string or signal into the pipe.
			// Blocks up to 10s if the queue is full.
			enum class PushResult
			{
				Success,
				Failure
			};
			PushResult push(std::variant<std::string, Signal> value);

			// Try to pop a string or signal from the pipe.
			// Blocks up to 10s if the queue is empty.
			std::optional<std::variant<std::string, Signal>> pop();

			// Try to get first element from the pipe.
			// Blocks up to 10s if the queue is empty.
			std::optional<std::variant<std::string, Signal>> front() const;

			// Returns immediately.
			bool empty() const;
	};
}
