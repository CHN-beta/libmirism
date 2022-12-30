# pragma once
# include <functional>
# include <span>
# include <mirism/utility.hpp>


/*
	framework
		client
			cpp-httplib
		server
			cpp-httplib
		handler
			MirrorPerDomain<Site...>
			MirrorPerPath<Site...>
				ContentProvider
					Html
					xxx
			Api
*/

namespace mirism
{
	namespace client
	{
		class Base
		{
			public: virtual ~Base() = default;

			// fetch a request from upstream
			public: virtual std::optional<http::Response_t> operator()(http::Request_t request) = 0;
		};
	}
	namespace server
	{
		class Base
		{
			public: virtual ~Base() = default;

			// start server, and handle requests using the callback. Return a function to stop the server.
			public: virtual std::function<void()> operator()
				(std::function<std::optional<http::Response_t>(http::Request_t)> callback) = 0;
		};
	}
	namespace handler
	{
		class Base
		{
			public: virtual ~Base() = default;

			// handle a request, and return a response
			public: virtual std::optional<http::Response_t> operator()(http::Request_t request) = 0;
		};
	}
	class Framework : public Logger::ObjectMonitor<Framework>
	{
		public: virtual ~Framework() = default;

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

		public: Framework
		(
			std::shared_ptr<client::Base> client, std::shared_ptr<server::Base> server,
			std::vector<std::shared_ptr<Patch>> patch_list = {}, bool insert_internal_patch = true,
			std::vector<std::shared_ptr<Handler::Base
		);
		public: virtual void start();
		public: virtual void stop();
		public: virtual void wait();

		protected: std::vector<std::pair<PatchTiming, std::vector<Patch>>> PatchList_;

		// public: static Patch xxx;
	};
};
