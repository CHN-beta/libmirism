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
	template <VariableString<64>... SupportedContentTypes> class Base : virtual public Base<>
		{public: virtual const std::set<std::string>& get_supported_content_types() const override;};

	// Constrct a content object from content_type string conviniently.
	// All default content class will inherit from Selector<>::Register<T>, make it could be remembered by Selector<>.
	// To add custom content_type support or override default, pass the class to Selector, like Selector<T1, T2>.
	// It is developer's responsibility to make sure no redundant content_type between default content classes and
	// between custom content classes.
	template <std::derived_from<Base<>>... Contents> class Selector;
	template<> class Selector<>
	{
		public: virtual ~Selector() = default;

		// check if content_type is supported or construct an object. Both is static virtual function.
		public: virtual bool exists(const std::string& content_type) const;
		public: virtual std::unique_ptr<Base<>> create(const std::string& content_type) const;

		protected: static std::unique_ptr<std::map<std::string, std::function<std::unique_ptr<Base<>>()>>>
			ContentConstructorsDefault_;
		protected: template <std::derived_from<Base<>> T> static void add_type
			(std::map<std::string, std::function<std::unique_ptr<Base<>>()>>&);

		public: template <std::derived_from<Base<>> T> class Register
			{protected: static bool Register_;};
	};
	template <std::derived_from<Base<>>... Contents>
		class Selector : virtual public Selector<>
	{
		public: bool exists(const std::string& content_type) const override;
		public: std::unique_ptr<Base<>> create(const std::string& content_type) const override;
		protected: static std::map<std::string, std::function<std::unique_ptr<Base<>>()>> ContentConstructors_;
	};
}
