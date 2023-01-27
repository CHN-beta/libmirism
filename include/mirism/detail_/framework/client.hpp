# pragma once
# include <mirism/utility.hpp>

namespace mirism::client
{
	class ClassBase
	{
		public: virtual ~ClassBase() = default;

		// fetch a request from upstream
		public: virtual std::unique_ptr<http::ClassResponse> operator()
			(std::unique_ptr<http::ClassRequest> request) = 0;
	};
}
