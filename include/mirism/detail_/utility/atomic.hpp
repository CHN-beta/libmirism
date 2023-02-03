# pragma once
# include <mutex>
# include <optional>
# include <condition_variable>
# include <experimental/memory>
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
		public: template <bool ReturnFunctionResult = false> auto apply(auto&& function) const&
			{return apply_<ReturnFunctionResult>(*this, std::forward<Function>(function));}
		public: template <bool ReturnFunctionResult = false> auto apply(auto&& function) &
			{return apply_<ReturnFunctionResult>(*this, std::forward<Function>(function));}
		public: template <bool ReturnFunctionResult = false> auto apply(auto&& function) &&
			{return apply_<ReturnFunctionResult>(std::move(*this), std::forward<Function>(function));}
		public: template <bool ReturnFunctionResult = false>
			auto apply(auto&& function, auto&& condition_function) const&
		{
			return apply_<ReturnFunctionResult>
				(*this, std::forward<Function>(function), std::forward<ConditionFunction>(condition_function));
		}
		public: template <bool ReturnFunctionResult = false>
			auto apply(auto&& function, auto&& condition_function) &
		{
			return apply_<ReturnFunctionResult>
				(*this, std::forward<Function>(function), std::forward<ConditionFunction>(condition_function));
		}
		public: template <bool ReturnFunctionResult = false>
			auto apply(auto&& function, auto&& condition_function) &&
		{
			return apply_<ReturnFunctionResult>
			(
				std::move(*this), std::forward<Function>(function),
				std::forward<ConditionFunction>(condition_function)
			);
		}
		public: template <bool ReturnFunctionResult = false>
			auto apply(auto&& function, auto&& condition_function, auto timeout) const&
		{
			return apply_<ReturnFunctionResult>
			(
				*this, std::forward<Function>(function),
				std::forward<ConditionFunction>(condition_function), timeout
			);
		}
		public: template <bool ReturnFunctionResult = false>
			auto apply(auto&& function, auto&& condition_function, auto timeout) &
		{
			return apply_<ReturnFunctionResult>
			(
				*this, std::forward<Function>(function),
				std::forward<ConditionFunction>(condition_function), timeout
			);
		}
		public: template <bool ReturnFunctionResult = false>
			auto apply(auto&& function, auto&& condition_function, auto timeout) &&
		{
			return apply_<ReturnFunctionResult>
			(
				std::move(*this), std::forward<Function>(function),
				std::forward<ConditionFunction>(condition_function), timeout
			);
		}

		// All apply() functions are implemented in this function.
		// At the end of non-const overload, ConditionVariable_.notify_all() is called.
		// ConditionFunction might be nullptr
		protected: template <bool ReturnFunctionResult> static auto apply_
				(auto&& atomic, auto&& function, auto&& condition_function = nullptr, auto timeout = nullptr)
			-> std::conditional_t
			<
				ReturnFunctionResult,
				std::invoke_result_t<decltype(function), MoveQualifiersType<decltype(atomic), ValueType>>,
				decltype(atomic)&&
			>
			requires
			(
				std::invocable<decltype(function), MoveQualifiersType<decltype(atomic), ValueType>>,
				std::same_as<decltype(condition_function), nullptr_t> ||
				(
					std::InvocableWithResult<decltype(condition_function), bool, const ValueType&> &&
					(
						std::same_as<decltype(timeout), nullptr_t>
						|| std::specialization_of<decltype(timeout), std::chrono::duration>
					)
				)
			);

		public: template <bool ReturnFunctionResult = false, std::invocable<const ValueType&> Function>
			std::conditional_t
				<ReturnFunctionResult, std::invoke_result_t<Function, const ValueType&>, const Atomic<ValueType>&>
			apply(Function&& function) const&;
		public: template <bool ReturnFunctionResult = false, std::invocable<ValueType&> Function>
			std::conditional_t
				<ReturnFunctionResult, std::invoke_result_t<Function, ValueType&>, Atomic<ValueType>&>;
			apply(Function&& function) &;
		public: template <bool ReturnFunctionResult = false, std::invocable<ValueType&&> Function>
			std::conditional_t
				<ReturnFunctionResult, std::invoke_result_t<Function, ValueType&&>, Atomic<ValueType>&&>;
			apply(Function&& function) &&;

		// Wait until condition funciton returns true, and apply a function to stored value.
		public: template
			<
				bool ReturnFunctionResult = false,
				std::invocable<const ValueType&> Function,
				InvocableWithResult<bool, const ValueType&> ConditionFunction
			> std::conditional_t
				<ReturnFunctionResult, std::invoke_result_t<Function, const ValueType&>, const Atomic<ValueType>&>
			apply(Function&& function, ConditionFunction&& condition_function) const&;
		public: template
			<
				bool ReturnFunctionResult = false,
				std::invocable<ValueType&> Function,
				InvocableWithResult<bool, const ValueType&> ConditionFunction
			> std::conditional_t
				<ReturnFunctionResult, std::invoke_result_t<Function, ValueType&>, Atomic<ValueType>&>
			apply(Function&& function, ConditionFunction&& condition_function) &;
		public: template
			<
				bool ReturnFunctionResult = false,
				std::invocable<ValueType&&> Function,
				InvocableWithResult<bool, const ValueType&> ConditionFunction
			> std::conditional_t
				<ReturnFunctionResult, std::invoke_result_t<Function, ValueType&&>, Atomic<ValueType>&&>
			apply(Function&& function, ConditionFunction&& condition_function) &&;

		// Wait for some time, until condition funciton returns true and apply a function to stored value.
		public: template
			<
				bool ReturnFunctionResult = false,
				std::invocable<const ValueType&> Function,
				InvocableWithResult<bool, const ValueType&> ConditionFunction,
				SpecializationOf<std::chrono::duration> Duration
			> std::conditional_t
				<ReturnFunctionResult, std::invoke_result_t<Function, const ValueType&>, const Atomic<ValueType>&>
			apply(Function&& function, ConditionFunction&& condition_function, Duration timeout) const&;
		public: template
			<
				bool ReturnFunctionResult = false,
				std::invocable<ValueType&> Function,
				InvocableWithResult<bool, const ValueType&> ConditionFunction,
				SpecializationOf<std::chrono::duration> Duration
			> std::conditional_t
				<ReturnFunctionResult, std::invoke_result_t<Function, ValueType&>, Atomic<ValueType>&>
			apply(Function&& function, ConditionFunction&& condition_function, Duration timeout) &;
		public: template
			<
				bool ReturnFunctionResult = false,
				std::invocable<ValueType&&> Function,
				InvocableWithResult<bool, const ValueType&> ConditionFunction,
				SpecializationOf<std::chrono::duration> Duration
			> std::conditional_t
				<ReturnFunctionResult, std::invoke_result_t<Function, ValueType&&>, Atomic<ValueType>&&>
			apply(Function&& function, ConditionFunction&& condition_function, Duration timeout) &&;



		// Wait until condition funciton returns true, with an optional timeout
		public: template <InvocableWithResult<bool, const ValueType&> ConditionFunction>
			const Atomic<ValueType>& wait(ConditionFunction&& condition_function) const&;
		public: template <InvocableWithResult<bool, const ValueType&> ConditionFunction>
			Atomic<ValueType>& wait(ConditionFunction&& condition_function) &;
		public: template <InvocableWithResult<bool, const ValueType&> ConditionFunction>
			Atomic<ValueType>&& wait(ConditionFunction&& condition_function) &&;
		public: template <bool Nothrow = false, InvocableWithResult<bool, const ValueType&> ConditionFunction>
			std::conditional_t<NoThrow, bool, const Atomic<ValueType>&>
			wait(ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout) const&;
		public: template <bool Nothrow = false, InvocableWithResult<bool, const ValueType&> ConditionFunction>
			std::conditional_t<NoThrow, bool, Atomic<ValueType>&>
			wait(ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout) &;
		public: template <bool Nothrow = false, InvocableWithResult<bool, const ValueType&> ConditionFunction>
			std::conditional_t<NoThrow, bool, Atomic<ValueType>&&>
			wait(ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout) &&;
		protected: template <typename AtomicType, typename ConditionFunction, bool UseTimeout>
			static bool wait_
			(
				AtomicType&& atomic, ConditionFunction&& condition_function,
				std::conditional_t<UseTimeout, std::chrono::steady_clock::duration, std::nullopt_t> timeout
			);

		// Attain lock from outside when constructing, and release when destructing.
		// For non-const variant, When destructing, ConditionVariable_.notify_all() is called.
		public: template <bool Const> class Guard
		{
			protected: std::unique_lock<std::recursive_mutex> Lock_;
			protected: std::experimental::observer_ptr
				<std::conditional_t<Const, const Atomic<ValueType>, Atomic<ValueType>>> Value_;

			public: Guard(const Guard<Const>& other) = delete;
			public: template <bool OtherConst> Guard(Guard<OtherConst>&& other) requires (Const || !OtherConst);
			public: Guard
				(std::unique_lock<std::recursive_mutex>&& lock, decltype(Value_) value, CalledBy<Atomic<ValueType>>);
			public: ~Guard();

			public: std::conditional_t<Const, const ValueType&, ValueType&> operator*() const;
			public: std::conditional_t<Const, const ValueType*, ValueType*> operator->() const;
			public: std::conditional_t<Const, const ValueType&, ValueType&> value() const;
		};
		public: Guard<true> lock() const;
		public: Guard<false> lock();
		public: template <InvocableWithResult<bool, const ValueType&> ConditionFunction>
			Guard<true> lock(ConditionFunction&& condition_function) const;
		public: template <InvocableWithResult<bool, const ValueType&> ConditionFunction>
			Guard<false> lock(ConditionFunction&& condition_function);
		public: template <bool Nothrow = false, InvocableWithResult<bool, const ValueType&> ConditionFunction>
			std::conditional_t<Nothrow, std::optional<Guard<true>>, Guard<true>>
			lock(ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout) const;
		public: template <bool Nothrow = false, InvocableWithResult<bool, const ValueType&> ConditionFunction>
			std::conditional_t<Nothrow, std::optional<Guard<false>>, Guard<false>>
			lock(ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout);
		public: template <bool Nothrow = false, typename ConditionFunction>
			auto lock(ConditionFunction&&, std::chrono::steady_clock::duration) const&& = delete;
		protected: template <typename AtomicType, typename ConditionFunction, bool UseTimeout> static
			std::optional<Guard<std::is_const_v<AtomicType>>> lock_
			(
				AtomicType&& atomic, ConditionFunction&& condition_function,
				std::conditional_t<UseTimeout, std::chrono::steady_clock::duration, std::nullopt_t> timeout
			);
	};
}
