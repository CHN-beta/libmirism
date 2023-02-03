# pragma once
# include <mirism/detail_/handler/content/base.tpp>

namespace mirism::content
{
	class Binary : public Base<"application/octet-stream">
	{
		public: Binary& read
			(http::Body content, std::optional<std::string> content_encoding, std::optional<std::string> content_type)
			override;
		public: Binary& write
			(http::Body& content, std::optional<std::string> content_encoding, std::optional<std::string> content_type)
			override;

		public: Binary& patch
			(const http::Request& request, handler::Base::UrlHandler& url_handler) override;
		public: Binary& depatch
			(const http::Response& response, handler::Base::UrlHandler& url_handler) override;

		public: http::Body Content_;
	};
}
