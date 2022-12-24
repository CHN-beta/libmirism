# include <mirism/detail_/utility/http.hpp>

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
}
