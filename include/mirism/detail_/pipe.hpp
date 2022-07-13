# pragma once
# include <mirism/detail_/logger.hpp>

namespace mirism
{
	class Pipe : public Logger::ObjectMonitor<Pipe>
	{
		public: enum class Signal
			{
				EndOfFile,
				Break
			};

		protected: Atomic<std::queue<std::variant<std::string, Signal>>> Queue_;

		public: Pipe() = default;
		public: Pipe(const Pipe&) = delete;
		public: Pipe(Pipe&&) = delete;
		public: Pipe& operator=(const Pipe&) = delete;
		public: Pipe& operator=(Pipe&&) = delete;

		// Try to push a string or signal into the pipe.
		// Blocks up to 10s if the queue is full.
		public: enum class PushResult
		{
			Success,
			Failure
		};
		public: PushResult push(std::variant<std::string, Signal> value);

		// Try to pop a string or signal from the pipe.
		// Blocks up to 10s if the queue is empty.
		public: std::optional<std::variant<std::string, Signal>> pop();

		// Try to get first element from the pipe.
		// Blocks up to 10s if the queue is empty.
		public: std::optional<std::variant<std::string, Signal>> front() const;

		// Returns immediately.
		public: bool empty() const;
	};
}
