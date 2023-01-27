# pragma once
# include <mirism/detail_/utility/atomic.hpp>

namespace mirism
{
	template <decayed_type Value_t> Atomic_t<Value_t>::Atomic_t(const Value_t& value)
		: Value_{value} {}
	template <decayed_type Value_t> Atomic_t<Value_t>::Atomic_t(Value_t&& value)
		: Value_{std::move(value)} {}
	template <decayed_type Value_t> Atomic_t<Value_t>::Atomic_t(const Atomic_t& other)
		: Value_{other.get()} {}
	template <decayed_type Value_t> Atomic_t<Value_t>::Atomic_t(Atomic_t&& other)
		: Value_{std::move(other).get()} {}

	template <decayed_type Value_t> Atomic_t<Value_t>& Atomic_t<Value_t>::operator=(const Value_t& other)
	{
		std::scoped_lock lock{Mutex_};
		Value_ = other;
		ConditionVariable_.notify_all();
		return *this;
	}
	template <decayed_type Value_t> Atomic_t<Value_t>& Atomic_t<Value_t>::operator=(Value_t&& other)
	{
		std::scoped_lock lock{Mutex_};
		Value_ = std::move(other);
		ConditionVariable_.notify_all();
		return *this;
	}
	template <decayed_type Value_t> Atomic_t<Value_t>& Atomic_t<Value_t>::operator=(const Atomic_t& other)
	{
		std::scoped_lock lock{Mutex_, other.Mutex_};
		Value_ = other.Value_;
		ConditionVariable_.notify_all();
		return *this;
	}
	template <decayed_type Value_t> Atomic_t<Value_t>& Atomic_t<Value_t>::operator=(Atomic_t&& other)
	{
		std::scoped_lock lock{Mutex_, other.Mutex_};
		Value_ = std::move(other.Value_);
		ConditionVariable_.notify_all();
		return *this;
	}

	template <decayed_type Value_t> Value_t Atomic_t<Value_t>::get() const&
	{
		std::scoped_lock lock{Mutex_};
		return Value_;
	}
	template <decayed_type Value_t> Value_t Atomic_t<Value_t>::get() &&
	{
		std::scoped_lock lock{Mutex_};
		return std::move(Value_);
	}
	template <decayed_type Value_t> Atomic_t<Value_t>::operator Value_t() const&
		{return get();}
	template <decayed_type Value_t> Atomic_t<Value_t>::operator Value_t() &&
		{return std::move(*this).get();}

	template <decayed_type Value_t> template <typename Function_t>
		auto Atomic_t<Value_t>::apply(Function_t&& function) const
		-> decltype(function(Value_)) requires requires() {function(Value_);}
	{
		std::scoped_lock lock{Mutex_};
		return function(Value_);
	}
	template <decayed_type Value_t> template <typename Function_t> auto Atomic_t<Value_t>::apply(Function_t&& function)
		-> decltype(function(Value_)) requires requires() {function(Value_);}
	{
		std::scoped_lock lock{Mutex_};
		if constexpr (std::same_as<decltype(function(Value_)), void>)
		{
			function(Value_);
			ConditionVariable_.notify_all();
		}
		else
		{
			auto&& result = function(Value_);
			ConditionVariable_.notify_all();
			return std::forward<decltype(function(Value_))>(result);
		}
	}

