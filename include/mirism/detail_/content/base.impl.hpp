# pragma once
# include <mirism/detail_/content/base.hpp>

namespace mirism::content
{
	inline std::size_t Base<>::patch_register(std::move_only_function<void(std::string&)> patch)
	{
		Logger::Guard log;
		Patches_.emplace(NextPatchId_, std::move(patch));
		return log.rtn(NextPatchId_++);
	}
	inline Base<>& Base<>::patch_unregister(std::size_t id)
	{
		Logger::Guard log(id);
		Patches_.erase(id);
		return *this;
	}

	template <VariableString<64>... SupportedContentTypes> requires (sizeof...(SupportedContentTypes) > 0)
		inline const std::set<std::string>& Base<SupportedContentTypes...>::get_supported_content_types() const
	{
		static auto supported_content_types = [&]()
		{
			std::set<std::string> types;
			(types.emplace(SupportedContentTypes.Data, SupportedContentTypes.Size), ...);
			return types;
		}();
		return supported_content_types;
	}
}
