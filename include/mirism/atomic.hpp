# pragma once

# include <mirism/common.hpp>

namespace mirism
{
	template <typename T> class Atomic
	{
		protected: mutable std::mutex Mutex_;
		protected: mutable std::condition_variable ConditionVariable_;
		protected: T Value_;

		public: Atomic() = default;
		public: Atomic(const T& value);
		public: Atomic(const Atomic& other);
		public: Atomic& operator=(const Atomic& other);
		public: Atomic& operator=(const T& value);

		public: enum class WaitResult
		{
			Success,
			Timeout
		};

		public: T read() const;
		public: template <typename ReadFunction> auto read(ReadFunction read_function) const
			-> decltype(read_function(Value_));
		public: template <typename ReadFunction, typename WaitFunction> auto read
			(ReadFunction read_function, WaitFunction wait_function, std::chrono::steady_clock::duration timeout) const
			-> std::optional<decltype(read_function(Value_))>;
		public: template <typename WriteFunction> auto write(WriteFunction write_function)
			-> decltype(write_function(Value_));
		public: template <typename WriteFunction, typename WaitFunction> auto write
			(WriteFunction write_function, WaitFunction wait_function, std::chrono::steady_clock::duration timeout)
			-> std::conditional_t
			<
				std::same_as<decltype(write_function(Value_)), void>,
				WaitResult,
				std::optional<decltype(write_function(Value_))>
			>;
		public: WaitResult wait
			(std::function<bool(const T&)> wait_function, std::chrono::steady_clock::duration timeout) const;
	};
}
