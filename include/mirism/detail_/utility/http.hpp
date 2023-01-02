# pragma once
# include <map>
# include <any>
# include <fmt/ostream.h>
# include <mirism/detail_/utility/atomic.tpp>
# include <mirism/detail_/utility/common.tpp>

namespace mirism::http
{
	enum class Scheme_t {Http, Https};
	enum class Version_t {v1_0, v1_1, v2, v3};
	struct Method_t
	{
		std::string Name;
		bool HasBody;
		static const Method_t Connect;
		static const Method_t Delete;
		static const Method_t Get;
		static const Method_t Head;
		static const Method_t Options;
		static const Method_t Patch;
		static const Method_t Post;
		static const Method_t Put;
		static const Method_t Trace;
	};
	using Headers_t = std::multimap<std::string, std::string, CaseInsensitiveStringLess>;
	using Body_t = std::shared_ptr<std::variant<std::string, std::deque<std::string>>>;
	using Ip_t = std::optional<std::variant<std::uint32_t, std::array<std::uint16_t, 8>>>;
	struct Request_t
	{
		std::optional<Scheme_t> Scheme;
		std::optional<Version_t> Version;
		Method_t Method;
		std::string Path;
		Headers_t Headers;

		// use std::string to store whole body, or use std::deque<std::string>> to store body in parts
		// empty string in deque means end of body
		Body_t Body;
		struct
		{
			Ip_t Ip;
			std::optional<std::uint16_t> Port;
		} Remote, Local;
		std::shared_ptr<Atomic<bool>> Cancelled;
		std::map<std::string, std::any> Extra;
	};
	struct Response_t
	{
		std::uint16_t Status;
		Headers_t Headers;
		Body_t Body;
		struct
		{
			Ip_t Ip;
			std::optional<std::uint16_t> Port;
		} Remote, Local;
		std::shared_ptr<Atomic<bool>> Cancelled;
		std::map<std::string, std::any> Extra;
	};
}

namespace mirism::inline stream_operators
{
	std::ostream& operator<<(std::ostream& os, const http::Method_t& method);
	std::ostream& operator<<(std::ostream& os, const http::Request_t& request);
	std::ostream& operator<<(std::ostream& os, const http::Response_t& response);
}
template<> struct fmt::formatter<mirism::http::Method_t> : public fmt::ostream_formatter {};
template<> struct fmt::formatter<mirism::http::Request_t> : public fmt::ostream_formatter {};
template<> struct fmt::formatter<mirism::http::Response_t> : public fmt::ostream_formatter {};
