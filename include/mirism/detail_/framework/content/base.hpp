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

		public: virtual Base<>& read(http::Body_t content, std::optional<std::string> content_encoding) = 0;
		public: virtual http::Body_t write(std::optional<std::string> content_encoding) = 0;

		public: virtual Base& patch(const http::Request_t& request, handler::Base::UrlHandler& url_handler) = 0;
		public: virtual Base& depatch(const http::Response_t& response, handler::Base::UrlHandler& url_handler) = 0;

		public: virtual std::unordered_set<std::string> get_content_type_set() const = 0;
		protected: http::Body_t Content_;
	};
	template <VariableString<64>... ContentTypes> class Base : virtual public Base<>
	{
		public: std::unordered_set<std::string> get_content_type_set() const override
		{
			return {{ContentTypes.Data, ContentTypes.Size}...};
		}
	};
}
