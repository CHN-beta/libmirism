# pragma once
# include <mirism/detail_/utility/atomic.hpp>

namespace mirism
{
	template <DecayedType ValueType> Atomic<ValueType>::Atomic(const ValueType& value) : Value_{value} {}
	template <DecayedType ValueType> Atomic<ValueType>::Atomic(ValueType&& value) : Value_{std::move(value)} {}
	template <DecayedType ValueType> Atomic<ValueType>::Atomic(const Atomic& other) : Value_{other.get()} {}
	template <DecayedType ValueType> Atomic<ValueType>::Atomic(Atomic&& other) : Value_{std::move(other).get()} {}

	template <DecayedType ValueType> Atomic<ValueType>& Atomic<ValueType>::operator=(const ValueType& other)
	{
		std::scoped_lock lock{Mutex_};
		Value_ = other;
		ConditionVariable_.notify_all();
		return *this;
	}
	template <DecayedType ValueType> Atomic<ValueType>& Atomic<ValueType>::operator=(ValueType&& other)
	{
		std::scoped_lock lock{Mutex_};
		Value_ = std::move(other);
		ConditionVariable_.notify_all();
		return *this;
	}
	template <DecayedType ValueType> Atomic<ValueType>& Atomic<ValueType>::operator=(const Atomic& other)
	{
		std::scoped_lock lock{Mutex_, other.Mutex_};
		Value_ = other.Value_;
		ConditionVariable_.notify_all();
		return *this;
	}
	template <DecayedType ValueType> Atomic<ValueType>& Atomic<ValueType>::operator=(Atomic&& other)
	{
		std::scoped_lock lock{Mutex_, other.Mutex_};
		Value_ = std::move(other.Value_);
		ConditionVariable_.notify_all();
		return *this;
	}

	template <DecayedType ValueType> ValueType Atomic<ValueType>::get() const&
	{
		std::scoped_lock lock{Mutex_};
		return Value_;
	}
	template <DecayedType ValueType> ValueType Atomic<ValueType>::get() &&
	{
		std::scoped_lock lock{Mutex_};
		return std::move(Value_);
	}
	template <DecayedType ValueType> Atomic<ValueType>::operator ValueType() const& {return get();}
	template <DecayedType ValueType> Atomic<ValueType>::operator ValueType() && {return std::move(*this).get();}

	template <DecayedType ValueType> template <typename Function>
		auto Atomic<ValueType>::apply(Function&& function) const
		-> decltype(function(Value_)) requires requires() {function(Value_);}
	{
		std::scoped_lock lock{Mutex_};
		return function(Value_);
	}
	template <DecayedType ValueType> template <typename Function> auto Atomic<ValueType>::apply(Function&& function)
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

