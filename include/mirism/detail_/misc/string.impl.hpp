# pragma once
# include <mirism/detail_/misc/string.hpp>
# include <mirism/detail_/misc/logger.hpp>

namespace mirism
{
	template <typename Char, Char... c> inline std::basic_ostream<Char>& stream_operators::operator<<
		(std::basic_ostream<Char>& os, BasicStaticString<Char, c...>)
		{return os << std::basic_string_view{c...};}
	template <typename Char, Char... c> consteval inline BasicStaticString<Char, c...> literals::operator""_ss()
		{return {};}

	template <typename Char, std::size_t N> constexpr inline
		BasicFixedString<Char, N>::BasicFixedString(const Char (&str)[N])
		{std::copy_n(str, N, Data);}
	template <typename Char, std::size_t N> inline std::basic_ostream<Char>& stream_operators::operator<<
		(std::basic_ostream<Char>& os, const BasicFixedString<Char, N>& str)
		{return os << std::basic_string_view<Char>(str.Data, str.Size);}
	template <BasicFixedString FS> constexpr inline decltype(FS) literals::operator""_fs()
		{return FS;}

	template <typename Char, std::size_t N> template <std::size_t M> requires (M<=N) constexpr inline
		BasicVariableString<Char, N>::BasicVariableString(const Char (&str)[M]) : Size(M)
	{
		std::fill(Data, Data + N, '\0');
		std::copy_n(str, M, Data);
	}
	template <typename Char, std::size_t N> inline std::basic_ostream<Char>& stream_operators::operator<<
		(std::basic_ostream<Char>& os, const BasicVariableString<Char, N>& str)
		{return os << std::basic_string_view<Char>(str.Data, str.Size);}

	inline cppcoro::generator<std::pair<std::string_view, std::sregex_iterator>> string::find
		(SmartRef<const std::string> data, SmartRef<const std::regex> regex)
	{
		Logger::Guard log;
		std::string::const_iterator unmatched_prefix_begin = data->cbegin(), unmatched_prefix_end;
		std::sregex_iterator regit;
		while (true)
		{
			if (regit == std::sregex_iterator{})
				regit = std::sregex_iterator{data->begin(), data->end(), *regex};
			else
				regit++;
			if (regit == std::sregex_iterator{})
			{
				unmatched_prefix_end = data->cend();
				log.log<Logger::Level::Debug>("distance: {}"_f(std::distance(unmatched_prefix_begin, unmatched_prefix_end)));
			}
			else
				unmatched_prefix_end = (*regit)[0].first;
			co_yield
			{
				std::string_view
				{
					&*unmatched_prefix_begin,
					static_cast<std::size_t>(std::distance(unmatched_prefix_begin, unmatched_prefix_end))
				},
				regit
			};
			if (regit == std::sregex_iterator{})
				break;
			unmatched_prefix_begin = (*regit)[0].second;
		}
	}

	inline std::string string::replace
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

	template <> inline std::optional<std::string> string::compress<string::CompressMethod::Gzip>
		(const std::string& data)
	{
		Logger::Guard log;
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
	}
	template <> inline std::optional<std::string> string::compress<string::CompressMethod::Deflated>
		(const std::string& data)
	{
		Logger::Guard log;
		try
		{
			std::stringstream compressed;
			std::stringstream origin(std::move(data));
			boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
			out.push(boost::iostreams::zlib_compressor(boost::iostreams::zlib::best_compression));
			out.push(origin);
			boost::iostreams::copy(out, compressed);
			return std::move(compressed).str();
		}
		catch (...)
		{
			log.log<Logger::Level::Error>("deflated failed");
			return {};
		}
	}
	template <> inline std::optional<std::string> string::compress<string::CompressMethod::Brotli>
		(const std::string& data)
	{
		Logger::Guard log;
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
					(instance, BROTLI_OPERATION_FINISH, &available_in, &next_in, &available_out, &next_out, nullptr);
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
	template <> inline std::optional<std::string> string::decompress<string::CompressMethod::Gzip>
		(const std::string& data)
	{
		Logger::Guard log;
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
	}
	template <> inline std::optional<std::string> string::decompress<string::CompressMethod::Deflated>
		(const std::string& data)
	{
		Logger::Guard log;
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
	}
	template <> inline std::optional<std::string> string::decompress<string::CompressMethod::Brotli>
		(const std::string& data)
	{
		Logger::Guard log;
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
