# pragma once

# include <mirism/common.hpp>

namespace mirism
{
	template <typename T>
		class Atomic
	{
		protected:
			mutable std::mutex Mutex_;
			mutable std::condition_variable ConditionVariable_;
			T Value_;

		public:
			Atomic() = default;
			Atomic(const T& value);
			Atomic(const Atomic& other);
			Atomic& operator=(const Atomic& other);
			Atomic& operator=(const T& value);

			enum class WaitResult
			{
				Success,
				Timeout
			};

			T read() const;
			template <typename ReadFunction> auto
				read(ReadFunction read_function) const
				-> decltype(read_function(Value_));
			template <typename ReadFunction, typename WaitFunction> auto
				read
				(
					ReadFunction read_function,
					WaitFunction wait_function, std::chrono::steady_clock::duration timeout
				) const
				-> std::optional<decltype(read_function(Value_))>;
			template <typename WriteFunction> auto
				write(WriteFunction write_function)
				-> decltype(write_function(Value_));
			template <typename WriteFunction, typename WaitFunction> auto
				write
				(WriteFunction write_function, WaitFunction wait_function, std::chrono::steady_clock::duration timeout)
				-> std::conditional_t
				<
					std::same_as<decltype(write_function(Value_)), void>,
					WaitResult,
					std::optional<decltype(write_function(Value_))>
				>;
			WaitResult wait
				(std::function<bool(const T&)> wait_function, std::chrono::steady_clock::duration timeout) const;
	};
}
