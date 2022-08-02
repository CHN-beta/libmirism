# pragma once
# include <mirism/detail_/misc/pipe.hpp>

namespace mirism::content
{
	// content is not thread safe.
	template <VariableString<64>... SupportedContentTypes> class Base;
	template<> class Base<>
	{
		protected: std::map<std::size_t, std::move_only_function<void(std::string&)>> Patches_;
		protected: std::size_t NextPatchId_ = 0;
		public: virtual ~Base() = default;

		// get supported content types. `this` should not be used in this function, that is, like static function.
		public: virtual const std::set<std::string>& get_supported_content_types() const = 0;

		// register a patch to the content. The string that the patch received may not be the whole data but chunks one
		// by one.
		// Patch should be registered before content read anything, otherwise some data may be missed.
		// Returned value is the id of the patch, it could be used to unregister the patch after.
		public: virtual std::size_t patch_register(std::move_only_function<void(std::string&)> patch);

		// unregister a patch.
		public: virtual Base<>& patch_unregister(std::size_t id);
	};
	template <VariableString<64>... SupportedContentTypes> requires (sizeof...(SupportedContentTypes) > 0)
		class Base<SupportedContentTypes...> : virtual public Base<>
		{public: virtual const std::set<std::string>& get_supported_content_types() const override;};
}
