# pragma once
# include <mirism/detail_/handler/handler/base.hpp>

namespace mirism::handler
{
	template <std::derived_from<site::Base<>>... Site_ts> class Domain_t;
	template <> class Domain_t<> : public Base_t
	{
		protected: struct RouteStrategy_t
		{
			std::unordered_map<std::string, std::string> CodeToWord, WordToCode;
			std::unordered_map<std::string, std::shared_ptr<site::Base<>>> DomainToSite;
			RouteStrategy_t(std::vector<std::shared_ptr<site::Base<>>> sites);
		}

		public: Domain_t
			(std::shared_ptr<framework::Client_t> client, std::vector<std::shared_ptr<site::Base_t<>>> sites);

		public: PatchListSpan_t get_patch_list() const override;
		public: UrlPatchListSpan_t get_url_patch_list() const override;

		protected: PatchList_t PatchList_;
		protected: UrlPatchList_t UrlPatchList_;
	}
}
