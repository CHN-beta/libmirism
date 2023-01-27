# pragma once
# include <functional>
# include <span>
# include <mirism/utility.hpp>
# include <mirism/framework.hpp>

namespace mirism::handler
{
	class Base_t
	{
		public: virtual ~Base_t() = default;

		public: enum class PatchTiming_t
		{
			BeforeAllPatch,
				BeforeRequestHeaderPatch, AtRequestHeaderPatch, AfterRequestHeaderPatch,
				BeforeRequestBodyPatch,
					BeforeRequestBodyRead, AtRequestBodyRead, AfterRequestBodyRead,
					BeforeInternalRequestBodyPatch, AtRequestBodyPatch, AfterInternalRequestBodyPatch,
					BeforeRequestBodyWrite, AtRequestBodyWrite, AfterRequestBodyWrite,
				AfterRequestBodyPatch,
				BeforeClientPatch, AtClientPatch, AfterClientPatch,
				BeforeResponseHeaderPatch, AtResponseHeaderPatch, AfterResponseHeaderPatch,
				BeforeResponseBodyPatch,
					BeforeResponseBodyRead, AtResponseBodyRead, AfterResponseBodyRead,
					BeforeInternalResponseBodyPatch, AtResponseBodyPatch, AfterInternalResponseBodyPatch,
					BeforeResponseBodyWrite, AtResponseBodyRead, AfterResponseBodyWrite,
				AfterResponseBodyPatch,
			AfterAllPatch
		};
		public: struct Patch_t;
		public: using PatchList_t
			= std::vector<std::pair<PatchTiming_t, std::vector<std::pair<std::shared_ptr<Patch_t>, bool>>>>;
		public: using PatchListSpan_t
			= std::span<std::pair<PatchTiming_t, std::vector<std::pair<std::shared_ptr<Patch_t>, bool>>>>;
		public: struct Patch_t
		{
			std::string Name;
			PatchTiming_t Timing;
			std::function<bool
			(
				std::experimental::observer_ptr<http::Request_t>, std::experimental::observer_ptr<http::Response_t>,
				const PatchListSpan_t&
			)> Patch;
		};

		public: struct Url_t
		{
			public: Url_t(std::string url, std::string request_domain, std::string request_path);
			public: enum class Type_t {Full, Half, Absolute, Relative} Type;
			public: std::optional<std::string> Scheme, HostAndPort;
			public: std::string Path, RequestDomain, RequestPath;
		};

		public: enum class UrlPatchTiming_t {BeforePatch, AtPatch, AfterPatch};
		public: struct UrlPatch_t;
		public: using UrlPatchList_t
			= std::vector<std::pair<UrlPatchTiming_t, std::vector<std::pair<std::shared_ptr<UrlPatch_t>, bool>>>>;
		public: using UrlPatchListSpan_t
			= std::span<std::pair<UrlPatchTiming_t, std::vector<std::pair<std::shared_ptr<UrlPatch_t>, bool>>>>;
		public: struct UrlPatch_t
		{
			std::string Name;
			UrlPatchTiming_t Timing;
			std::function<std::optional<Url_t>(Url_t, const UrlPatchListSpan_t&)> Patch, Depatch;
		};

		protected: Base_t
		(
			std::shared_ptr<client::Base_t> client,
			std::vector<PatchListSpan_t> patch_list, std::vector<UrlPatchListSpan_t> url_patch_list
		);

		// handle a request, and return a response
		// nullptr might be used for testing or once any fatal error occurs
		public: virtual std::shared_ptr<http::Response_t> operator()(std::shared_ptr<http::Request_t> request) = 0;

		// get patch list
		// outside objects should not hold the shared pointer when the handler is going to be destroyed
		public: virtual PatchListSpan_t get_patch_list() const = 0;
		public: virtual UrlPatchListSpan_t get_url_patch_list() const = 0;

		protected: PatchList_t PatchList_;
		protected: UrlPatchList_t UrlPatchList_;
		protected: std::shared_ptr<client::Base> Client_;
	};
}
