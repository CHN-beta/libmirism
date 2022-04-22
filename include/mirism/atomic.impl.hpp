# pragma once

# include <mirism/atomic.hpp>

namespace mirism
{
	template <typename T> inline
		Atomic<T>::Atomic
		(const T& value)
		: Value_(value)
		{}
	template <typename T> inline
		Atomic<T>::Atomic
		(const Atomic& other)
		: Value_(other())
		{}
	template <typename T> inline
		Atomic<T>&
		Atomic<T>::operator=
		(const Atomic& other)
		{
			if (&other == this)
				return *this;
			std::lock_guard<std::mutex> lock(Mutex_);
			Value_ = other();
			return *this;
		}
	template <typename T> inline
		Atomic<T>&
		Atomic<T>::operator=
		(const T& value)
		{
			std::lock_guard<std::mutex> lock(Mutex_);
			Value_ = value;
			return *this;
		}
	
	template <typename T> inline
		T
		Atomic<T>::operator()
		() const
		{
			std::lock_guard<std::mutex> lock(Mutex_);
			return Value_;
		}
	template <typename T> template <typename U> inline
		U
		Atomic<T>::operator()
		(std::function<U(T&)> write_function)
		{
			std::lock_guard<std::mutex> lock(Mutex_);
			return write_function(Value_);
		}
	template <typename T> template <typename U> inline
		U
		Atomic<T>::operator()
		(std::function<U(const T&)> read_function) const
		{
			std::lock_guard<std::mutex> lock(Mutex_);
			return read_function(Value_);
		}
	template <typename T> inline
		typename Atomic<T>::WaitResult
		Atomic<T>::operator()
		(std::function<bool(const T&)> wait_function, std::chrono::steady_clock::duration timeout) const
		{
			std::unique_lock<std::mutex> lock(Mutex_);
			if (ConditionVariable_.wait_for(lock, timeout, wait_function))
				return WaitResult::Success;
			return WaitResult::Timeout;
		}
	template <typename T> template <typename U> inline
		std::tuple<std::optional<U>, typename Atomic<T>::WaitResult>
		Atomic<T>::operator()
		(
			std::function<U(T&)> write_function,
			std::function<bool(const T&)> wait_function, std::chrono::steady_clock::duration timeout
		)
		{
			std::unique_lock<std::mutex> lock(Mutex_);
			if (ConditionVariable_.wait_for(lock, timeout, wait_function))
				return {write_function(Value_), WaitResult::Success};
			return {std::nullopt, WaitResult::Timeout};
		}
	template <typename T> template <typename U> inline
		std::tuple<std::optional<U>, typename Atomic<T>::WaitResult>
		Atomic<T>::operator()
		(
			std::function<U(const T&)> read_function,
			std::function<bool(const T&)> wait_function, std::chrono::steady_clock::duration timeout
		) const
		{
			std::unique_lock<std::mutex> lock(Mutex_);
			if (ConditionVariable_.wait_for(lock, timeout, wait_function))
				return {read_function(Value_), WaitResult::Success};
			return {std::nullopt, WaitResult::Timeout};
		}
}
