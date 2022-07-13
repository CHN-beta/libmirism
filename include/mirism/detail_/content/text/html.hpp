# pragma once
# include <mirism/detail_/logger.hpp>

namespace mirism::content::text
{
	class Html
	{
		public: struct Attribute
		{
			std::string name;
			std::optional<std::string> value;
		};
		public: struct Node
		{
			std::string Tag;
			std::vector<Attribute> Attributes;
			std::vector<std::variant<Node, std::string>> Children;
			std::experimental::observer_ptr<Node> Parent;
		};
		public: struct Document
		{
			std::vector<std::string> Doctype;
			std::vector<Node> Nodes;
		};
		public: std::optioinal<Document> deserialize(const std::string& s);
		public: std::string serialize(const Node& node);
	};
}