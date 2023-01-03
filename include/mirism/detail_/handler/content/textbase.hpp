# pragma once
# include <mirism/detail_/framework/content/base.hpp>

namespace mirism::content
{
	template <VariableString<64>... ContentTypes> class TextBase;
	template <> class TextBase<> : virtual public Base<>
	{
		public: TextBase<>& read(http::Body_t content, std::optional<std::string> content_encoding) override;
		public: http::Body_t write(std::optional<std::string> content_encoding) override;
		public: virtual TextBase<>& patch(std::function<void(std::string&)> patcher);
		protected: std::string Text_;
	};
	template <VariableString<64>... ContentTypes> class TextBase : public Base<ContentTypes...>, public TextBase<> {};
}
