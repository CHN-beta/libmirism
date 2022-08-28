# pragma once
# include <mirism/detail_/handler/base.hpp>

namespace mirism::handler::mirror
{
	class Base : public mirism::handler::Base
	{
		public: virtual std::optional<std::string> url_convert
			(std::observer_ptr<const Instance::Request> request, std::string url) = 0;
		public: virtual std::optional<std::string> url_convert
			(std::observer_ptr<const Instance::Response> response, std::string url) = 0;

		public: struct Request : public Instance::Request
		{
			
		};

	enum class PatchTiming
	{
		BeforeAllPatch,
		BeforeRequestHeaderPatch,
		BeforeInternalRequestHeaderPatch,
		AtRequestHeaderPatch,
		AfterInternalRequestHeaderPatch,
		AfterRequestHeaderPatch,
		BeforeRequestBodyPatch,
		BeforeRequestBodyRead,
		AfterRequestBodyRead,
		BeforeInternalRequestBodyPatch,
		AtRequestBodyPatch,
		AfterInternalRequestBodyPatch,
		BeforeRequestBodyWrite,
		AfterRequestBodyWrite,
		AfterRequestBodyPatch,
		BeforeFetchPatch,
		AfterFetchPatch,
		BeforeResponseHeaderPatch,
		BeforeInternalResponseHeaderPatch,
		AtResponseHeaderPatch,
		AfterInternalResponseHeaderPatch,
		AfterResponseHeaderPatch,
		BeforeResponseBodyPatch,
		BeforeResponseBodyRead,
		AfterResponseBodyRead,
		BeforeInternalResponseBodyPatch,
		AtResponseBodyPatch,
		AfterInternalResponseBodyPatch,
		BeforeResponseBodyWrite,
		AfterResponseBodyWrite,
		AfterResponseBodyPatch,
		AfterAllPatch
	};

	url_t url_patch(const Request&, const DomainStrategy&, const url_t&);

	}
}
