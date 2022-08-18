# pragma once
# include <mirism/detail_/instance.hpp>
# include <mirism/detail_/server/base.hpp>
# include <mirism/detail_/client/base.hpp>
# include <mirism/detail_/handler/base.hpp>
# include <mirism/detail_/misc/format.hpp>

namespace mirism
{
	inline Instance::Instance
	(
		std::shared_ptr<server::Base> server, std::shared_ptr<handler::Base> handler,
		std::shared_ptr<client::Base> client
	)
	: Server_{server}, Handler_{handler}, Client_{client}, Status_{Status::Stopped}
		{Logger::Guard log{Server_, Handler_, Client_};}

	inline Instance& Instance::run()
	{
		Logger::Guard log;
		std::lock_guard<std::mutex> lock{Lock_};

		if (Status_ == Status::Running) [[unlikely]]
		{
			log.log<Logger::Level::Error>("Instance is already running. Ignoring request.");
			return *this;
		}
		if (!Server_)  [[unlikely]]
		{
			log.log<Logger::Level::Error>("Server is not set. Ignoring request.");
			return *this;
		}
		if (ShutdownHandler_)
			std::unreachable();

		ShutdownHandler_ = (*Server_)
			(std::experimental::make_observer(Handler_.get()), std::experimental::make_observer(Client_.get()));

		if (!ShutdownHandler_) [[unlikely]]
		{
			log.log<Logger::Level::Error>("Server returned nullptr, it has failed to start.");
			return *this;
		}

		Status_ = Status::Running;
		return *this;
	}
	inline Instance& Instance::shutdown()
	{
		Logger::Guard log;
		std::lock_guard<std::mutex> lock{Lock_};

		if (Status_ == Status::Stopped) [[unlikely]]
		{
			log.log<Logger::Level::Error>("Instance is already stopped. Ignoring request.");
			return *this;
		}
		if (ShutdownHandler_)
			std::unreachable();

		ShutdownHandler_();
		Status_ = Status::Stopped;
		ShutdownHandler_ = nullptr;
		return *this;
	}

	template <auto Instance::* Member, FixedString Name> inline Instance& Instance::set_(auto value)
	{
		Logger::Guard log{value};
		std::lock_guard<std::mutex> lock{Lock_};
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
		Logger::Guard log;
		std::lock_guard<std::mutex> lock{Lock_};
		return log.rtn(this->*Member);
	}
	inline std::shared_ptr<server::Base> Instance::get_server() const
		{return get_<&Instance::Server_, "server">();}
	inline std::shared_ptr<handler::Base> Instance::get_handler() const
		{return get_<&Instance::Handler_, "handler">();}
	inline std::shared_ptr<client::Base> Instance::get_client() const
		{return get_<&Instance::Client_, "client">();}
	inline Instance::Status Instance::get_status() const
		{return get_<&Instance::Status_, "status">();}

	inline std::optional<std::variant<std::uint32_t, std::array<std::uint16_t, 8>>> Instance::ip_convert
		(const std::string& ip_str)
	{
		Logger::Guard log{ip_str};

		auto ipv4_parse = [](const std::string& ip_str) -> std::optional<std::uint32_t>
		{
			std::uint32_t ip;
			if (std::istringstream{ip_str} >> ip)
				return ip;
			return std::nullopt;
		};

		if (ip_str.contains(':'))
		{
			std::vector<std::uint16_t> ip_forward;
			std::vector<std::uint16_t> ip_backward;
			std::optional<std::string> ipv4;
			bool backward = false;
			for (auto& [unmatched, matched] : string::find(ip_str, "([0-9a-fA-F]{0,4})([:]{1,2}|$)"_re))
			{
				// only v4 part of ipv6(dual) could be unmatched
				if (!unmatched.empty()) [[unlikely]]
					return std::nullopt;

				// if matched is empty, it means we reached the end of the string
				// however, this should not happen, because we have exit after the last match
				if (matched == std::sregex_iterator{}) [[unlikely]]
					return std::nullopt;

				// put into forward or backward vector
				if (backward)
					ip_backward.push_back(std::stoi((*matched)[1].str(), nullptr, 16));
				else
					ip_forward.push_back(std::stoi((*matched)[1].str(), nullptr, 16));

				// if matched[2] is "::", we shoud go to backward; else if empty, we should break
				if ((*matched)[2].str() == "::")
				{
					if (backward) [[unlikely]]
						return std::nullopt;
					backward = true;
				}
				else if ((*matched)[2].str().empty())
					break;
			}

			std::array<std::uint16_t, 8> ip_array;
			for (std::size_t i = 0; i < ip_forward.size(); ++i)
				ip_array[i] = ip_forward[i];
			for (std::size_t i = 0; i < ip_backward.size(); ++i)
				ip_array[8 - ip_backward.size()] = ip_backward[i];
		}
		else
		{

		}
		auto result = std::experimental::make_observer(ip_str);
		if (result)
		{
			auto [ip, port] = *result;
			if (ip.size() == 4)
				return std::variant<std::uint32_t, std::array<std::uint16_t, 8>>{ip};
			else if (ip.size() == 16)
			{
				std::array<std::uint16_t, 8> array{};
				for (std::size_t i = 0; i < 8; ++i)
					array[i] = ip[i * 2] << 8 | ip[i * 2 + 1];
				return std::variant<std::uint32_t, std::array<std::uint16_t, 8>>{array};
			}
			else
				return std::nullopt;
		}
		else
			return std::nullopt;
	}

	inline std::ostream& stream_operators::operator<<(std::ostream& os, const Instance::Request& request)
	{
		return os << "{} {} {} {} {} {{{} {}}} {{{} {}}} {}"_f
		(
			request.Version, request.Method, request.Path, request.Headers, request.Body,
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
