# pragma once
# include <mirism/detail_/misc/common.hpp>

namespace mirism::http
{
	enum class Scheme {Http, Https};
	enum class Version {v1_0, v1_1, v2, v3};
	enum class Method {Connect, Delete, Get, Head, Options, Patch, Post, Put, Trace};
	struct Request
	{
		std::optional<Scheme> Scheme;
		std::optional<Version> Version;
		Method Method;
		std::string Path;
		std::multimap<std::string, std::string, CaseInsensitiveStringLess> Headers;
		std::shared_ptr<Pipe> Body;
		struct
		{
			std::optional<std::variant<std::uint32_t, std::array<std::uint16_t, 8>>> IP;
			std::optional<std::uint16_t> Port;
		} Remote, Local;
		std::shared_ptr<Atomic<bool>> Cancelled;
	};
	struct Response
	{
		std::uint16_t Status;
		std::multimap<std::string, std::string, CaseInsensitiveStringLess> Headers;
		std::shared_ptr<Pipe> Body;
		struct
		{
			std::optional<std::variant<std::uint32_t, std::array<std::uint16_t, 8>>> IP;
			std::optional<std::uint16_t> Port;
		} Remote, Local;
	};
	std::optional<std::variant<std::uint32_t, std::array<std::uint16_t, 8>>> ip_convert(const std::string& ip_str);
	std::string ip_convert(const std::uint32_t ip);
	std::string ip_convert(const std::array<std::uint16_t, 8> ip);
	std::string ip_convert(const std::variant<std::uint32_t, std::array<std::uint16_t, 8>>& ip);
}

namespace mirism::inline stream_operators
{
	std::ostream& operator<<(std::ostream& os, const http::Request& request);
	std::ostream& operator<<(std::ostream& os, const http::Response& response);
}
template<> struct fmt::formatter<mirism::Instance::Request> : public fmt::ostream_formatter {};
template<> struct fmt::formatter<mirism::Instance::Response> : public fmt::ostream_formatter {};
