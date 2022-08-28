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
			std::vector<std::string> parts;

			// split using '.'
			for (auto& [unmatched, splitter] : string::find(ip_str, R"(\.)"_re))
				parts.push_back(std::string{unmatched});

			if (parts.size() != 4) [[unlikely]]
				return std::nullopt;
			for (auto& part : parts)
			{
				if (!std::regex_match(part, R"(^[0-9]{1,3}$)"_re) || std::stoi(part) > 255) [[unlikely]]
					return std::nullopt;
				ip = (ip << 8) + std::stoi(part);
			}
			return ip;
		};
		auto ipv6_parse = [ipv4_parse](const std::string& ip_str) -> std::optional<std::array<std::uint16_t, 8>>
		{
			std::array<std::uint16_t, 8> ip = {};
			std::vector<std::string> parts;
			std::optional<std::uint32_t> ipv4;
			bool preceding_zero = false, postfix_zero = false;
			std::size_t empty_count = 0;

			// split using ':'
			for (auto& [unmatched, splitter] : string::find(ip_str, ":"_re))
				parts.push_back(std::string{unmatched});

			// shortest: :: 3
			// longest: ::1:2:3:4:5:6:7 9
			if (parts.size() < 3 || parts.size() > 9) [[unlikely]]
				return std::nullopt;
			if (ip_str == "::")
				return ip;
			
			for (std::size_t i = 0; i < parts.size() - 1; i++)
				if (!std::regex_match(parts[i], "[0-9a-fA-F]{0,4}"_re))
					return std::nullopt;
			if (!std::regex_match(parts.back(), "[0-9a-fA-F]{0,4}"_re))
			{
				ipv4 = ipv4_parse(parts.back());
				// for ipv6(dual), max number of parts is 8
				if (!ipv4 || parts.size() > 8) [[unlikely]]
					return std::nullopt;
			}

			for (auto& part : parts)
				if (part.empty())
					empty_count++;
			if (empty_count > 2) [[unlikely]]
				return std::nullopt;
			if (empty_count == 2)
			{
				if (parts[0].empty() && parts[1].empty())
					preceding_zero = true;
				else if (parts[parts.size() - 2].empty() && parts[parts.size() - 1].empty())
					postfix_zero = true;
				else [[unlikely]]
					return std::nullopt;
			}

			if (ipv4)
			{
				ip[7] = *ipv4 & 0xffff;
				ip[6] = *ipv4 >> 16;
				if (preceding_zero)
					for (int i = 5; i >= 0; i--)
						// if parts[parts.size() - (7 - i)] exists and contains non-empty value
						if (parts.size() - (7 - i) >= 2)
							ip[i] = std::stoi(parts[parts.size() - (7 - i)]);
				else if (empty_count == 1)
				{
					if (parts.size() > 7) [[unlikely]]
						return std::nullopt;
					for (std::size i = 0, p = 0; i < parts.size() - 1; i++, p++)
					{
						if (parts[i].empty())
							p += (7 - parts.size());
						else
							ip[p] = std::stoi(parts[i]);
					}
				}
				else
				{
					if (parts.size() != 7) [[unlikely]]
						return std::nullopt;
					for (std::size i = 0; i < parts.size() - 1; i++)
						ip[i] = std::stoi(parts[i]);
				}
			}
			else if (preceding_zero)
				for (std::size i = 2; i < parts.size(); i++)
					ip[i + (8 - parts.size())] = std::stoi(parts[i]);
			else if (postfix_zero)
				for (std::size i = 0; i < parts.size() - 2; i++)
					ip[i] = std::stoi(parts[i]);
			else if (empty_count == 1)
				for (std::size i = 0, p = 0; i < parts.size(); i++, p++)
				{
					if (parts[i].empty())
						p += (8 - parts.size());
					else
						ip[p] = std::stoi(parts[i]);
				}
			else
			{
				if (parts.size() != 8) [[unlikely]]
					return std::nullopt;
				for (std::size i = 0; i < parts.size() - 1; i++)
					ip[i] = std::stoi(parts[i]);
			}
			return ip;
		};

		if (ip_str.contains(':'))
			return log.rtn(ipv6_parse(ip_str));
		else
			return log.rtn(ipv4_parse(ip_str));
	}
	inline std::string Instance::ip_convert(const std::uint32_t& ip)
	{
		Logger::Guard log{ip};
		return log.rtn("{}.{}.{}.{}"_f(ip >> 24, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff));
	}
	inline std::string Instance::ip_convert(const std::array<std::uint16_t, 8>& ip)
	{
		Logger::Guard log{ip};
		std::string ip_str;
		for (std::size_t i = 0; i < ip.size(); i++)
			if (ip[i] == 0)
			{
				for (std::size_t j = i; j < ip.size() - 1; j++)
					if (ip[j + 1] != 0)
					{
						if (i == 0)
							ip_str = ":";
						else
							for (std::size_t k = 0; k < i; k++)
								ip_str += "{:x}:"_f(k);
						for (std::size_t k = j + 1; k < 8; k++)
							ip_str += "{:x}:"_f(k);
						return log.rtn(ip_str);
					}
				if (i == 0)
					return log.rtn("::");
				for (std::size_t k = 0; k < i; k++)
					ip_str += "{:x}:"_f(k);
				return log.rtn(ip_str + ":");
			}
		for (std::size_t i = 0; i < ip.size(); i++)
			ip_str += "{:x}:"_f(i);
		return log.rtn(ip_str.substr(0, ip_str.size() - 1));
	}
	inline std::string Instance::ip_convert(const std::variant<std::uint32_t, std::array<std::uint16_t, 8>>& ip)
	{
		Logger::Guard log{ip};
		if (std::holds_alternative<std::uint32_t>(ip))
			return log.rtn(ip_convert(std::get<std::uint32_t>(ip)));
		else if (std::holds_alternative<std::array<std::uint16_t, 8>>(ip))
			return log.rtn(ip_convert(std::get<std::array<std::uint16_t, 8>>(ip)));
		else
			std::unreachable();
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
