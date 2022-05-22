# pragma once

# include <mirism/common.hpp>

namespace mirism
{
	// Thread safe wrapper of custom class
	template <not_cvref_type T> class Atomic
	{
		protected: mutable std::recursive_mutex Mutex_;
		protected: mutable std::condition_variable_any ConditionVariable_;
		protected: T Value_;

		public: Atomic() = default;
		public: Atomic(const T& value);
		public: Atomic(T&& value);
		public: Atomic(const Atomic<T>& other);
		public: Atomic(Atomic<T>&& other);

		// after assignment, ConditionVariable_.notify_all() is called
		public: Atomic<T>& operator=(const T& value);
		public: Atomic<T>& operator=(T&& value);
		public: Atomic<T>& operator=(const Atomic<T>& other);
		public: Atomic<T>& operator=(Atomic<T>&& other);

		// Return a copy (or move) of stored value.
		public: T get() const&;
		public: T get() &&;
		public: operator T() const&;
		public: operator T() &&;

		// Apply a function to stored value.
		// At the end of non-const overload, ConditionVariable_.notify_all() is called.
		public: template <typename F> auto apply(F&& f) const -> decltype(f(Value_)) requires requires() {f(Value_);};
		public: template <typename F> auto apply(F&& f) -> decltype(f(Value_)) requires requires() {f(Value_);};

		// Wait until condition funciton returns true, and apply a function to stored value.
		// At the end of non-const overload, ConditionVariable_.notify_all() is called.
		public: template <typename F, typename ConditionF> auto apply(F&& f, ConditionF&& condition_f) const
			-> decltype(f(Value_)) requires requires(){f(Value_); {condition_f(Value_)} -> convertible_to<bool>;};
		public: template <typename F, typename ConditionF> auto apply(F&& f, ConditionF&& condition_f)
			-> decltype(f(Value_))
			requires requires(){f(Value_); {condition_f(std::declval<const T&>())} -> convertible_to<bool>;};

		// Wait for some time, until condition funciton returns true and apply a function to stored value and return the
		// result (with std::optional wrapped); if timeout, return false or std::nullopt.
		// At the end of non-const overload, if not timeout, ConditionVariable_.notify_all() is called.
		public: template <typename F, typename ConditionF>
			auto apply(F&& f, ConditionF&& condition_f, std::chrono::steady_clock::duration timeout) const
			-> std::conditional_t<std::same_as<decltype(f(Value_)), void>, bool, std::optional<decltype(f(Value_))>>
			requires requires(){f(Value_); {condition_f(Value_)} -> convertible_to<bool>;};
		public: template <typename F, typename ConditionF>
			auto apply(F&& f, ConditionF&& condition_f, std::chrono::steady_clock::duration timeout)
			-> std::conditional_t<std::same_as<decltype(f(Value_)), void>, bool, std::optional<decltype(f(Value_))>>
			requires requires(){f(Value_); {condition_f(std::declval<const T&>())} -> convertible_to<bool>;};

		// Wait until condition funciton returns true, with an optional timeout
		public: enum class WaitResult {Success, Timeout};
		public: template <typename ConditionF> void wait(ConditionF&& condition_f) const
			requires requires(){{condition_f(Value_)} -> convertible_to<bool>;};
		public: template <typename ConditionF>
			WaitResult wait(ConditionF&& condition_f, std::chrono::steady_clock::duration timeout) const
			requires requires(){{condition_f(Value_)} -> convertible_to<bool>;};

		// Attain lock from outside when constructing, and release when destructing.
		// For non-const variant, When destructing, ConditionVariable_.notify_all() is called.
		public: template <bool Const> class Guard
		{
			friend class Atomic<T>;
			protected: std::unique_lock<std::recursive_mutex> Lock_;
			protected: std::experimental::observer_ptr<std::conditional_t<Const, const Atomic<T>, Atomic<T>>>
				Value_;

			public: Guard(const Guard<Const>& other) = delete;
			public: Guard(Guard<Const>&& other);
			protected: Guard
			(
				std::unique_lock<std::recursive_mutex>&& lock,
				std::experimental::observer_ptr<std::conditional_t<Const, const Atomic<T>, Atomic<T>>> value
			);
			public: ~Guard();

			public: std::conditional_t<Const, const T&, T&> operator*() const;
			public: std::conditional_t<Const, const T*, T*> operator->() const;
			public: std::conditional_t<Const, const T&, T&> value() const;
		};

		public: Guard<true> lock() const;
		public: Guard<false> lock();
		public: template <typename ConditionF> Guard<true> lock(ConditionF&& condition_f) const
			requires requires(){{condition_f(Value_)} -> convertible_to<bool>;};
		public: template <typename ConditionF> Guard<false> lock(ConditionF&& condition_f)
			requires requires(){{condition_f(std::declval<const T&>())} -> convertible_to<bool>;};
		public: template <typename ConditionF>
			std::optional<Guard<true>> lock(ConditionF&& condition_f, std::chrono::steady_clock::duration timeout) const
			requires requires(){{condition_f(Value_)} -> convertible_to<bool>;};
		public: template <typename ConditionF>
			std::optional<Guard<false>> lock(ConditionF&& condition_f, std::chrono::steady_clock::duration timeout)
			requires requires(){{condition_f(std::declval<const T&>())} -> convertible_to<bool>;};
	};
}
