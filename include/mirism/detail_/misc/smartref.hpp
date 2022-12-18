# pragma once
# include <mirism/detail_/misc/concepts.hpp>

namespace mirism
{
	// store a reference to an lvalue, or a value (instead of a reference) copy or moved from an rvalue
	template <decayed_type T> class SmartRef
	{
		protected: std::unique_ptr<T> Ptr_;
		protected: T& Ref_;

		public: SmartRef(T& val);
		public: template <typename... Us> requires (std::is_constructible_v<T, Us...>) SmartRef(Us&&... val);
		public: T& operator*();
		public: T* operator->();
	};
}
