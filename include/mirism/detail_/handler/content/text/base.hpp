# pragma once
# include <mirism/detail_/handler/content/base.tpp>

namespace mirism::content::text
{
	class Base : virtual public content::Base<>
	{
		public: virtual Base& read(std::string content, std::optional<std::string> content_type) = 0;
		public: virtual Base& write(std::string& content, std::optional<std::string> content_type) = 0;
		public: Base& read
			(http::Body content, std::optional<std::string> content_encoding, std::optional<std::string> content_type)
			override;
		public: Base& write
			(http::Body& content, std::optional<std::string> content_encoding, std::optional<std::string> content_type)
			override;
	};
}
