# pragma once
# include <functional>
# include <span>
# include <mirism/utility.hpp>
# include <mirism/framework.hpp>

namespace mirism::handler
{
	class Base
	{
		public: virtual ~Base() = default;

		public: enum class PatchTiming
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
		public: struct Patch;
		public: using PatchList
			= std::vector<std::pair<PatchTiming, std::vector<std::pair<std::shared_ptr<Patch>, bool>>>>;
		public: using PatchListSpan
			= std::span<std::pair<PatchTiming, std::vector<std::pair<std::shared_ptr<Patch>, bool>>>>;
		public: struct Patch
		{
			std::string Name;
			PatchTiming Timing;
			std::function<bool
			(
				std::experimental::observer_ptr<http::Request>, std::experimental::observer_ptr<http::Response>,
				const PatchListSpan&
			)> Patch;
		};

		public: struct Url
		{
			public: Url(std::string url, std::string request_domain, std::string request_path);
			public: enum class Type {Full, Half, Absolute, Relative} Type;
			public: std::optional<std::string> Scheme, HostAndPort;
			public: std::string Path, RequestDomain, RequestPath;
		};

		public: enum class UrlPatchTiming {BeforePatch, AtPatch, AfterPatch};
		public: struct UrlPatch;
		public: using UrlPatchList
			= std::vector<std::pair<UrlPatchTiming, std::vector<std::pair<std::shared_ptr<UrlPatch>, bool>>>>;
		public: using UrlPatchListSpan
			= std::span<std::pair<UrlPatchTiming, std::vector<std::pair<std::shared_ptr<UrlPatch>, bool>>>>;
		public: struct UrlPatch
		{
			std::string Name;
			UrlPatchTiming Timing;
			std::function<std::optional<Url>(Url, const UrlPatchListSpan&)> Patch, Depatch;
		};

		protected: Base
		(
			std::shared_ptr<client::Base> client,
			std::vector<PatchListSpan> patch_list, std::vector<UrlPatchListSpan> url_patch_list
		);

		// handle a request, and return a response
		// nullptr might be used for testing or once any fatal error occurs
		public: virtual std::shared_ptr<http::Response> operator()(std::shared_ptr<http::Request> request) = 0;

		// get patch list
		// outside objects should not hold the shared pointer when the handler is going to be destroyed
		public: virtual PatchListSpan get_patch_list() const = 0;
		public: virtual UrlPatchListSpan get_url_patch_list() const = 0;

		protected: PatchList PatchList_;
		protected: UrlPatchList UrlPatchList_;
		protected: std::shared_ptr<client::Base> Client_;
	};
}
