# pragma once
# include <mirism/common.hpp>

namespace mirism::client
{
	class ::mirism::Pipe;

	// All the content here should be legal (e.g. Host only contains legal characters, and so on)
	// Besides, the following restrictions are imposed:
	// 	* Version should be std::nullopt or "1.0", "1.1", "2", "3";
	// 	* Host is in lower case and have no leading or trailing '.';
	// 	* Keys in Headers are in lower case;
	//  * Method is in upper case, and only these values are allowed:
	// 		"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH" 
	//  * For methods that do not allowed to have content, Body should be set as nullptr; if method is allowed to
	// 		have content, Body should be set as non-nullptr.
	struct Request
	{
		std::optional<std::string> Version;
		std::string Method;
		std::string Host;
		std::string Path;
		std::multimap<std::string, std::string> Headers;
		std::shared_ptr<Pipe> Body;
	};
}
