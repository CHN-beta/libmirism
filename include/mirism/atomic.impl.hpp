# pragma once

# include <mirism/atomic.hpp>

namespace mirism
{
	template <typename T> inline Atomic<T>::Atomic(const T& value)
		: Value_(value) {}
	template <typename T> inline Atomic<T>::Atomic(const Atomic& other)
		: Value_(other()) {}
	template <typename T> inline Atomic<T>& Atomic<T>::operator=(const Atomic& other)
	{
		if (&other == this)
			return *this;
		std::lock_guard<std::mutex> lock(Mutex_);
		Value_ = other();
		return *this;
	}
	template <typename T> inline Atomic<T>& Atomic<T>::operator=(const T& value)
	{
		std::lock_guard<std::mutex> lock(Mutex_);
		Value_ = value;
		return *this;
	}
	
	template <typename T> inline T Atomic<T>::read() const
	{
		std::lock_guard<std::mutex> lock(Mutex_);
		return Value_;
	}
	template <typename T> template <typename ReadFunction> inline auto
		Atomic<T>::read(ReadFunction read_function) const
		-> decltype(read_function(Value_))
		{
			std::lock_guard<std::mutex> lock(Mutex_);
			return read_function(Value_);
		}
	template <typename T> template <typename ReadFunction, typename WaitFunction> inline auto
		Atomic<T>::read
		(ReadFunction read_function, WaitFunction wait_function, std::chrono::steady_clock::duration timeout) const
		-> std::optional<decltype(read_function(Value_))>
		{
			std::unique_lock<std::mutex> lock(Mutex_);
			if (ConditionVariable_.wait_for(lock, timeout, std::bind(wait_function, std::cref(Value_))))
				return read_function(Value_);
			return std::nullopt;
		}
	template <typename T> template <typename WriteFunction> inline auto
		Atomic<T>::write(WriteFunction write_function)
		-> decltype(write_function(Value_))
		{
			std::lock_guard<std::mutex> lock(Mutex_);
			return write_function(Value_);
		}
	template <typename T> template <typename WriteFunction, typename WaitFunction> inline auto
		Atomic<T>::write
		(WriteFunction write_function, WaitFunction wait_function, std::chrono::steady_clock::duration timeout)
		-> std::conditional_t
		<
			std::same_as<decltype(write_function(Value_)), void>,
			WaitResult,
			std::optional<decltype(write_function(Value_))>
		>
		{
			std::unique_lock<std::mutex> lock(Mutex_);
			if constexpr (std::same_as<decltype(write_function(Value_)), void>)
			{
				if (ConditionVariable_.wait_for(lock, timeout, std::bind(wait_function, std::cref(Value_))))
				{
					write_function(Value_);
					return WaitResult::Success;
				}
				else
					return WaitResult::Timeout;
			}
			else
			{
				if (ConditionVariable_.wait_for(lock, timeout, std::bind(wait_function, std::cref(Value_))))
					return write_function(Value_);
				else
					return std::nullopt;
			}
		}
	template <typename T> inline
		typename Atomic<T>::WaitResult Atomic<T>::wait
		(std::function<bool(const T&)> wait_function, std::chrono::steady_clock::duration timeout) const
		{
			std::unique_lock<std::mutex> lock(Mutex_);
			if (ConditionVariable_.wait_for(lock, timeout, std::bind(wait_function, std::cref(Value_))))
				return WaitResult::Success;
			return WaitResult::Timeout;
		}
}
