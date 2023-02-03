# pragma once
# include <unordered_set>
# include <mirism/utility.hpp>
# include <mirism/detail_/framework/handler/base.hpp>

namespace mirism::content
{
	template <VariableString<64>... ContentTypes> class Base;
	template <> class Base<>
	{
		public: virtual ~Base() = default;

		public: virtual Base<>& read
			(http::Body content, std::optional<std::string> content_type, std::optional<std::string> content_encoding)
			= 0;
		public: virtual Base<>& write
			(http::Body& content, std::optional<std::string> content_encoding, std::optional<std::string> content_type)
			= 0;
		public: virtual http::Body write
			(std::optional<std::string> content_encoding, std::optional<std::string> content_type);

		public: virtual Base& patch
			(const http::Request& request, handler::Base::UrlHandler& url_handler) = 0;
		public: virtual Base& depatch
			(const http::Response& response, handler::Base::UrlHandler& url_handler) = 0;

		public: virtual std::unordered_set<std::string> get_content_type_set() const = 0;
	};
	template <VariableString<64>... ContentTypes> class Base : virtual public Base<>
		{public: std::unordered_set<std::string> get_content_type_set() const override;};
}
