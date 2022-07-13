# pragma once
# include <mirism/detail/instance.hpp>
# include <mirism/detail/server/base.hpp>
# include <mirism/detail/client/base.hpp>
# include <mirism/detail/handler/base.hpp>

namespace mirism
{
	inline Instance::Instance
	(
		std::shared_ptr<server::Base> server, std::shared_ptr<handler::Base> handler,
		std::shared_ptr<client::Base> client
	)
	: Server_(server), Handler_(handler), Client_(client), Status_(Status::Stopped)
		{Logger::Guard log(Server_, Handler_, Client_);}

	inline Instance& Instance::run(bool async)
	{
		Logger::Guard log(async);
		auto status = std::make_optional(Status_.lock());
		if (**status != Status::Stopped) [[unlikely]]
			log.log<Logger::Level::Error>("Instance is already running. Ignoring request.");
		else if (!Server_)  [[unlikely]]
			log.log<Logger::Level::Error>("Server is not set. Ignoring request.");
		else
		{
			if (async)
			{
				auto shutdown_handler = ShutdownHandler_.lock();
				*shutdown_handler = (*Server_)
				(
					async, std::experimental::make_observer(Handler_.get()),
					std::experimental::make_observer(Client_.get())
				);
				if (!*shutdown_handler) [[unlikely]]
					log.log<Logger::Level::Error>("Server returned nullptr, it has failed to start.");
				else
					**status = Status::RunningAsync;
			}
			else
			{
				**status = Status::RunningSync;
				std::jthread thread([&, this]
				{
					Logger::Guard log;
					auto result = (*Server_)
					(
						async, std::experimental::make_observer(Handler_.get()),
						std::experimental::make_observer(Client_.get())
					);
					if (result) [[unlikely]]
						log.log<Logger::Level::Error>("Server returned shutdown handler in sync mode. Ignoring it.");
				});
				status.reset();
				thread.join();
				Status_ = Status::Stopped;
			}
		}
		return *this;
	}
	inline Instance& Instance::shutdown()
	{
		Logger::Guard log;
		auto status = Status_.lock();
		if (*status == Status::Stopped) [[unlikely]]
			log.log<Logger::Level::Error>("Instance is already stopped. Ignoring request.");
		else if (*status == Status::RunningSync) [[unlikely]]
			log.log<Logger::Level::Error>("Instance is running in sync mode. Ignoring request.");
		else
		{
			auto shutdown_handler = ShutdownHandler_.lock();
			if (!*shutdown_handler) [[unlikely]]
				log.log<Logger::Level::Error>("Shutdown handler is not set. Ignoring request.");
			else
			{
				**shutdown_handler();
				*status = Status::Stopped;
				*shutdown_handler = nullptr;
			}
		}
		return *this;
	}

	template <auto Instance::* Member, FixedString Name> inline Instance& Instance::set_(auto value)
	{
		Logger::Guard log(value);
		auto status = Status_.lock();
		if (Status_ != Status::Stopped) [[unlikely]]
			log.log<Logger::Level::Error>("Instance is running. Set {} anyway."_f(Name));
		this->*Member = value;
		return *this;
	}
	inline Instance& Instance::set_server(std::shared_ptr<server::Base> server)
		{return set_<&Instance::Server_, "server">(server);}
	inline Instance& Instance::set_handler(std::shared_ptr<handler::Base> handler)
		{return set_<&Instance::Handler_, "handler">(handler);}
	inline Instance& Instance::set_client(std::shared_ptr<client::Base> client)
		{return set_<&Instance::Client_, "client">(client);}
	template <auto Instance::* Member, FixedString Name> inline auto Instance::get_() const
	{
		Logger::Guard log(value);
		auto status = Status_.lock();
		if constexpr (std::same_as<decltype(Member), decltype(&Instance::Status_)>)
			if constexpr (Member == &Instance::Status_)
				log.rtn(*status);
		log.rtn(this->*Member.get());
	}
	inline std::shared_ptr<server::Base> Instance::get_server() const
		{return get_<&Instance::Server_, "server">();}
	inline std::shared_ptr<handler::Base> Instance::get_handler() const
		{return get_<&Instance::Handler_, "handler">();}
	inline std::shared_ptr<client::Base> Instance::get_client() const
		{return get_<&Instance::Client_, "client">();}
	inline Instance::Status Instance::get_status() const
		{return get_<&Instance::Status_, "status">();}

	inline std::ostream& stream_operators::operator<<(std::ostream& os, const Instance::Request& request)
	{
		return os << "{} {} {} {} {} {} {{{} {}}} {{{} {}}} {}"_f
		(
			request.Version, request.Method, request.Domain, request.Path, request.Headers, request.Body,
			request.Remote.IP, request.Remote.Port, request.Local.IP, request.Local.Port, request.Cancelled
		);
	}
	inline std::ostream& stream_operators::operator<<(std::ostream& os, const Instance::Response& response)
	{
		return os << "{} {} {} {{{} {}}} {{{} {}}}"_f
		(
			response.Status, response.Headers, response.Body,
			response.Remote.IP, response.Remote.Port, response.Local.IP, response.Local.Port
		);
	}
}
