// atomic.tpp:
//	class Atomic;

# pragma once
# include <mirism/detail_/misc/atomic.hpp>

namespace mirism
{
	template <decayed_type T> Atomic<T>::Atomic(const T& value)
		: Value_{value} {}
	template <decayed_type T> Atomic<T>::Atomic(T&& value)
		: Value_{std::move(value)} {}
	template <decayed_type T> Atomic<T>::Atomic(const Atomic& other)
		: Value_{other.get()} {}
	template <decayed_type T> Atomic<T>::Atomic(Atomic&& other)
		: Value_{std::move(other).get()} {}

	template <decayed_type T> Atomic<T>& Atomic<T>::operator=(const T& other)
	{
		std::scoped_lock lock{Mutex_};
		Value_ = other;
		ConditionVariable_.notify_all();
		return *this;
	}
	template <decayed_type T> Atomic<T>& Atomic<T>::operator=(T&& other)
	{
		std::scoped_lock lock{Mutex_};
		Value_ = std::move(other);
		ConditionVariable_.notify_all();
		return *this;
	}
	template <decayed_type T> Atomic<T>& Atomic<T>::operator=(const Atomic& other)
	{
		std::scoped_lock lock{Mutex_, other.Mutex_};
		Value_ = other.Value_;
		ConditionVariable_.notify_all();
		return *this;
	}
	template <decayed_type T> Atomic<T>& Atomic<T>::operator=(Atomic&& other)
	{
		std::scoped_lock lock{Mutex_, other.Mutex_};
		Value_ = std::move(other.Value_);
		ConditionVariable_.notify_all();
		return *this;
	}

	template <decayed_type T> T Atomic<T>::get() const&
	{
		std::scoped_lock lock{Mutex_};
		return Value_;
	}
	template <decayed_type T> T Atomic<T>::get() &&
	{
		std::scoped_lock lock{Mutex_};
		return std::move(Value_);
	}
	template <decayed_type T> Atomic<T>::operator T() const&
		{return get();}
	template <decayed_type T> Atomic<T>::operator T() &&
		{return std::move(*this).get();}

	template <decayed_type T> template <typename F> auto
		Atomic<T>::apply(F&& f) const -> decltype(f(Value_)) requires requires() {f(Value_);}
	{
		std::scoped_lock lock{Mutex_};
		return f(Value_);
	}
	template <decayed_type T> template <typename F> auto
		Atomic<T>::apply(F&& f) -> decltype(f(Value_)) requires requires() {f(Value_);}
	{
		std::scoped_lock lock{Mutex_};
		if constexpr (std::same_as<decltype(f(Value_)), void>)
		{
			f(Value_);
			ConditionVariable_.notify_all();
		}
		else
		{
			auto&& result = f(Value_);
			ConditionVariable_.notify_all();
			return std::forward<decltype(f(Value_))>(result);
		}
	}

	template <decayed_type T> template <typename F, typename ConditionF>
		auto Atomic<T>::apply(F&& f, ConditionF&& condition_f) const -> decltype(f(Value_))
		requires requires() {f(Value_); {condition_f(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_f]{return condition_f(Value_);});
		return f(Value_);
	}
	template <decayed_type T> template <typename F, typename ConditionF>
		auto Atomic<T>::apply(F&& f, ConditionF&& condition_f) -> decltype(f(Value_))
		requires requires() {f(Value_); {condition_f(std::declval<const T&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_f]{return condition_f(const_cast<const T&>(Value_));});
		if constexpr (std::same_as<decltype(f(Value_)), void>)
		{
			f(Value_);
			ConditionVariable_.notify_all();
		}
		else
		{
			auto&& result = f(Value_);
			ConditionVariable_.notify_all();
			return std::forward<decltype(f(Value_))>(result);
		}
	}

