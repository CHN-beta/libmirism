# pragma once
# include <mirism/detail_/framework/content/base.hpp>

namespace mirism::content
{
	template <VariableString<64>... ContentTypes> class TextBase_t;
	template <> class TextBase_t<> : virtual public Base_t<>
	{
		public: TextBase_t<>& read(http::Body_t content, std::optional<std::string> content_encoding) override;
		public: http::Body_t write(std::optional<std::string> content_encoding) override;
		public: virtual TextBase_t<>& patch(std::function<void(std::string&)> patcher);
		protected: std::string Text_;
	};
	template <VariableString<64>... ContentTypes> class TextBase_t
		: public Base_t<ContentTypes...>, public TextBase_t<> {};
}
