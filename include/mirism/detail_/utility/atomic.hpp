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
	template <ConceptDecayed Class> class ClassTemplateAtomic
	{
		protected: mutable std::recursive_mutex Mutex_;
		protected: mutable std::condition_variable_any ConditionVariable_;
		protected: Class Value_;

		public: ClassTemplateAtomic() = default;
		public: ClassTemplateAtomic(const Class& value);
		public: ClassTemplateAtomic(Class&& value);
		public: ClassTemplateAtomic(const ClassTemplateAtomic<Class>& other);
		public: ClassTemplateAtomic(ClassTemplateAtomic<Class>&& other);

		// after assignment, ConditionVariable_.notify_all() is called
		public: Atomic_t<Value_t>& operator=(const Value_t& value);
		public: Atomic_t<Value_t>& operator=(Value_t&& value);
		public: Atomic_t<Value_t>& operator=(const Atomic_t<Value_t>& other);
		public: Atomic_t<Value_t>& operator=(Atomic_t<Value_t>&& other);

		// Return a copy (or move) of stored value.
		public: Value_t get() const&;
		public: Value_t get() &&;
		public: operator Value_t() const&;
		public: operator Value_t() &&;

		// Apply a function to stored value.
		// At the end of non-const overload, ConditionVariable_.notify_all() is called.
		public: template <typename Function_t> auto apply(Function_t&& function) const -> decltype(function(Value_))
			requires requires() {function(Value_);};
		public: template <typename Function_t> auto apply(Function_t&& function) -> decltype(function(Value_))
			requires requires() {function(Value_);};

		// Wait until condition funciton returns true, and apply a function to stored value.
		// At the end of non-const overload, ConditionVariable_.notify_all() is called.
		public: template <typename Function_t, typename ConditionFunction_t>
			auto apply(Function_t&& function, ConditionFunction_t&& condition_function) const
			-> decltype(function(Value_)) requires requires()
			{function(Value_); {condition_function(Value_)} -> convertible_to<bool>;};
		public: template <typename Function_t, typename ConditionFunction_t>
			auto apply(Function_t&& function, ConditionFunction_t&& condition_function)
			-> decltype(function(Value_)) requires requires()
			{function(Value_); {condition_function(std::declval<const Value_t&>())} -> convertible_to<bool>;};

		// Wait for some time, until condition funciton returns true and apply a function to stored value and return the
		// result (with std::optional wrapped); if timeout, return false or std::nullopt.
		// At the end of non-const overload, if not timeout, ConditionVariable_.notify_all() is called.
		public: template <typename Function_t, typename ConditionFunction_t> auto apply
		(
			Function_t&& function, ConditionFunction_t&& condition_function, std::chrono::steady_clock::duration timeout
		) const
			-> std::conditional_t
				<std::same_as<decltype(function(Value_)), void>, bool, std::optional<decltype(function(Value_))>>
			requires requires() {function(Value_); {condition_function(Value_)} -> convertible_to<bool>;};
		public: template <typename Function_t, typename ConditionFunction_t> auto apply
		(
			Function_t&& function, ConditionFunction_t&& condition_function, std::chrono::steady_clock::duration timeout
		)
			-> std::conditional_t
				<std::same_as<decltype(function(Value_)), void>, bool, std::optional<decltype(function(Value_))>>
			requires requires()
			{function(Value_); {condition_function(std::declval<const Value_t&>())} -> convertible_to<bool>;};

		// Wait until condition funciton returns true, with an optional timeout
		public: template <typename ConditionFunction_t> void wait(ConditionFunction_t&& condition_function) const
			requires requires() {{condition_function(Value_)} -> convertible_to<bool>;};
		public: template <typename ConditionFunction_t>
			bool wait(ConditionFunction_t&& condition_function, std::chrono::steady_clock::duration timeout) const
			requires requires() {{condition_function(Value_)} -> convertible_to<bool>;};

		// Attain lock from outside when constructing, and release when destructing.
		// For non-const variant, When destructing, ConditionVariable_.notify_all() is called.
		public: template <bool Const> class Guard_t
		{
			protected: std::unique_lock<std::recursive_mutex> Lock_;
			protected: std::experimental::observer_ptr
				<std::conditional_t<Const, const Atomic_t<Value_t>, Atomic_t<Value_t>>> Value_;

			public: Guard_t(const Guard_t<Const>& other) = delete;
			public: Guard_t(Guard_t<Const>&& other);
			public: Guard_t
				(std::unique_lock<std::recursive_mutex>&& lock, decltype(Value_) value, CalledBy<Atomic_t<Value_t>>);
			public: ~Guard();

			public: std::conditional_t<Const, const Value_t&, Value_t&> operator*() const;
			public: std::conditional_t<Const, const Value_t*, Value_t*> operator->() const;
			public: std::conditional_t<Const, const Value_t&, Value_t&> value() const;
		};
		public: Guard_t<true> lock() const;
		public: Guard_t<false> lock();
		public: template <typename ConditionFunction_t>
			Guard_t<true> lock(ConditionFunction_t&& condition_function) const
			requires requires() {{condition_function(Value_)} -> convertible_to<bool>;};
		public: template <typename ConditionFunction_t> Guard_t<false> lock(ConditionFunction_t&& condition_function)
			requires requires() {{condition_function(std::declval<const Value_t&>())} -> convertible_to<bool>;};
		public: template <typename ConditionFunction_t> std::optional<Guard_t<true>> lock
			(ConditionFunction_t&& condition_function, std::chrono::steady_clock::duration timeout) const
			requires requires() {{condition_function(Value_)} -> convertible_to<bool>;};
		public: template <typename ConditionFunction_t> std::optional<Guard_t<false>> lock
			(ConditionFunction_t&& condition_function, std::chrono::steady_clock::duration timeout)
			requires requires() {{condition_function(std::declval<const Value_t&>())} -> convertible_to<bool>;};
	};
}
