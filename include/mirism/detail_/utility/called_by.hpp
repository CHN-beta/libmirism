# pragma once

namespace mirism
{
	template <typename T> class CalledBy
	{
		protected: CalledBy() = default;
		friend T;
	};
}
