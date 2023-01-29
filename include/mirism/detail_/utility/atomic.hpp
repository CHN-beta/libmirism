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

		public: Atomic() = default;
		public: Atomic(const ValueType& value);
		public: Atomic(ValueType&& value);
		public: Atomic(const Atomic<ValueType>& other);
		public: Atomic(Atomic<ValueType>&& other);

		// after assignment, ConditionVariable_.notify_all() is called
		public: Atomic<ValueType>& operator=(const ValueType& value);
		public: Atomic<ValueType>& operator=(ValueType&& value);
		public: Atomic<ValueType>& operator=(const Atomic<ValueType>& other);
		public: Atomic<ValueType>& operator=(Atomic<ValueType>&& other);

		// Return a copy (or move) of stored value.
		public: ValueType get() const&;
		public: ValueType get() &&;
		public: operator ValueType() const&;
		public: operator ValueType() &&;

		// Apply a function to stored value.
		// At the end of non-const overload, ConditionVariable_.notify_all() is called.
		public: template <typename Function> auto apply(Function&& function) const -> decltype(function(Value_))
			requires requires() {function(Value_);};
		public: template <typename Function> auto apply(Function&& function) -> decltype(function(Value_))
			requires requires() {function(Value_);};

		// Wait until condition funciton returns true, and apply a function to stored value.
		// At the end of non-const overload, ConditionVariable_.notify_all() is called.
		public: template <typename Function, typename ConditionFunction>
			auto apply(Function&& function, ConditionFunction&& condition_function) const -> decltype(function(Value_))
			requires requires() {function(Value_); {condition_function(Value_)} -> ConvertibleTo<bool>;};
		public: template <typename Function, typename ConditionFunction>
			auto apply(Function&& function, ConditionFunction&& condition_function) -> decltype(function(Value_))
			requires requires()
			{function(Value_); {condition_function(std::declval<const ValueType&>())} -> ConvertibleTo<bool>;};

		// Wait for some time, until condition funciton returns true and apply a function to stored value and return the
		// result (with std::optional wrapped); if timeout, return false or std::nullopt.
		// At the end of non-const overload, if not timeout, ConditionVariable_.notify_all() is called.
		public: template <typename Function, typename ConditionFunction> auto apply
		(
			Function&& function, ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout
		) const
			-> std::conditional_t
				<std::same_as<decltype(function(Value_)), void>, bool, std::optional<decltype(function(Value_))>>
			requires requires() {function(Value_); {condition_function(Value_)} -> ConvertibleTo<bool>;};
		public: template <typename Function, typename ConditionFunction> auto apply
		(
			Function&& function, ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout
		)
			-> std::conditional_t
				<std::same_as<decltype(function(Value_)), void>, bool, std::optional<decltype(function(Value_))>>
			requires requires()
			{function(Value_); {condition_function(std::declval<const ValueType&>())} -> ConvertibleTo<bool>;};

		// Wait until condition funciton returns true, with an optional timeout
		public: template <typename ConditionFunction> void wait(ConditionFunction&& condition_function) const
			requires requires() {{condition_function(Value_)} -> ConvertibleTo<bool>;};
		public: template <typename ConditionFunction>
			bool wait(ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout) const
			requires requires() {{condition_function(Value_)} -> ConvertibleTo<bool>;};

		// Attain lock from outside when constructing, and release when destructing.
		// For non-const variant, When destructing, ConditionVariable_.notify_all() is called.
		public: template <bool Const> class Guard
		{
			protected: std::unique_lock<std::recursive_mutex> Lock_;
			protected: std::experimental::observer_ptr
				<std::conditional_t<Const, const Atomic<ValueType>, Atomic<ValueType>>> Value_;

			public: Guard(const Guard<Const>& other) = delete;
			public: Guard(Guard<Const>&& other);
			public: Guard
				(std::unique_lock<std::recursive_mutex>&& lock, decltype(Value_) value, CalledBy<Atomic<ValueType>>);
			public: ~Guard();

			public: std::conditional_t<Const, const ValueType&, ValueType&> operator*() const;
			public: std::conditional_t<Const, const ValueType*, ValueType*> operator->() const;
			public: std::conditional_t<Const, const ValueType&, ValueType&> value() const;
		};
		public: Guard<true> lock() const;
		public: Guard<false> lock();
		public: template <typename ConditionFunction> Guard<true> lock(ConditionFunction&& condition_function) const
			requires requires() {{condition_function(Value_)} -> ConvertibleTo<bool>;};
		public: template <typename ConditionFunction> Guard<false> lock(ConditionFunction&& condition_function)
			requires requires() {{condition_function(std::declval<const ValueType&>())} -> ConvertibleTo<bool>;};
		public: template <typename ConditionFunction> std::optional<Guard<true>> lock
			(ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout) const
			requires requires() {{condition_function(Value_)} -> ConvertibleTo<bool>;};
		public: template <typename ConditionFunction> std::optional<Guard<false>> lock
			(ConditionFunction&& condition_function, std::chrono::steady_clock::duration timeout)
			requires requires() {{condition_function(std::declval<const ValueType&>())} -> ConvertibleTo<bool>;};
	};
}
