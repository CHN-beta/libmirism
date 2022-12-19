# pragma once
# include <map>
# include <mirism/detail_/utility/atomic.tpp>

namespace mirism
{
	class Logger
	{
		// All the member function corresponding to stacktrace should be inlined
		// All the public function (including public function of member class) is thread safe.

		public: Logger() = delete;

		// Setup output stream and log level in a thread-safe way
		public: enum class Level_t
		{
			None,
			Error,
			Access,
			Info,
			Debug
		};
		protected: struct LoggerConfig_t_
		{
			std::experimental::observer_ptr<std::ostream> Stream;
			std::shared_ptr<std::ostream> StreamStorage;
			Level_t Level;
		};
		protected: static Atomic<std::optional<LoggerConfig_t_>> LoggerConfig_;
		public: static void init(std::experimental::observer_ptr<std::ostream> stream, Level_t level);
		public: static void init(std::shared_ptr<std::ostream> stream, Level_t level);

		// Send a telegram message if token and chat id are set, all the functions are thread-safe
		protected: static Atomic<std::optional<std::pair<std::string, std::string>>> TelegramConfig_;
		public: static void telegram_init(const std::string& token, const std::string& chat_id);
		public: static void telegram_notify(const std::string& message);
		public: static void telegram_notify_async(const std::string& message);

		// Monitor the lifetime of an object
		// usage: struct my_class : protected Logger::ObjectMonitor<my_class> {}
		public: template <typename T> class ObjectMonitor
		{
			protected: const std::chrono::time_point<std::chrono::steady_clock> CreateTime_;

			// call log<Debug>("create {type} at {address}.");
			protected: [[gnu::always_inline]] ObjectMonitor();

			// call log<Debug>("destroy {type} at {address} after {duration} ms.");
			protected: [[gnu::always_inline]] virtual ~ObjectMonitor();
		};
		template <typename T> friend class ObjectMonitor;

		// List of objects that is being monitored by ObjectMonitor, {address, type}
		protected: static Atomic<std::multimap<const void*, std::string_view>> Objects_;

		// Monitor the start and end of a function, as well as corresponding thread.
		// This object should be construct at the beginning of the function, and should never be passed to another
		// function or thread.
		public: class Guard
		{
			protected: thread_local static unsigned Indent_;
			protected: const std::chrono::time_point<std::chrono::steady_clock> StartTime_;

			// if sizeof...(Param) > 0, call log<Debug>("begin function with {arguments}.");
			// else call log<Debug>("begin function.");
			public: template <typename... Param> [[gnu::always_inline]] explicit Guard(Param&&... param);

			// call log<Debug>("end function after {duration} ms.")
			public: [[gnu::always_inline]] virtual ~Guard();

			// call log<Debug>("reached after {duration} ms.")
			public: [[gnu::always_inline]] void operator()() const;

			// call log<Debug>("return {return} after {duration} ms.")
			public: template <typename T> [[gnu::always_inline]] T rtn(T&& value) const;

			// print the following message if LoggerConfig_ is set and the level is higher than the level of the
			// LoggerConfig_
			// [ {time} {thread} {indent} {filename}:{line} {function_name} ] {message}
			public: template <Level_t L> [[gnu::always_inline]] void log(const std::string& message) const;
		};
		friend class Guard;

		// list of threads which is being monitored by Guard and number of Guard created in this thread so far
		protected: static Atomic<std::map<std::size_t, std::size_t>> Threads_;
	};
}