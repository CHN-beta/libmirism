# pragma once

namespace mirism
{
	template <typename AllowedType> class CalledBy
	{
		protected: CalledBy() = default;
		friend AllowedType;
	};
}
