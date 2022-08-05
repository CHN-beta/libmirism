# pragma once
# include <mirism/detail_/content/binary.hpp>

namespace mirism::content
{
	inline Binary& Binary::process()
	{
		std::unique_lock<std::mutex> lock{Mutex_};
		Logger::Guard log;
		if (ReadPipe_ == nullptr || WritePipe_ == nullptr) [[unlikely]]
		{
			log.log<Logger::Level::Error>("pipe is not set, ignoring");
			return *this;
		}
		std::thread{[lock = std::move(lock), this]
		{
			Logger::Guard log;
			while (true)
			{
				auto chunk = ReadPipe_->pop();
				if (!chunk)
				{
					log.log<Logger::Level::Debug>("read failed, breaking");
					break;
				}
				else if (auto signal = std::get_if<Pipe::Signal>(&*chunk))
				{
					if (*signal == Pipe::Signal::Break || *signal == Pipe::Signal::EndOfFile)
						break;
					else
					{
						log.log<Logger::Level::Error>("unknown signal {}, writing to pipe"_f(*signal));
						if (!WritePipe_->push(*chunk))
						{
							log.log<Logger::Level::Error>("failed to write to pipe, stopping now");
							break;
						}
					}
				}
				else if (auto string = std::get_if<std::string>(&*chunk))
				{
					for (auto& patch : Patches_)
						patch.second(*string);
					if (!WritePipe_->push(*chunk))
					{
						log.log<Logger::Level::Error>("failed to write to pipe, stopping now");
						break;
					}
				}
				else
				{
					log.log<Logger::Level::Error>("unknown chunk type, write anyway");
					if (!WritePipe_->push(*chunk))
					{
						log.log<Logger::Level::Error>("failed to write to pipe, stopping now");
						break;
					}
				}
			}
			ReadPipe_.reset();
			WritePipe_.reset();
		}}.detach();
		return *this;
	}

	inline Binary& Binary::set_pipe_read(std::shared_ptr<Pipe> pipe)
	{
		std::lock_guard<std::mutex> lock{Mutex_};
		ReadPipe_ = std::move(pipe);
		return *this;
	}
	inline Binary& Binary::set_pipe_write(std::shared_ptr<Pipe> pipe)
	{
		std::unique_lock<std::mutex> lock{Mutex_};
		WritePipe_ = std::move(pipe);
		return *this;
	}
	inline std::size_t Binary::patch_register(std::move_only_function<void(std::string&)> patch)
	{
		std::unique_lock<std::mutex> lock{Mutex_};
		return Base<>::patch_register(std::move(patch));
	}
	inline Binary& Binary::patch_unregister(std::size_t id)
	{
		std::unique_lock<std::mutex> lock{Mutex_};
		Base<>::patch_unregister(id);
		return *this;
	}
}
