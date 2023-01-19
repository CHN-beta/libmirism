# include <mirism/detail_/utility/http.hpp>
# include <mirism/detail_/utility/logger.tpp>

namespace mirism::http
{
	const Method_t Method_t::Connect{"CONNECT", false};
	const Method_t Method_t::Delete{"DELETE", true};
	const Method_t Method_t::Get{"GET", false};
	const Method_t Method_t::Head{"HEAD", false};
	const Method_t Method_t::Options{"OPTIONS", false};
	const Method_t Method_t::Patch{"PATCH", true};
	const Method_t Method_t::Post{"POST", true};
	const Method_t Method_t::Put{"PUT", true};
	const Method_t Method_t::Trace{"TRACE", false};

	std::pair<ReadResult, std::optional<std::string>> ReadWholeBody
		(Body_t body, std::shared_ptr<Atomic<bool>> cancelled, std::chrono::steady_clock::duration timeout)
	{
		Logger::Guard log{body, cancelled, timeout};
		if (!body || !cancelled)
		{
			log.log<Logger::Level_t::Error>("body or cancelled is null");
			return {ReadResult::OtherError, {}};
		}
		if (std::holds_alternative<Atomic<std::string>>(*body))
		{
			log.log<Logger::Level_t::Info>("body is std::string");
			if (*cancelled)
			{
				log.log<Logger::Level_t::Info>("cancelled");
				return {ReadResult::Cancelled, {}};
			}
			else
				return {ReadResult::Success, std::get<Atomic<std::string>>(*body).get()};
		}
		else if (std::holds_alternative<Atomic<std::deque<std::optional<std::string>>>>(*body))
		{
			std::string result;
			auto deque = std::get<Atomic<std::deque<std::optional<std::string>>>>(*body);
			while (true)
			{
				// 等待 一段时间，直到 deque 中有项目，或超时。
				// 检查 cancelled。如果为 true，按照取消处理。
				// 否则，按照超时或读取成功处理。
				auto lock = deque.lock([](const auto& deque){return !deque.empty();}, timeout);
				if (*cancelled)
				{
					log.log<Logger::Level_t::Info>("cancelled");
					return {ReadResult::Cancelled, {}};
				}
				else if (!lock)
				{
					log.log<Logger::Level_t::Info>("timeout");
					return {ReadResult::Timeout, {}};
				}
				else
					while (!(*lock)->empty())
					{
						auto part = (*lock)->front();
						(*lock)->pop_front();
						if (!part)
						{
							log.log<Logger::Level_t::Info>("end of body");
							return {ReadResult::Success, result};
						}
						else
							result += *part;
					}
			}
		}
		else [[unlikely]]
		{
			log.log<Logger::Level_t::Error>("variant error");
			return {ReadResult::OtherError, {}};
		}
	}
	cppcoro::generator<std::pair<ReadResult, std::optional<std::string>>> ReadNextBodyPart
		(Body_t body, std::shared_ptr<Atomic<bool>> cancelled, std::chrono::steady_clock::duration timeout)
	{
		Logger::Guard log{body, cancelled, timeout};
		if (!body || !cancelled)
		{
			log.log<Logger::Level_t::Error>("body or cancelled is null");
			co_yield {ReadResult::OtherError, {}};
		}
		else if (std::holds_alternative<Atomic<std::string>>(*body))
		{
			log.log<Logger::Level_t::Info>("body is std::string");
			if (*cancelled)
			{
				log.log<Logger::Level_t::Info>("cancelled");
				co_yield {ReadResult::Cancelled, {}};
			}
			else
			{
				co_yield {ReadResult::Success, std::get<Atomic<std::string>>(*body).get()};
				co_yield {ReadResult::EndOfFile, {}};
			}
		}
		else if (std::holds_alternative<Atomic<std::deque<std::optional<std::string>>>>(*body))
		{
			auto deque = std::get<Atomic<std::deque<std::optional<std::string>>>>(*body);
			while (true)
			{
				auto lock = deque.lock([](const auto& deque){return !deque.empty();}, timeout);
				if (*cancelled)
				{
					log.log<Logger::Level_t::Info>("cancelled");
					lock.reset();
					co_yield {ReadResult::Cancelled, {}};
					break;
				}
				else if (!lock)
				{
					log.log<Logger::Level_t::Info>("timeout");
					co_yield {ReadResult::Timeout, {}};
				}
				else
				{
					auto part = (*lock)->front();
					(*lock)->pop_front();
					lock.reset();
					if (!part)
					{
						log.log<Logger::Level_t::Info>("end of body");
						co_yield {ReadResult::EndOfFile, {}};
						break;
					}
					else
						co_yield {ReadResult::Success, part};
				}
			}
		}
		else [[unlikely]]
		{
			log.log<Logger::Level_t::Error>("variant error");
			co_yield {ReadResult::OtherError, {}};
		}
	}
}
