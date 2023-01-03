# pragma once
# include <functional>
# include <span>
# include <mirism/utility.hpp>
# include <mirism/detail_/framework/client.hpp>

namespace mirism::handler
{
	class Base
	{
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
		public: using PatchListSpan = std::span<std::pair<PatchTiming, std::vector<std::pair<Patch, bool>>>>;
		public: struct Patch
		{
			std::string Name;
			PatchTiming Timing;
			std::function<bool(http::Request_t&, PatchListSpan&)> Patch;
		};

		public: virtual ~Base() = default;

		public: Base(std::shared_ptr<client::Base> client);

		// handle a request, and return a response
		public: virtual std::optional<http::Response_t> operator()(http::Request_t request) = 0;

		// check if a request should be handled by this handler
		public: virtual bool check(const http::Request_t& request) const = 0;

		public: class UrlHandler
		{
			public: virtual ~UrlHandler() = default;

			public: virtual std::optional<std::string> patch
				(const std::string& url, const std::string& domain, const std::string& path) = 0;
			public: virtual bool patch_check
				(const std::string& url, const std::string& domain, const std::string& path) const = 0;
			public: virtual std::optional<std::string> depatch
				(const std::string& url, const std::string& domain, const std::string& path) = 0;
			public: virtual bool depatch_check
				(const std::string& url, const std::string& domain, const std::string& path) const = 0;
		};
		public: virtual std::shared_ptr<UrlHandler> get_url_handler() const = 0;

		protected: std::vector<std::pair<PatchTiming, std::vector<Patch>>> PatchList_;
		protected: std::shared_ptr<client::Base> Client_;
		protected: std::shared_ptr<UrlHandler> UrlHandler_;
	};
}
