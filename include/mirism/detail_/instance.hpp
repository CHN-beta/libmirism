# pragma once
# include <mirism/detail_/pipe.hpp>

namespace mirism
{
	namespace server {class Base;}
	namespace client {class Base;}
	namespace handler {class Base;}

	// Framework object of libmirism.
	class Instance : public Logger::ObjectMonitor<Instance>
	{

		// All the content in EndPoint, Request and Response should be legal (e.g. Host only contains legal characters,
		// and so on).
		public: enum class HttpScheme {Http, Https};
		public: enum class HttpVersion {v1_0, v1_1, v2, v3};
		public: enum class HttpMethod {Get, Head, Post, Put, Delete, Connect, Options, Trace, Patch};
		public: struct Request
		{
			std::optional<HttpScheme> Scheme;
			std::optional<HttpVersion> Version;
			HttpMethod Method;
			std::string Path;
			std::multimap<std::string, std::string, CaseInsensitiveStringLess> Headers;
			std::shared_ptr<Pipe> Body;
			struct
			{
				std::optional<std::uint32_t> IP;
				std::optional<std::uint16_t> Port;
			} Remote, Local;
			std::shared_ptr<Atomic<bool>> Cancelled;
		};
		public: struct Response
		{
			std::uint16_t Status;
			std::multimap<std::string, std::string, CaseInsensitiveStringLess> Headers;
			std::shared_ptr<Pipe> Body;
			struct
			{
				std::optional<std::uint32_t> IP;
				std::optional<std::uint16_t> Port;
			} Remote, Local;
		};

		protected: std::shared_ptr<server::Base> Server_;
		protected: std::shared_ptr<handler::Base> Handler_;
		protected: std::shared_ptr<client::Base> Client_;

		// Define the status of the instance, its lock is also used to prevent the member function of Instance be called
		// symultaneously.
		public: enum class Status {RunningSync, RunningAsync, Stopped};
		protected: Atomic<Status> Status_;

		// Under async mode, notify the server object to stop.
		protected: Atomic<std::move_only_function<void()>> ShutdownHandler_;

		public: Instance
		(
			std::shared_ptr<server::Base> server = nullptr, std::shared_ptr<handler::Base> handler = nullptr,
			std::shared_ptr<client::Base> client = nullptr
		);
		public: virtual ~Instance() = default;

		public: Instance& run(bool async = false);
		public: Instance& shutdown();

		protected: template <auto Instance::* Member, FixedString Name> [[gnu::always_inline]] Instance& set_(auto value);
		public: Instance& set_server(std::shared_ptr<server::Base> server = nullptr);
		public: Instance& set_handler(std::shared_ptr<handler::Base> handler = nullptr);
		public: Instance& set_client(std::shared_ptr<client::Base> client = nullptr);
		protected: template <auto Instance::* Member, FixedString Name> [[gnu::always_inline]] auto get_() const;
		public: std::shared_ptr<server::Base> get_server() const;
		public: std::shared_ptr<handler::Base> get_handler() const;
		public: std::shared_ptr<client::Base> get_client() const;
		public: Status get_status() const;
	};
	inline namespace stream_operators
	{
		std::ostream& operator<<(std::ostream& os, const Instance::Request& request);
		std::ostream& operator<<(std::ostream& os, const Instance::Response& response);
	}
}