	template <decayed_type Value_t> template <typename Function_t, typename ConditionFunction_t>
		auto Atomic_t<Value_t>::apply(Function_t&& function, ConditionFunction_t&& condition_function) const
		-> decltype(function(Value_))
		requires requires() {function(Value_); {condition_function(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_function]{return condition_function(Value_);});
		return function(Value_);
	}
	template <decayed_type Value_t> template <typename Function_t, typename ConditionFunction_t>
		auto Atomic_t<Value_t>::apply(Function_t&& function, ConditionFunction_t&& condition_function)
		-> decltype(function(Value_))
		requires requires()
		{function(Value_); {condition_function(std::declval<const Value_t&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait
			(lock, [this, &condition_function]{return condition_function(const_cast<const Value_t&>(Value_));});
		if constexpr (std::same_as<decltype(function(Value_)), void>)
		{
			function(Value_);
			ConditionVariable_.notify_all();
		}
		else
		{
			auto&& result = function(Value_);
			ConditionVariable_.notify_all();
			return std::forward<decltype(function(Value_))>(result);
		}
	}

	template <decayed_type Value_t> template <typename Function_t, typename ConditionFunction_t>
		auto Atomic_t<Value_t>::apply
	(
		Function_t&& function, ConditionFunction_t&& condition_function, std::chrono::steady_clock::duration timeout
	) const
		-> std::conditional_t
			<std::same_as<decltype(function(Value_)), void>, bool, std::optional<decltype(function(Value_))>>
		requires requires() {function(Value_); {condition_function(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		if (ConditionVariable_.wait_for(lock, timeout, [this, &condition_function]{return condition_function(Value_);}))
		{
			if constexpr (std::same_as<decltype(function(Value_)), void>)
			{
				function(Value_);
				return true;
			}
			else
				return function(Value_);
		}
		else
		{
			if constexpr (std::same_as<decltype(function(Value_)), void>)
				return false;
			else
				return std::nullopt;
		}
	}
	template <decayed_type Value_t> template <typename Function_t, typename ConditionFuncion>
		auto Atomic_t<Value_t>::apply
		(Function_t&& function, ConditionFuncion&& condition_function, std::chrono::steady_clock::duration timeout)
		-> std::conditional_t
			<std::same_as<decltype(function(Value_)), void>, bool, std::optional<decltype(function(Value_))>>
		requires requires() {function(Value_);
		{condition_function(std::declval<const Value_t&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		if (ConditionVariable_.wait_for(lock, timeout, [this, &condition_function]
			{return condition_function(const_cast<const Value_t&>(Value_));}))
		{
			if constexpr (std::same_as<decltype(function(Value_)), void>)
			{
				function(Value_);
				ConditionVariable_.notify_all();
				return true;
			}
			else
			{
				auto&& result = function(Value_);
				ConditionVariable_.notify_all();
				return std::forward<decltype(function(Value_))>(result);
			}
		}
		else
		{
			if constexpr (std::same_as<decltype(function(Value_)), void>)
				return false;
			else
				return std::nullopt;
		}
	}

	template <decayed_type Value_t> template <typename ConditionFunction_t>
		void Atomic_t<Value_t>::wait(ConditionFunction_t&& condition_function) const
		requires requires() {{condition_function(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_function]{return condition_function(Value_);});
	}
	template <decayed_type Value_t> template <typename ConditionFunction_t> bool Atomic_t<Value_t>::wait
		(ConditionFunction_t&& condition_function, std::chrono::steady_clock::duration timeout) const
		requires requires() {{condition_function(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		return ConditionVariable_.wait_for(lock, timeout, [this, &condition_function]
			{return condition_function(Value_);});
	}

	template <decayed_type Value_t> template <bool Const>
		Atomic_t<Value_t>::Guard<Const>::Guard
		(
			std::unique_lock<std::recursive_mutex>&& lock,
			std::experimental::observer_ptr<std::conditional_t<Const, const Atomic_t<Value_t>, Atomic_t<Value_t>>> value
		)
		: Lock_{std::move(lock)}, Value_{value} {}
	template <decayed_type Value_t> template <bool Const> Atomic_t<Value_t>::Guard<Const>::Guard(Guard<Const>&& other)
		: Lock_{std::move(other.Lock_)}, Value_{other.Value_} {}
	template <decayed_type Value_t> template <bool Const> Atomic_t<Value_t>::Guard<Const>::~Guard()
		{Value_->ConditionVariable_.notify_all();}

	template <decayed_type Value_t> template <bool Const>
		std::conditional_t<Const, const Value_t&, Value_t&> Atomic_t<Value_t>::Guard<Const>::operator*() const
		{return Value_->Value_;}
	template <decayed_type Value_t> template <bool Const>
		std::conditional_t<Const, const Value_t*, Value_t*> Atomic_t<Value_t>::Guard<Const>::operator->() const
		{return &Value_->Value_;}
	template <decayed_type Value_t> template <bool Const>
		std::conditional_t<Const, const Value_t&, Value_t&> Atomic_t<Value_t>::Guard<Const>::value() const
		{return Value_->Value_;}

	template <decayed_type Value_t> Atomic_t<Value_t>::Guard<true> Atomic_t<Value_t>::lock() const
		{return {std::unique_lock{Mutex_}, std::experimental::make_observer(this)};}
	template <decayed_type Value_t> Atomic_t<Value_t>::Guard<false> Atomic_t<Value_t>::lock()
		{return {std::unique_lock{Mutex_}, std::experimental::make_observer(this)};}
	template <decayed_type Value_t> template <typename ConditionFunction_t>
		Atomic_t<Value_t>::Guard<true> Atomic_t<Value_t>::lock(ConditionFunction_t&& condition_function) const
		requires requires() {{condition_function(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_function]{return condition_function(Value_);});
		return {{std::move(lock), std::experimental::make_observer(this)}};
	}
	template <decayed_type Value_t> template <typename ConditionFunction_t>
		Atomic_t<Value_t>::Guard<false> Atomic_t<Value_t>::lock(ConditionFunction_t&& condition_function)
		requires requires() {{condition_function(std::declval<const Value_t&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_function]
			{return condition_function(const_cast<const Value_t&>(Value_));});
		return {{std::move(lock), std::experimental::make_observer(this)}};
	}
	template <decayed_type Value_t> template <typename ConditionFunction_t>
		std::optional<typename Atomic_t<Value_t>::Guard<true>> Atomic_t<Value_t>::lock
		(ConditionFunction_t&& condition_function, std::chrono::steady_clock::duration timeout) const
		requires requires() {{condition_function(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		if (ConditionVariable_.wait_for(lock, timeout, [this, &condition_function]{return condition_function(Value_);}))
			return {{std::move(lock), std::experimental::make_observer(this)}};
		else
			return std::nullopt;
	}
	template <decayed_type Value_t> template <typename ConditionFunction_t>
		std::optional<typename Atomic_t<Value_t>::Guard<false>>
		Atomic_t<Value_t>::lock(ConditionFunction_t&& condition_function, std::chrono::steady_clock::duration timeout)
		requires requires() {{condition_function(std::declval<const Value_t&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		if
		(
			ConditionVariable_.wait_for(lock, timeout, [this, &condition_function]
				{return condition_function(const_cast<const Value_t&>(Value_));})
		)
			return {{std::move(lock), std::experimental::make_observer(this)}};
		else
			return std::nullopt;
	}
}
