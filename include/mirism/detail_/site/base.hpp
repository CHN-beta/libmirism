# pragma once
# include <mirism/detail_/handler/mirror/base.hpp>

namespace mirism::site
{
	class Base
	{
		public: virtual ~Base() = default;

		public: get_handled_domains() const = 0;
	}
}
