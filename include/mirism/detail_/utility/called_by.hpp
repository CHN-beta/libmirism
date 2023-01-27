# pragma once

namespace mirism
{
	template <typename ClassAllowed> class ClassTemplateCalledBy
	{
		protected: ClassTemplateCalledBy() = default;
		friend ClassAllowed;
	};
}
