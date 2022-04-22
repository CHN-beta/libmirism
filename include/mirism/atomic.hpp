# pragma once

# include <mirism/common.hpp>

namespace mirism
{
	template <typename T>
		class Atomic
	{
		protected:
			mutable
				std::mutex
				Mutex_;
			mutable
				std::condition_variable
				ConditionVariable_;
			T
				Value_;

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

			T
				operator()
				() const;
			template <typename U>
				U
				operator()
				(std::function<U(T&)> write_function);
			template <typename U>
				U
				operator()
				(std::function<U(const T&)> read_function) const;
			WaitResult
				operator()
				(std::function<bool(const T&)> wait_function, std::chrono::steady_clock::duration timeout) const;
			template <typename U>
				std::tuple<std::optional<U>, WaitResult>
				operator()
				(
					std::function<U(T&)> write_function,
					std::function<bool(const T&)> wait_function, std::chrono::steady_clock::duration timeout
				);
			template <typename U>
				std::tuple<std::optional<U>, WaitResult>
				operator()
				(
					std::function<U(const T&)> read_function,
					std::function<bool(const T&)> wait_function, std::chrono::steady_clock::duration timeout
				) const;
	};
}
