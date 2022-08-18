# pragma once
# include <mirism/detail_/content/text/base.hpp>

namespace mirism::content::text
{
	inline bool Base<>::read(std::shared_ptr<Pipe> pipe, std::optional<CompressMethod> method)
	{
		Logger::Guard log{pipe, method};
		Data_.reset();
		if (!pipe) [[unlikely]]
		{
			log.log<Logger::Level::Error>("pipe is nullptr, ignore now.");
			return false;
		}
		while (true)
		{
			auto data = pipe->pop();
			log.log<Logger::Level::Debug>("read data: {}", data);
			if (!data) [[unlikely]]
			{
				log.log<Logger::Level::Error>("read failed, break now.");
				Data_.clear();
				return false;
			}
			else if (auto strp = std::get_if<std::string>(&*data))
				*Data_+= *strp;
			else if (auto signalp = std::get_if<Pipe::Signal>(&*data))
			{
				if (*signalp == Pipe::Signal::Break)
				{
					log.log<Logger::Level::Debug>("read break signal, break now.");
					Data_.reset();
					return false;
				}
				else if (*signalp == Pipe::Signal::EndOfFile)
				{
					log.log<Logger::Level::Debug>("read end of file signal.");
					break;
				}
				else
					log.log<Logger::Level::Error>("read unknown signal, ignore it and continue");
			}
			else
				log.log<Logger::Level::Error>("read unknown data, ignore it and continue");
		}
		if (method)
			Data_ = decompress(*Data_, *method);
		return true;
	}
	inline bool Base<>::read(std::shared_ptr<Pipe> pipe, std::optional<std::string> content_encoding)
		{return content_encoding_to_method_<read>(pipe, content_encoding);}
}