	template <DecayedType ValueType> template <typename Function, typename ConditionFunction>
		auto Atomic<ValueType>::apply(Function&& function, ConditionFunction&& condition_function) const
		-> decltype(function(Value_))
		requires requires() {function(Value_); {condition_function(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_function]{return condition_function(Value_);});
		return function(Value_);
	}
	template <DecayedType ValueType> template <typename Function, typename ConditionFunction>
		auto Atomic<ValueType>::apply(Function&& function, ConditionFunction&& condition_function)
		-> decltype(function(Value_))
		requires requires()
		{function(Value_); {condition_function(std::declval<const ValueType&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait
			(lock, [this, &condition_function]{return condition_function(const_cast<const ValueType&>(Value_));});
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

	template <DecayedType ValueType> template <typename Function, typename ConditionFunction>
		auto Atomic<ValueType>::apply
	(
		Function&& function, ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout
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
	template <DecayedType ValueType> template <typename Function, typename ConditionFuncion>
		auto Atomic<ValueType>::apply
		(Function&& function, ConditionFuncion&& condition_function, std::chrono::steady_clock::duration timeout)
		-> std::conditional_t
			<std::same_as<decltype(function(Value_)), void>, bool, std::optional<decltype(function(Value_))>>
		requires requires() {function(Value_);
		{condition_function(std::declval<const ValueType&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		if (ConditionVariable_.wait_for(lock, timeout, [this, &condition_function]
			{return condition_function(const_cast<const ValueType&>(Value_));}))
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

	template <DecayedType ValueType> template <typename ConditionFunction>
		void Atomic<ValueType>::wait(ConditionFunction&& condition_function) const
		requires requires() {{condition_function(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_function]{return condition_function(Value_);});
	}
	template <DecayedType ValueType> template <typename ConditionFunction> bool Atomic<ValueType>::wait
		(ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout) const
		requires requires() {{condition_function(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		return ConditionVariable_.wait_for(lock, timeout, [this, &condition_function]
			{return condition_function(Value_);});
	}

	template <DecayedType ValueType> template <bool Const>
		Atomic<ValueType>::Guard<Const>::Guard
		(
			std::unique_lock<std::recursive_mutex>&& lock,
			std::experimental::observer_ptr<std::conditional_t<Const, const Atomic<ValueType>, Atomic<ValueType>>> value
		)
		: Lock_{std::move(lock)}, Value_{value} {}
	template <DecayedType ValueType> template <bool Const> Atomic<ValueType>::Guard<Const>::Guard(Guard<Const>&& other)
		: Lock_{std::move(other.Lock_)}, Value_{other.Value_} {}
	template <DecayedType ValueType> template <bool Const> Atomic<ValueType>::Guard<Const>::~Guard()
		{Value_->ConditionVariable_.notify_all();}

	template <DecayedType ValueType> template <bool Const>
		std::conditional_t<Const, const ValueType&, ValueType&> Atomic<ValueType>::Guard<Const>::operator*() const
		{return Value_->Value_;}
	template <DecayedType ValueType> template <bool Const>
		std::conditional_t<Const, const ValueType*, ValueType*> Atomic<ValueType>::Guard<Const>::operator->() const
		{return &Value_->Value_;}
	template <DecayedType ValueType> template <bool Const>
		std::conditional_t<Const, const ValueType&, ValueType&> Atomic<ValueType>::Guard<Const>::value() const
		{return Value_->Value_;}

	template <DecayedType ValueType> Atomic<ValueType>::Guard<true> Atomic<ValueType>::lock() const
		{return {std::unique_lock{Mutex_}, std::experimental::make_observer(this)};}
	template <DecayedType ValueType> Atomic<ValueType>::Guard<false> Atomic<ValueType>::lock()
		{return {std::unique_lock{Mutex_}, std::experimental::make_observer(this)};}
	template <DecayedType ValueType> template <typename ConditionFunction>
		Atomic<ValueType>::Guard<true> Atomic<ValueType>::lock(ConditionFunction&& condition_function) const
		requires requires() {{condition_function(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_function]{return condition_function(Value_);});
		return {{std::move(lock), std::experimental::make_observer(this)}};
	}
	template <DecayedType ValueType> template <typename ConditionFunction>
		Atomic<ValueType>::Guard<false> Atomic<ValueType>::lock(ConditionFunction&& condition_function)
		requires requires() {{condition_function(std::declval<const ValueType&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		ConditionVariable_.wait(lock, [this, &condition_function]
			{return condition_function(const_cast<const ValueType&>(Value_));});
		return {{std::move(lock), std::experimental::make_observer(this)}};
	}
	template <DecayedType ValueType> template <typename ConditionFunction>
		std::optional<typename Atomic<ValueType>::Guard<true>> Atomic<ValueType>::lock
		(ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout) const
		requires requires() {{condition_function(Value_)} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		if (ConditionVariable_.wait_for(lock, timeout, [this, &condition_function]{return condition_function(Value_);}))
			return {{std::move(lock), std::experimental::make_observer(this)}};
		else
			return std::nullopt;
	}
	template <DecayedType ValueType> template <typename ConditionFunction>
		std::optional<typename Atomic<ValueType>::Guard<false>>
		Atomic<ValueType>::lock(ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout)
		requires requires() {{condition_function(std::declval<const ValueType&>())} -> convertible_to<bool>;}
	{
		std::unique_lock lock{Mutex_};
		if
		(
			ConditionVariable_.wait_for(lock, timeout, [this, &condition_function]
				{return condition_function(const_cast<const ValueType&>(Value_));})
		)
			return {{std::move(lock), std::experimental::make_observer(this)}};
		else
			return std::nullopt;
	}
}
