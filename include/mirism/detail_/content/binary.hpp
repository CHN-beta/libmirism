# pragma once
# include <mirism/detail_/content/base.hpp>

namespace mirism::content
{
	// Binary is thread safe
	class Binary : public Logger::ObjectMonitor<Binary>, public Base
	<
		"application/octet-stream", "application/ogg", "application/pdf", "application/x-endnote-refer",
		"application/x-research-info-systems", "application/zip", "audio/mpeg", "font/otf", "font/ttf", "font/woff",
		"font/woff2", "image/gif", "image/jpeg", "image/png", "image/svg+xml", "image/vnd.microsoft.icon", "image/webp",
		"image/x-icon"
	>
	{
		protected: mutable std::mutex Mutex_;
		protected: std::shared_ptr<Pipe> ReadPipe_, WritePipe_;

		// transfer data from read pipe to write pipe in async way, until reached EOF, Break, or read/write failed.
		public: virtual Binary& process();

		// thread safe version
		public: virtual Binary& set_pipe_read(std::shared_ptr<Pipe> pipe);
		public: virtual Binary& set_pipe_write(std::shared_ptr<Pipe> pipe);
		public: std::size_t patch_register(std::move_only_function<void(std::string&)> patch) override;
		public: Binary& patch_unregister(std::size_t id) override;
	};
}
