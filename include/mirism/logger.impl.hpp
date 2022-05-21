# pragma once

# include <mirism/logger.hpp>

namespace mirism
{
	inline Atomic<std::optional<std::pair<std::experimental::observer_ptr<std::ostream>, Logger::Level>>>
		Logger::LoggerConfig_;
	inline void Logger::init(std::experimental::observer_ptr<std::ostream> stream, Logger::Level level)
		{LoggerConfig_ = std::make_pair(stream, level);}

	inline Atomic<std::optional<std::pair<std::string, std::string>>> Logger::TelegramConfig_;
	inline void Logger::telegram_init(const std::string& token, const std::string& chat_id)
		{TelegramConfig_ = std::make_pair(token, chat_id);}
	inline void Logger::telegram_notify(const std::string& message)
	{
		TelegramConfig_.read([&message](auto&& config)
		{
			if (!config)
				return;
			TgBot::Bot bot(config->first);
			bot.getApi().sendMessage(config->second, message);
		});
	}
	inline void Logger::telegram_notify_async(const std::string& message)
		{std::thread(Logger::telegram_notify, message).detach();}

	inline Atomic<std::multimap<const void*, std::string_view>> Logger::Objects;
	inline Logger::ObjectMonitor::ObjectMonitor()
	:	CreateTime_(std::chrono::steady_clock::now()), Type_(boost::core::demangle(typeid(*this).name()))
	{
		Guard guard;
		guard.log<Level::Debug>("create {} at {}."_f(Type_, fmt::ptr(this)));
		Objects.write([this](auto& objects){objects.insert({this, Type_});});
	}
	inline Logger::ObjectMonitor::~ObjectMonitor()
	{
		Guard guard;
		guard.log<Level::Debug>("destroy {} at {} after {} ms."_f
		(
			Type_, fmt::ptr(this),
			std::chrono::duration_cast<std::chrono::milliseconds>
				(std::chrono::steady_clock::now() - CreateTime_).count()
		));
		Objects.write([this, &guard](auto& objects)
		{
			auto range = objects.equal_range(this);
			for (auto it = range.first; it != range.second; it++)
				if (it->second == Type_)
				{
					objects.erase(it);
					return;
				}
			guard.log<Level::Error>("{} {} not found in Logger::Objects."_f(fmt::ptr(this), Type_));
		});
	}

	inline Atomic<std::map<std::size_t, std::size_t>> Logger::Threads;
	inline thread_local unsigned Logger::Guard::Indent_ = 0;
	template <typename... Param> inline Logger::Guard::Guard(Param&&... param)
	:	StartTime_(std::chrono::steady_clock::now())
	{
		Indent_++;
		Threads.write([](auto& threads)
		{
			auto thread_id = std::hash<std::thread::id>()(std::this_thread::get_id());
			if (threads.contains(thread_id))
				threads[thread_id]++;
			else
				threads.insert({thread_id, 1});
		});
		if constexpr (sizeof...(Param) > 0)
		{
			std::stringstream ss;
			std::vector<std::string> params = {"{}"_f(std::forward<Param>(param))...};
			ss << "begin function with {";
			for (auto& param : params)
				ss << param << ", ";
			ss.seekp(-2, ss.cur);
			ss << "}.";
			log<Level::Debug>(ss.str());
		}
		else
			log<Level::Debug>("begin function.");
	}
	inline Logger::Guard::~Guard()
	{
		log<Level::Debug>("end function after {} ms."_f(std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::steady_clock::now() - StartTime_).count()));
		Indent_--;
		Threads.write([this](auto& threads)
		{
			auto thread_id = std::hash<std::thread::id>()(std::this_thread::get_id());
			if (threads.contains(thread_id))
			{
				threads[thread_id]--;
				if (threads[thread_id] == 0)
					threads.erase(thread_id);
			}
			else [[unlikely]]
				log<Level::Debug>("{:08x} not found in Logger::Threads."_f
				(
					std::hash<std::thread::id>()(std::this_thread::get_id()) % std::numeric_limits<std::uint64_t>::max()
				));
		});
	}
	inline void Logger::Guard::operator()() const
	{
		log<Level::Debug>("reached after {} ms."_f
		(
			std::chrono::duration_cast<std::chrono::milliseconds>
				(std::chrono::steady_clock::now() - StartTime_).count()
		));
	}
	template <typename T> inline T Logger::Guard::rtn(T&& value) const
	{
		log<Level::Debug>("return {} after {} ms."_f
		(
			std::forward<T>(value), 
			std::chrono::duration_cast<std::chrono::milliseconds>
				(std::chrono::steady_clock::now() - StartTime_).count()
		));
		return std::forward<T>(value);
	}
	template <Logger::Level L> inline void Logger::Guard::log(const std::string& message) const
	{
		LoggerConfig_.read([&message](auto&& config)
		{
			if (!config)
				return;
			else if (config->second >= L)
			{
				static_assert(std::same_as<std::size_t, std::uint64_t>);
				auto time = std::chrono::system_clock::now();
				boost::stacktrace::stacktrace stack;
				config->first << "[ {:%Y-%m-%d %H:%M:%S}:{:03} {:08x} {:04} {}:{} {} ] {}\n"_f
				(
					time,
					std::chrono::time_point_cast<std::chrono::milliseconds>(time).time_since_epoch().count() % 1000,
					std::hash<std::thread::id>()(std::this_thread::get_id())
						% std::numeric_limits<std::uint64_t>::max(),
					Indent_,
					stack[1].source_file().empty() ? "??"s : stack[1].source_file().contains("mirism/")
						? stack[1].source_file().substr
							(stack[1].source_file().find_last_of("mirism/") + "mirism/"s.length())
						: stack[1].source_file(),
					stack[1].source_line() == 0 ? "??"s : "{}"_f(stack[1].source_line()),
					stack[1].name(),
					message
				) << std::flush;
			}
		});
	}
}
