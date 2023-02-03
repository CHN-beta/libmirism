# pragma once
# include <mirism/detail_/handler/content/base.hpp>

namespace mirism::content
{
	template <VariableString<64>... ContentTypes> std::unordered_set<std::string>
		Base<ContentTypes...>::get_content_type_set() const
		{return {{ContentTypes.Data, ContentTypes.Size}...};}
}
