# include <boost/iostreams/filtering_streambuf.hpp>
# include <boost/iostreams/copy.hpp>
# include <boost/iostreams/filter/gzip.hpp>
# include <boost/iostreams/filter/zlib.hpp>
# include <brotli/encode.h>
# include <brotli/decode.h>
# include <mirism/detail_/utility/compression.hpp>
# include <mirism/detail_/utility/logger.tpp>

namespace mirism::string
{
	template <CompressMethod Method> std::optional<std::string> compress(const std::string& data)
	{
		Logger::Guard log;
		try
		{
			if constexpr (Method == CompressMethod::Gzip || Method == CompressMethod::Deflate)
			{
				std::stringstream compressed;
				std::stringstream origin{std::move(data)};
				boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
				if constexpr (Method == CompressMethod::Gzip)
					out.push
					(
						boost::iostreams::gzip_compressor
							(boost::iostreams::gzip_params(boost::iostreams::gzip::best_compression))
					);
				else if constexpr (Method == CompressMethod::Deflate)
					out.push(boost::iostreams::zlib_compressor(boost::iostreams::zlib::best_compression));
				else
					std::unreachable();
				out.push(origin);
				boost::iostreams::copy(out, compressed);
				return std::move(compressed).str();
			}
			else if constexpr (Method == CompressMethod::Brotli)
			{
				static_assert(sizeof(uint8_t) == sizeof(char));
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
			else
				std::unreachable();
		}
		catch (...)
		{
			log.log<Logger::Level::Error>("{} failed"_f(Method));
			return {};
		}
	}
	template std::optional<std::string> compress<CompressMethod::Gzip>(const std::string& data);
	template std::optional<std::string> compress<CompressMethod::Deflate>(const std::string& data);
	template std::optional<std::string> compress<CompressMethod::Brotli>(const std::string& data);
	template <CompressMethod Method> std::optional<std::string> decompress(const std::string& data)
	{
		Logger::Guard log;
		try
		{
			if constexpr (Method == CompressMethod::Gzip || Method == CompressMethod::Deflate)
			{
				std::stringstream decompressed;
				std::stringstream origin{std::move(data)};
				boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
				if constexpr (Method == CompressMethod::Gzip)
					out.push(boost::iostreams::gzip_decompressor{});
				else if constexpr (Method == CompressMethod::Deflate)
					out.push(boost::iostreams::zlib_decompressor{});
				else
					std::unreachable();
				out.push(origin);
				boost::iostreams::copy(out, decompressed);
				return std::move(decompressed).str();
			}
			else if constexpr (Method == CompressMethod::Brotli)
			{
				static_assert(sizeof(uint8_t) == sizeof(char));
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
			else
				std::unreachable();
		}
		catch (...)
		{
			log.log<Logger::Level::Error>("{} failed"_f(Method));
			return {};
		}
	}
	template std::optional<std::string> decompress<CompressMethod::Gzip>(const std::string& data);
	template std::optional<std::string> decompress<CompressMethod::Deflate>(const std::string& data);
	template std::optional<std::string> decompress<CompressMethod::Brotli>(const std::string& data);
}
