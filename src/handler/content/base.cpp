# include <mirism/detail_/handler/content/base.tpp>

namespace mirism::content
{
	http::Body Base<>::write(std::optional<std::string> content_encoding, std::optional<std::string> content_type)
	{
		auto body = std::make_shared<http::Body::element_type>();
		this->write(body, content_encoding, content_type);
		return body;
	}
}
