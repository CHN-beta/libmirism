# include <mirism/detail_/handler/content/binary.hpp>

namespace mirism::content
{
	Binary& Binary::read(http::Body content, std::optional<std::string>, std::optional<std::string>)
	{
		Content_ = content;
		return *this;
	}
	Binary& Binary::write(http::Body& content, std::optional<std::string>, std::optional<std::string>)
	{
		content = Content_;
		Content_.reset();
		return *this;
	}
	Binary& Binary::patch(const http::Request& request, handler::Base::UrlHandler&)
		{return *this;}
	Binary& Binary::depatch(const http::Response& response, handler::Base::UrlHandler&)
		{return *this;}
}
