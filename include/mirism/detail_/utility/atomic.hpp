# pragma once
# include <mutex>
# include <optional>
# include <condition_variable>
# include <cstddef>
# include <experimental/memory>
# include <mirism/detail_/utility/common.hpp>
# include <mirism/detail_/utility/concepts.hpp>
# include <mirism/detail_/utility/called_by.hpp>

namespace mirism
{
	// Thread safe wrapper of custom class
	template <DecayedType ValueType> class Atomic
	{
		protected: mutable std::recursive_mutex Mutex_;
		protected: mutable std::condition_variable_any ConditionVariable_;
		protected: ValueType Value_;

		public: class TimeoutException : public std::exception
		{
			protected: std::string Message_;
			public: TimeoutException(std::string message);
			public: const char* what() const noexcept override;
		};

		public: Atomic() = default;
		public: Atomic(const ValueType& value) : Value_{value} {}
		public: Atomic(ValueType&& value) : Value_{std::move(value)} {}
		public: Atomic(const Atomic<ValueType>& other) : Value_{other.get()} {}
		public: Atomic(Atomic<ValueType>&& other) : Value_{std::move(other).get()} {}

		public: Atomic<ValueType>& operator=(auto&& value);

		// Return a copy (or move) of stored value.
		public: ValueType get() const& {std::scoped_lock lock{Mutex_}; return Value_;}
		public: ValueType get() && {std::scoped_lock lock{Mutex_}; return std::move(Value_);}
		public: operator ValueType() const& {return get();}
		public: operator ValueType() && {return std::move(*this).get();}

		// Apply a function to stored value.
		// Wait for some time (if provided) until condition funciton returns true (if provided)
		// before applying the function.
		protected: template
			<
				bool ReturnFunctionResult, bool Nothrow = false,
				typename ConditionFunction = std::nullptr_t, typename Duration = std::nullptr_t
			>
			static auto apply_
			(
				auto&& atomic, auto&& function,
				ConditionFunction&& condition_function = nullptr, Duration timeout = nullptr
			)
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
			);
		public: template <bool ReturnFunctionResult = false> decltype(auto) apply(auto&& function) const&
			{return apply_<ReturnFunctionResult>(*this, std::forward<decltype(function)>(function));}
		public: template <bool ReturnFunctionResult = false> decltype(auto) apply(auto&& function) &
			{return apply_<ReturnFunctionResult>(*this, std::forward<decltype(function)>(function));}
		public: template <bool ReturnFunctionResult = false> decltype(auto) apply(auto&& function) &&
			{return apply_<ReturnFunctionResult>(std::move(*this), std::forward<decltype(function)>(function));}
		public: template <bool ReturnFunctionResult = false>
			decltype(auto) apply(auto&& function, auto&& condition_function) const&
		{
			return apply_<ReturnFunctionResult>
			(
				*this, std::forward<decltype(function)>(function),
				std::forward<decltype(condition_function)>(condition_function)
			);
		}
		public: template <bool ReturnFunctionResult = false>
			decltype(auto) apply(auto&& function, auto&& condition_function) &
		{
			return apply_<ReturnFunctionResult>
			(
				*this, std::forward<decltype(function)>(function),
				std::forward<decltype(condition_function)>(condition_function)
			);
		}
		public: template <bool ReturnFunctionResult = false>
			decltype(auto) apply(auto&& function, auto&& condition_function) &&
		{
			return apply_<ReturnFunctionResult>
			(
				std::move(*this), std::forward<decltype(function)>(function),
				std::forward<decltype(condition_function)>(condition_function)
			);
		}
		public: template <bool ReturnFunctionResult = false, bool Nothrow = false>
			decltype(auto) apply(auto&& function, auto&& condition_function, auto timeout) const&
		{
			return apply_<ReturnFunctionResult, Nothrow>
			(
				*this, std::forward<decltype(function)>(function),
				std::forward<decltype(condition_function)>(condition_function), timeout
			);
		}
		public: template <bool ReturnFunctionResult = false, bool Nothrow = false>
			decltype(auto) apply(auto&& function, auto&& condition_function, auto timeout) &
		{
			return apply_<ReturnFunctionResult, Nothrow>
			(
				*this, std::forward<decltype(function)>(function),
				std::forward<decltype(condition_function)>(condition_function), timeout
			);
		}
		public: template <bool ReturnFunctionResult = false, bool Nothrow = false>
			decltype(auto) apply(auto&& function, auto&& condition_function, auto timeout) &&
		{
			return apply_<ReturnFunctionResult, Nothrow>
			(
				std::move(*this), std::forward<decltype(function)>(function),
				std::forward<decltype(condition_function)>(condition_function), timeout
			);
		}

