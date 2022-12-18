# pragma once
# include <mirism/detail_/misc/smartref.hpp>

namespace mirism
{
	template <decayed_type T> SmartRef<T>::SmartRef(T& val) : Ref_(val) {}
	template <decayed_type T> template <typename... Us> requires (std::is_constructible_v<T, Us...>)
		SmartRef<T>::SmartRef(Us&&... val)
		: Ptr_(std::make_unique<T>(std::forward<Us>(val)...)), Ref_(*Ptr_) {}
	template <decayed_type T> inline T& SmartRef<T>::operator*() {return Ref_;}
	template <decayed_type T> inline T* SmartRef<T>::operator->() {return &Ref_;}
}
