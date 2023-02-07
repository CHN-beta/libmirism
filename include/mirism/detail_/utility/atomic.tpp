# pragma once
# include <mirism/detail_/utility/atomic.hpp>

namespace mirism
{
	template <DecayedType ValueType> Atomic<ValueType>& Atomic<ValueType>::operator=(auto&& other)
	{
		std::scoped_lock lock{Mutex_};
		Value_ = std::forward<decltype(other)>(other);
		ConditionVariable_.notify_all();
		return *this;
	}

	template <DecayedType ValueType>
		template<bool ReturnFunctionResult, bool Nothrow, typename ConditionFunction, typename Duration>
		auto Atomic<ValueType>::apply_
			(auto&& atomic, auto&& function, ConditionFunction&& condition_function, Duration timeout)
		-> std::conditional_t
		<
			Nothrow,
			std::conditional_t
			<
				ReturnFunctionResult && !std::is_void_v<std::invoke_result<decltype(function), ValueType>>,
				std::optional<std::remove_cvref_t<FallbackIfNoTypeDeclaredType<std::invoke_result
					<decltype(function), MoveQualifiersType<decltype(atomic), ValueType>, int>>>>,
				bool
			>,
			std::conditional_t
			<
				ReturnFunctionResult,
				std::invoke_result_t<decltype(function), MoveQualifiersType<decltype(atomic), ValueType>>,
				decltype(atomic)&&
			>
		>
		requires
		(
			std::invocable<decltype(function), MoveQualifiersType<decltype(atomic), ValueType>>,
			std::is_null_pointer_v<ConditionFunction> ||
			(
				InvocableWithResult<ConditionFunction, bool, const ValueType&>
					&& (std::is_null_pointer_v<Duration> || SpecializationOf<Duration, std::chrono::duration>)
			)
		)
	{
		std::unique_lock lock{atomic.Mutex_};

		// try to meet the condition
		if constexpr (!std::is_null_pointer_v<ConditionFunction>)
		{
			if constexpr (std::is_null_pointer_v<Duration>) atomic.ConditionVariable_.wait(lock, [&]
				{return std::forward<ConditionFunction>(condition_function)(std::as_const(atomic.Value_));});
			else if (!atomic.ConditionVariable_.wait_for(lock, timeout, [&]
				{return std::forward<ConditionFunction>(condition_function)(std::as_const(atomic.Value_));}))
			{
				if constexpr (Nothrow)
				{
					if constexpr
						(ReturnFunctionResult && !std::is_void_v<std::invoke_result_t<decltype(function), ValueType>>)
						return std::nullopt;
					else return false;
				}
				// TODO: use logger to throw
				else throw TimeoutException{};
			}
		}

		// apply the function and return
		if constexpr (ReturnFunctionResult && !std::is_void_v<std::invoke_result_t<decltype(function), ValueType>>)
		{
			auto&& result = std::forward<decltype(function)>(function)
				(static_cast<MoveQualifiersType<decltype(atomic), ValueType>&&>(atomic.Value_));
			if constexpr (!std::is_const_v<decltype(atomic)>) atomic.ConditionVariable_.notify_all();
			return std::forward<decltype(result)>(result);
		}
		else
		{
			std::forward<decltype(function)>(function)
				(static_cast<MoveQualifiersType<decltype(atomic), ValueType>&&>(atomic.Value_));
			if constexpr (!std::is_const_v<decltype(atomic)>) atomic.ConditionVariable_.notify_all();
			if constexpr (ReturnFunctionResult && std::is_void_v<std::invoke_result_t<decltype(function), ValueType>>)
				return;
			else return std::forward<decltype(atomic)>(atomic);
		}
	}

	template <DecayedType ValueType> template <bool Nothrow, typename Duration> auto Atomic<ValueType>::wait_
		(auto&& atomic, auto&& condition_function, Duration timeout)
		-> std::conditional_t<Nothrow && !std::is_null_pointer_v<Duration>, bool, decltype(atomic)&&>
		requires (InvocableWithResult<decltype(condition_function), bool, const ValueType&>
			&& (std::is_null_pointer_v<Duration> || SpecializationOf<Duration, std::chrono::duration>))
	{
		std::unique_lock lock{atomic.Mutex_};

		if constexpr (std::is_null_pointer_v<Duration>)
		{
			atomic.ConditionVariable_.wait(lock, [&]
				{return std::forward<decltype(condition_function)>(condition_function)(std::as_const(atomic.Value_));});
			return std::forward<decltype(atomic)>(atomic);
		}
		else
		{
			if (!atomic.ConditionVariable_.wait_for(lock, timeout, [&]
				{return std::forward<decltype(condition_function)>(condition_function)(std::as_const(atomic.Value_));}))
			{
				if constexpr (Nothrow) return false;
				// TODO: use logger to throw
				else throw TimeoutException{};
			}
			else
			{
				if constexpr (Nothrow) return true;
				else return std::forward<decltype(atomic)>(atomic);
			}
		}
	}

	template <DecayedType ValueType> template <bool Nothrow, typename ConditionFunction, typename Duration>
		auto Atomic<ValueType>::lock_(auto&& atomic, ConditionFunction&& condition_function, Duration timeout)
		-> std::conditional_t
		<
			Nothrow && !std::is_null_pointer_v<Duration>,
			std::optional<std::conditional_t<std::is_const_v<decltype(atomic)>, Guard<true>, Guard<false>>>,
			std::conditional_t<std::is_const_v<decltype(atomic)>, Guard<true>, Guard<false>>
		>
		requires (std::is_null_pointer_v<ConditionFunction> ||
		(
			InvocableWithResult<ConditionFunction, bool, const ValueType&>
				&& (std::is_null_pointer_v<Duration> || SpecializationOf<Duration, std::chrono::duration>)
		))
	{
		if constexpr (std::is_null_pointer_v<ConditionFunction>)
			return {std::unique_lock{atomic.Mutex_}, std::experimental::make_observer(&atomic), {}};
		else if constexpr (std::is_null_pointer_v<Duration>)
		{
			std::unique_lock lock{atomic.Mutex_};
			atomic.ConditionVariable_.wait(lock, [&]
				{return std::forward<ConditionFunction>(condition_function)(std::as_const(atomic.Value_));});
			return {std::move(lock), std::experimental::make_observer(&atomic), {}};
		}
		else
		{
			std::unique_lock lock{atomic.Mutex_};
			if (!atomic.ConditionVariable_.wait_for(lock, timeout, [&]
				{return std::forward<ConditionFunction>(condition_function)(std::as_const(atomic.Value_));}))
			{
				if constexpr (Nothrow) return std::nullopt;
				// TODO: use logger to throw
				else throw TimeoutException{};
			}
			else
				return {{std::move(lock), std::experimental::make_observer(&atomic), {}}};
		}
	}
}