		// Wait until condition funciton returns true, with an optional timeout
		protected: template <bool Nothrow = false, typename Duration = std::nullptr_t> static auto wait_
			(auto&& atomic, auto&& condition_function, Duration timeout = nullptr)
			-> std::conditional_t<Nothrow && !std::is_null_pointer_v<Duration>, bool, decltype(atomic)&&>
			requires (InvocableWithResult<decltype(condition_function), bool, const ValueType&>
				&& (std::is_null_pointer_v<Duration> || SpecializationOf<Duration, std::chrono::duration>));
		public: decltype(auto) wait(auto&& condition_function) const&
			{return wait_(*this, std::forward<decltype(condition_function)>(condition_function));}
		public: decltype(auto) wait(auto&& condition_function) &
			{return wait_(*this, std::forward<decltype(condition_function)>(condition_function));}
		public: decltype(auto) wait(auto&& condition_function) &&
			{return wait_(std::move(*this), std::forward<decltype(condition_function)>(condition_function));}
		public: template <bool Nothrow = false> decltype(auto) wait(auto&& condition_function, auto timeout) const&
			{return wait_<Nothrow>(*this, std::forward<decltype(condition_function)>(condition_function), timeout);}
		public: template <bool Nothrow = false> decltype(auto) wait(auto&& condition_function, auto timeout) &
			{return wait_<Nothrow>(*this, std::forward<decltype(condition_function)>(condition_function), timeout);}
		public: template <bool Nothrow = false> decltype(auto) wait(auto&& condition_function, auto timeout) &&
		{
			return wait_<Nothrow>
				(std::move(*this), std::forward<decltype(condition_function)>(condition_function), timeout);
		}

		// Attain lock from outside when constructing, and release when destructing.
		// For non-const variant, When destructing, ConditionVariable_.notify_all() is called.
		public: template <bool Const> class Guard
		{
			protected: std::unique_lock<std::recursive_mutex> Lock_;
			protected: std::experimental::observer_ptr
				<std::conditional_t<Const, const Atomic<ValueType>, Atomic<ValueType>>> Value_;

			public: Guard(const Guard<Const>& other) = delete;
			public: template <bool OtherConst> Guard(Guard<OtherConst>&& other) requires (Const || !OtherConst)
				: Lock_{std::move(other.Lock_)}, Value_{other.Value_} {}
			public: Guard(decltype(Lock_)&& lock, decltype(Value_) value, CalledBy<Atomic<ValueType>>)
				: Lock_{std::move(lock)}, Value_{value} {}
			public: ~Guard() {Value_->ConditionVariable_.notify_all();}

			public: std::conditional_t<Const, const ValueType&, ValueType&> operator*() const&
				{return Value_->Value_;}
			public: std::conditional_t<Const, const ValueType*, ValueType*> operator->() const&
				{return &Value_->Value_;}
			public: std::conditional_t<Const, const ValueType&, ValueType&> value() const&
				{return Value_->Value_;}
			public: auto operator*() const&& = delete;
			public: auto operator->() const&& = delete;
			public: auto value() const&& = delete;
		};
		protected: template <bool Nothrow = false,
				typename ConditionFunction = std::nullptr_t, typename Duration = std::nullptr_t> static
			auto lock_(auto&& atomic, ConditionFunction&& condition_function = nullptr, Duration timeout = nullptr)
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
			));
		public: auto lock() const& {return lock_(*this);}
		public: auto lock() & {return lock_(*this);}
		public: auto lock(auto&& condition_function) const& {return lock_(*this, condition_function);}
		public: auto lock(auto&& condition_function) & {return lock_(*this, condition_function);}
		public: template <bool Nothrow = false> auto lock(auto&& condition_function, auto timeout) const&
			{return lock_<Nothrow>(*this, condition_function, timeout);}
		public: template <bool Nothrow = false> auto lock(auto&& condition_function, auto timeout) &
			{return lock_<Nothrow>(*this, condition_function, timeout);}
		public: auto lock() const&& = delete;
		public: auto lock(auto&& condition_function) const&& = delete;
		public: template <bool Nothrow = false> auto lock(auto&& condition_function, auto timeout) const&& = delete;
	};
}
