# pragma once
# include <mirism/detail_/content/base.hpp>

namespace mirism::content::text
{
	template <VariableString<64>... SupportedContentTypes> class Base;
	template<> class Base<> : virtual public content::Base<>
	{
		protected: std::optional<std::string> Data_;

		// Compress or decompress a string. On error return std::nullopt
		public: enum class CompressMethod {Gzip, Deflated, Brotli};
		public: static std::optional<std::string> compress(const std::string& data, CompressMethod method);
		public: static std::optional<std::string> decompress(const std::string& data, CompressMethod method);

		// Find specific content in a string view. Return unmatched content before the match and the match result every
		// time. If match reached the end, the second returned value will be std::sregex_iterator().
		public: static cppcoro::generator<std::pair<std::string_view, std::sregex_iterator>> find
			(const std::string& data, std::regex regex);
		public: static cppcoro::generator<std::pair<std::string_view, std::sregex_iterator>> find
			(std::string&& data, std::regex regex);

		// Use a regex to find all matches and replace them with a callback function
		public: static std::string replace
			(const std::string& data, const std::regex& regex, std::function<std::string(const std::smatch&)> function);

		// Read synchronously fmrom a pipe. will read all data into Data_, that is, until read failed, Break, EndOfFile.
		// If read failed or Break, return false and no data will preserve.
		public: virtual bool read(std::shared_ptr<Pipe> pipe, std::optional<CompressMethod> method = {});
		public: virtual bool write(std::shared_ptr<Pipe> pipe, std::optional<CompressMethod> method = {});
		public: virtual bool read(std::shared_ptr<Pipe> pipe, std::optional<std::string> content_encoding = {});
		public: virtual bool write(std::shared_ptr<Pipe> pipe, std::optional<std::string> content_encoding = {});
		protected: template <bool (Base<>::* Function)(std::shared_ptr<Pipe>, std::optional<CompressMethod>)> bool
			content_encoding_to_method_(std::shared_ptr<Pipe> pipe, std::optional<std::string> content_encoding);

		// Text do not automatically apply patch. Instead, use this function to apply patch.
		// It is guaranteed that the whole data will be supplied to the patch in one shot.
		public: virtual Base<>& patch_apply();

		// apply patch without register it.
		public: virtual Base<>& patch_apply(std::move_only_function<void(std::string&)> patch);
	};
	template <VariableString<64>... SupportedContentTypes> requires(sizeof...(SupportedContentTypes) > 1)
		class Base<SupportedContentTypes...>
		: virtual public Base<>, virtual public content::Base<SupportedContentTypes...> {};
}
