# pragma once
# include <mirism/detail_/content/base.hpp>
# include <mirism/detail_/misc/staticize.hpp>

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
		Logger::Guard log{id};
		Patches_.erase(id);
		return *this;
	}

	template <VariableString<64>... SupportedContentTypes>
		inline const std::set<std::string>& Base<SupportedContentTypes...>::get_supported_content_types() const
	{
		static auto supported_content_types = [&]
		{
			std::set<std::string> types;
			(types.emplace(SupportedContentTypes.Data, SupportedContentTypes.Size), ...);
			return types;
		}();
		return supported_content_types;
	}

	inline bool Selector<>::exists(const std::string& content_type) const
		{return ContentConstructorsDefault_ && ContentConstructorsDefault_->contains(content_type);}
	inline std::unique_ptr<Base<>> Selector<>::create(const std::string& content_type) const
	{
		if (!ContentConstructorsDefault_)
			return nullptr;
		if (auto it = ContentConstructorsDefault_->find(content_type); it != ContentConstructorsDefault_->end())
			return it->second();
		return nullptr;
	}

	inline constinit std::unique_ptr<std::map<std::string, std::function<std::unique_ptr<Base<>>()>>>
		Selector<>::ContentConstructorsDefault_;
	template <std::derived_from<Base<>> T> inline void Selector<>::add_type
		(std::map<std::string, std::function<std::unique_ptr<Base<>>()>>& content_constructors)
	{
		for (auto& content_type : staticize<&T::get_supported_content_types>()())
			content_constructors.emplace(content_type, []{return std::make_unique<T>();});
	}

	template <typename T> inline bool Selector<>::Register<T>::Register_ = []
	{
		if (!Selector<>::ContentConstructorsDefault_)
			Selector<>::ContentConstructorsDefault_
				= std::make_unique<std::map<std::string, std::function<std::unique_ptr<Base<>>()>>>();
		add_type<T>(*Selector<>::ContentConstructorsDefault_);
		return true;
	}();

	template <std::derived_from<Base<>>... Contents> requires (sizeof...(Contents) > 0)
		inline bool Selector<Contents...>::exists(const std::string& content_type) const
		{return ContentConstructors_.contains(content_type) || Selector<>::exists(content_type);}
	template <std::derived_from<Base<>>... Contents> requires (sizeof...(Contents) > 0)
		inline std::unique_ptr<Base<>> Selector<Contents...>::create(const std::string& content_type) const
	{
		if (auto it = ContentConstructors_.find(content_type); it != ContentConstructors_.end())
			return it->second();
		return Selector<>::create(content_type);
	}
	template <std::derived_from<Base<>>... Contents> requires (sizeof...(Contents) > 0)
		inline std::map<std::string, std::function<std::unique_ptr<Base<>>()>> ContentConstructors_ = []
		{
			std::map<std::string, std::function<std::unique_ptr<Base<>>()>> content_constructors;
			(Selector<>::add_type<Contents>(content_constructors), ...);
			return content_constructors;
		}();
}
