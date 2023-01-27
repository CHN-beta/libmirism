# pragma once
# include <unordered_set>
# include <mirism/utility.hpp>
# include <mirism/detail_/framework/handler/base.hpp>

namespace mirism::content
{
	template <ClassTemplateVariableString<64>... ClassContentTypes> class ClassBase;
	template <> class ClassBase<>
	{
		public: virtual ~ClassBase() = default;

		public: virtual ClassBase<>& read(http::ClassBody content, std::optional<std::string> content_encoding) = 0;
		public: virtual http::Body_t write(std::optional<std::string> content_encoding) = 0;

		public: virtual ClassBase& patch
			(const http::ClassRequest& request, handler::ClassBase::ClassUrlHandler& url_handler) = 0;
		public: virtual ClassBase& depatch
			(const http::ClassResponse& response, handler::ClassBase::ClassUrlHandler& url_handler) = 0;

		public: virtual std::unordered_set<std::string> get_content_type_set() const = 0;
		protected: http::ClassBody Content_;
	};
	template <ClassTemplateVariableString<64>... ClassContentTypes> class ClassBase : virtual public ClassBase<>
	{
		public: std::unordered_set<std::string> get_content_type_set() const override
		{
			return {{ClassContentTypes.Data, ContentTypes.Size}...};
		}
	};
}
