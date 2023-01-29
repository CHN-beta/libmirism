# pragma once
# include <map>
# include <any>
# include <fmt/ostream.h>
# include <cppcoro/generator.hpp>
# include <mirism/detail_/utility/atomic.tpp>
# include <mirism/detail_/utility/common.tpp>

namespace mirism::http
{
	enum class Scheme {Http, Https};
	enum class Version {v1_0, v1_1, v2, v3};
	struct Method
	{
		std::string Name;
		bool HasBody;
		static const Method Connect;
		static const Method Delete;
		static const Method Get;
		static const Method Head;
		static const Method Options;
		static const Method Patch;
		static const Method Post;
		static const Method Put;
		static const Method Trace;
	};
	using Headers = Atomic<std::multimap<std::string, std::string, CaseInsensitiveStringLessComparator>>;
	using Body = std::shared_ptr<std::variant<Atomic<std::string>, Atomic<std::deque<std::optional<std::string>>>>>;
	using Ip = std::optional<std::variant<std::uint32_t, std::array<std::uint16_t, 8>>>;
	struct Request
	{
		std::optional<Scheme> Scheme;
		std::optional<Version> Version;
		Method Method;
		std::string Path;
		Headers Headers;

		// use std::string to store whole body, or use std::deque<std::string>> to store body in parts
		// empty string in deque means end of body
		Body Body;
		struct
		{
			Ip Ip;
			std::optional<std::uint16_t> Port;
		} Remote, Local;
		std::shared_ptr<Atomic<bool>> Cancelled;
		Atomic<std::map<std::string, std::any>> Extra;
	};
	struct Response_t
	{
		std::uint16_t Status;
		Headers Headers;
		Body Body;
		struct
		{
			Ip Ip;
			std::optional<std::uint16_t> Port;
		} Remote, Local;
		std::shared_ptr<Atomic<bool>> Cancelled;
		Atomic<std::map<std::string, std::any>> Extra;
	};

	// for convenience
	enum class ReadResult {Success, EndOfFile, Timeout, Cancelled, OtherError};
	std::pair<ReadResult, std::optional<std::string>> ReadWholeBody
		(Body body, std::shared_ptr<Atomic<bool>> cancelled, std::chrono::steady_clock::duration timeout);
	cppcoro::generator<std::pair<ReadResult, std::optional<std::string>>> ReadNextBodyPart
		(Body body, std::shared_ptr<Atomic<bool>> cancelled, std::chrono::steady_clock::duration timeout);
}

namespace mirism::inline stream_operators
{
	std::ostream& operator<<(std::ostream& os, const http::Method& method);
	std::ostream& operator<<(std::ostream& os, const http::Request& request);
	std::ostream& operator<<(std::ostream& os, const http::Response& response);
}
template<> struct fmt::formatter<mirism::http::Method> : public fmt::ostream_formatter {};
template<> struct fmt::formatter<mirism::http::Request> : public fmt::ostream_formatter {};
template<> struct fmt::formatter<mirism::http::Response> : public fmt::ostream_formatter {};
