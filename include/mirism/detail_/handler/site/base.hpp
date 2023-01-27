# pragma once
# include <mirism/detail_/handler/content/base.hpp>

namespace mirism::site_t
{
	template <typename AllowedRequestContentTypes_t = void, typename AllowedResponseContentTypes_t = void> class Base_t;
	template <> class Base_t<>
	{
		public: virtual ~Base() = default;
		public: using ContentTypeMap_t
			= std::unordered_map<std::string, std::function<std::unique_ptr<content::Base_t<>>()>>;

		public: virtual const ContentTypeMap_t& get_request_content_map() const = 0;
		public: virtual const ContentTypeMap_t& get_response_content_map() const = 0;

		public: virtual std::vector<handler::Base::PatchTiming_t, handler::Base_t::Patch_t> get_patch_list() const = 0;

		public: virtual const DomainStrategy_t& get_domain_strategy() const = 0;

		// 获取该类对应的所有 Group
		virtual
		const DomainStrategy& get_domain_strategy() const = 0;

		// 获取该类使用的所有 content
		virtual
		const content_map_t& get_request_content() const = 0;
		virtual
		std::unique_ptr<content::Base> get_default_request_content() const;
		virtual
		const content_map_t& get_response_content() const = 0;
		virtual
		std::unique_ptr<content::Base> get_default_response_content() const;

	protected:

		// 派生类在构造函数中将 patch 写入这里
		patch_map_t PatchMap_;

		template<std::derived_from<content::Base>... Ts>
		static content_map_t create_content_map_();
	};
	template<std::derived_from<content::Base>... Ts>
	class Base<std::tuple<Ts...>, void> : public virtual Base<void, void>
	{
	public:
		const content_map_t& get_request_content() const override;
	};
	template<std::derived_from<content::Base>... Ts>
	class Base<void, std::tuple<Ts...>> : public virtual Base<void, void>
	{
	public:
		const content_map_t& get_response_content() const override;
	};
	template<typename Requests, typename Responses>
	class Base : public virtual Base<Requests, void>, public virtual Base<void, Responses>
	{};
}