	template <decayed_type T> template <typename F, typename ConditionF>
		auto Atomic<T>::apply(F&& f, ConditionF&& condition_f, std::chrono::steady_clock::duration timeout) const
		-> std::conditional_t<std::same_as<decltype(f(Value_)), void>, bool, std::optional<decltype(f(Value_))>>
		requires requires() {f(Value_); {condition_f(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		if (ConditionVariable_.wait_for(lock, timeout, [this, &condition_f]{return condition_f(Value_);}))
		{
			if constexpr (std::same_as<decltype(f(Value_)), void>)
			{
				f(Value_);
				return true;
			}
			else
				return f(Value_);
		}
		else
		{
			if constexpr (std::same_as<decltype(f(Value_)), void>)
				return false;
			else
				return std::nullopt;
		}
	}
	template <decayed_type T> template <typename F, typename ConditionF>
		auto Atomic<T>::apply(F&& f, ConditionF&& condition_f, std::chrono::steady_clock::duration timeout)
		-> std::conditional_t<std::same_as<decltype(f(Value_)), void>, bool, std::optional<decltype(f(Value_))>>
		requires requires() {f(Value_); {condition_f(std::declval<const T&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		if (ConditionVariable_.wait_for(lock, timeout, [this, &condition_f]{return condition_f(const_cast<const T&>(Value_));}))
		{
			if constexpr (std::same_as<decltype(f(Value_)), void>)
			{
				f(Value_);
				ConditionVariable_.notify_all();
				return true;
			}
			else
			{
				auto&& result = f(Value_);
				ConditionVariable_.notify_all();
				return std::forward<decltype(f(Value_))>(result);
			}
		}
		else
		{
			if constexpr (std::same_as<decltype(f(Value_)), void>)
				return false;
			else
				return std::nullopt;
		}
	}

	template <decayed_type T> template <typename ConditionF> void Atomic<T>::wait(ConditionF&& condition_f) const
		requires requires() {{condition_f(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_f]{return condition_f(Value_);});
	}
	template <decayed_type T> template <typename ConditionF>
		bool Atomic<T>::wait(ConditionF&& condition_f, std::chrono::steady_clock::duration timeout) const
		requires requires() {{condition_f(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		return ConditionVariable_.wait_for(lock, timeout, [this, &condition_f]{return condition_f(Value_);});
	}

	template <decayed_type T> template <bool Const>
		Atomic<T>::Guard<Const>::Guard
		(
			std::unique_lock<std::recursive_mutex>&& lock,
			std::experimental::observer_ptr<std::conditional_t<Const, const Atomic<T>, Atomic<T>>> value
		)
		: Lock_{std::move(lock)}, Value_{value} {}
	template <decayed_type T> template <bool Const> Atomic<T>::Guard<Const>::Guard(Guard<Const>&& other)
		: Lock_{std::move(other.Lock_)}, Value_{other.Value_} {}
	template <decayed_type T> template <bool Const> Atomic<T>::Guard<Const>::~Guard()
		{Value_->ConditionVariable_.notify_all();}

	template <decayed_type T> template <bool Const>
		std::conditional_t<Const, const T&, T&> Atomic<T>::Guard<Const>::operator*() const
		{return Value_->Value_;}
	template <decayed_type T> template <bool Const>
		std::conditional_t<Const, const T*, T*> Atomic<T>::Guard<Const>::operator->() const
		{return &Value_->Value_;}
	template <decayed_type T> template <bool Const>
		std::conditional_t<Const, const T&, T&> Atomic<T>::Guard<Const>::value() const
		{return Value_->Value_;}

	template <decayed_type T> Atomic<T>::Guard<true> Atomic<T>::lock() const
		{return {std::unique_lock{Mutex_}, std::experimental::make_observer(this)};}
	template <decayed_type T> Atomic<T>::Guard<false> Atomic<T>::lock()
		{return {std::unique_lock{Mutex_}, std::experimental::make_observer(this)};}
	template <decayed_type T> template <typename ConditionF>
		Atomic<T>::Guard<true> Atomic<T>::lock(ConditionF&& condition_f) const
		requires requires() {{condition_f(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_f]{return condition_f(Value_);});
		return {{std::move(lock), std::experimental::make_observer(this)}};
	}
	template <decayed_type T> template <typename ConditionF>
		Atomic<T>::Guard<false> Atomic<T>::lock(ConditionF&& condition_f)
		requires requires() {{condition_f(std::declval<const T&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_f]{return condition_f(const_cast<const T&>(Value_));});
		return {{std::move(lock), std::experimental::make_observer(this)}};
	}
	template <decayed_type T> template <typename ConditionF> std::optional<typename Atomic<T>::Guard<true>>
		Atomic<T>::lock(ConditionF&& condition_f, std::chrono::steady_clock::duration timeout) const
		requires requires() {{condition_f(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		if (ConditionVariable_.wait_for(lock, timeout, [this, &condition_f]{return condition_f(Value_);}))
			return {{std::move(lock), std::experimental::make_observer(this)}};
		else
			return std::nullopt;
	}
	template <decayed_type T> template <typename ConditionF> std::optional<typename Atomic<T>::Guard<false>>
		Atomic<T>::lock(ConditionF&& condition_f, std::chrono::steady_clock::duration timeout)
		requires requires() {{condition_f(std::declval<const T&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		if
		(
			ConditionVariable_.wait_for
				(lock, timeout, [this, &condition_f]{return condition_f(const_cast<const T&>(Value_));})
		)
			return {{std::move(lock), std::experimental::make_observer(this)}};
		else
			return std::nullopt;
	}
}
