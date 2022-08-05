# pragma once
# include <mirism/detail_/content/text/base.hpp>

namespace mirism::content::text
{
	inline std::optional<std::string> Base<>::compress(const std::string& data, CompressMethod method)
	{
		Logger::Guard log{method};
		if (method == CompressMethod::Gzip)
			try
			{
				std::stringstream compressed;
				std::stringstream origin{std::move(data)};
				boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
				out.push
				(
					boost::iostreams::gzip_compressor
						(boost::iostreams::gzip_params(boost::iostreams::gzip::best_compression))
				);
				out.push(origin);
				boost::iostreams::copy(out, compressed);
				return std::move(compressed).str();
			}
			catch (...)
			{
				log.log<Logger::Level::Error>("gzip failed");
				return {};
			}
		else if (method == CompressMethod::Deflated)
			try
			{
				std::stringstream compressed;
				std::stringstream origin(std::move(data));
				boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
				out.push
				(
					boost::iostreams::zlib_compressor(boost::iostreams::zlib::best_compression)
				);
				out.push(origin);
				boost::iostreams::copy(out, compressed);
				return std::move(compressed).str();
			}
			catch (...)
			{
				log.log<Logger::Level::Error>("deflated failed");
				return {};
			}
		else if (method == CompressMethod::Brotli)
		{
			static_assert(sizeof(uint8_t) == sizeof(char));
			try
			{
				auto instance = BrotliEncoderCreateInstance(nullptr, nullptr, nullptr);
				std::array<uint8_t, 1u << 20> buffer;
				std::string result;

				size_t available_in = data.length(), available_out = buffer.size();
				const uint8_t* next_in = reinterpret_cast<const uint8_t*>(data.c_str());
				uint8_t* next_out = buffer.data();

				do
				{
					auto compress_status = BrotliEncoderCompressStream
					(
						instance, BROTLI_OPERATION_FINISH,
						&available_in, &next_in, &available_out, &next_out, nullptr
					);
					if (!compress_status)
					{
						log.log<Logger::Level::Error>("brotli failed");
						BrotliEncoderDestroyInstance(instance);
						return {};
					}
					result.append(reinterpret_cast<const char*>(buffer.data()), buffer.size() - available_out);
					available_out = buffer.size();
					next_out = buffer.data();
				}
				while (!(available_in == 0 && BrotliEncoderIsFinished(instance)));

				BrotliEncoderDestroyInstance(instance);
				return result;
			}
			catch (...)
			{
				log.log<Logger::Level::Error>("compress brotli failed");
				return {};
			}
		}
		else
		{
			log.log<Logger::Level::Error>("unknown compress method");
			return {};
		}
	}
	inline std::optional<std::string> Base<>::decompress(const std::string& data, CompressMethod method)
	{
		Logger::Guard log{method};
		if (method == CompressMethod::Gzip)
			try
			{
				std::stringstream decompressed;
				std::stringstream origin{std::move(data)};
				boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
				out.push(boost::iostreams::gzip_decompressor{});
				out.push(origin);
				boost::iostreams::copy(out, decompressed);
				return std::move(decompressed).str();
			}
			catch (...)
			{
				log.log<Logger::Level::Error>("gzip failed");
				return {};
			}
		else if (method == CompressMethod::Deflated)
			try
			{
				std::stringstream decompressed;
				std::stringstream origin{std::move(data)};
				boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
				out.push(boost::iostreams::zlib_decompressor{});
				out.push(origin);
				boost::iostreams::copy(out, decompressed);
				return std::move(decompressed).str();
			}
			catch (...)
			{
				log.log<Logger::Level::Error>("deflated failed");
				return {};
			}
		else if (method == CompressMethod::Brotli)
		{
			static_assert(sizeof(uint8_t) == sizeof(char));
			try
			{
				auto instance = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);
				std::array<uint8_t, 1u << 20> buffer;
				std::string result;

				size_t available_in = data.length(), available_out = buffer.size();
				const uint8_t* next_in = reinterpret_cast<const uint8_t*>(data.data());
				uint8_t* next_out = buffer.data();
				BrotliDecoderResult oneshot_result;

				do
				{
					oneshot_result = BrotliDecoderDecompressStream
						(instance, &available_in, &next_in, &available_out, &next_out, nullptr);
					if
					(
						oneshot_result == BROTLI_DECODER_RESULT_ERROR
						|| oneshot_result == BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT
					)
					{
						log.log<Logger::Level::Error>("brotli decompress failed");
						BrotliDecoderDestroyInstance(instance);
						return {};
					}
					result.append(reinterpret_cast<const char*>(buffer.data()), buffer.size() - available_out);
					available_out = buffer.size();
					next_out = buffer.data();
				}
				while (!(available_in == 0 && oneshot_result == BROTLI_DECODER_RESULT_SUCCESS));

				BrotliDecoderDestroyInstance(instance);
				return result;
			}
			catch (...)
			{
				log.log<Logger::Level::Error>("brotli decompress failed");
				return {};
			}
		}
	}

	inline cppcoro::generator<std::pair<std::string_view, std::sregex_iterator>> Base<>::find
		(const std::string& data, std::regex regex)
	{
		Logger::Guard log;
		std::string::const_iterator unmatched_prefix_begin = data.cbegin(), unmatched_prefix_end;
		std::sregex_iterator regit;
		while (true)
		{
			if (regit == std::sregex_iterator{})
				regit = std::sregex_iterator{data.begin(), data.end(), regex};
			else
				regit++;
			if (regit == std::sregex_iterator{})
				unmatched_prefix_end = data.end();
			else
				unmatched_prefix_end = (*regit)[0].first;
			co_yield
			{
				std::string_view{&*unmatched_prefix_begin, std::distance(unmatched_prefix_begin, unmatched_prefix_end)},
				regit
			};
			if (regit == std::sregex_iterator{})
				break;
			unmatched_prefix_begin = (*regit)[0].second;
		}
	}

	inline std::string Base<>::replace
		(const std::string& data, const std::regex& regex, std::function<std::string(const std::smatch&)> function)
	{
		Logger::Guard log;
		std::string result;
		for (auto matched : find(data, regex))
		{
			result.append(matched.first);
			if (matched.second != std::sregex_iterator{})
				result.append(function(*matched.second));
		}
		return result;
	}

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
