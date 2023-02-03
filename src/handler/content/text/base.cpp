# include <mirism/detail_/handler/content/compressed.hpp>

namespace mirism::content::text
{
	Base& Base::read
		(http::Body content, std::optional<std::string> content_encoding, std::optional<std::string> content_type)
	{
		auto [status, body] = http::ReadWholeBody(content);
		if (status != http::ReadResult::Success)
			throw std::runtime_error("Failed to read body.");
		
	}
	Base& Base::write
		(http::Body& content, std::optional<std::string> content_encoding, std::optional<std::string> content_type)
	{
	}
}
