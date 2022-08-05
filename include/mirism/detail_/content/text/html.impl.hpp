# pragma once
# include <mirism/detail_/content/text/html.hpp>

namespace mirism::content::text
{
	inline Html::Document Html::deserialize(const std::string& document_serialized)
	{
		auto read_one_word = [](const std::string& s) -> cppcoro::generator<std::string_view>
		{
			auto it1 = s.begin(), it2 = s.begin();
			do
			{
				while (it2 != s.end() && !std::set<char>{' ', '\t', '\n', '\r', '=', '"', '<', '>', '/'}.contains(*it2))
					++it2;
				if (it1 == it2)
					it2++;
				co_yield {it1, it2};
				it1 = it2;
			}
			while (it1 != s.end());
		};
		auto lowwer_case = [](std::string s) -> std::string
		{
			for (auto& c : s)
				if (c >= 'A' && c <= 'Z')
					c += 'a' - 'A';
			return s;
		};
		Document document;
		enum status
		{
			waiting_for_doctype,					// |<!DOCTYPE html xxx...
			waiting_for_doctype_name,				// <|!DOCTYPE html xxx...
			waiting_for_doctype_attribute,			// <!DOCTYPE| html xxx...
			waiting_for_tag,						// |<html xxx=xxx...
			waiting_for_tag_name,					// <|div xxx=xxx...
			waiting_for_tag_attribute_name,			// <div| xxx=xxx..., also maybe "=" or />
			waiting_for_tag_attribute_value,		// <div xxx=|xxx...
			waiting_for_tag_attribute_value_quoted,	// <div xxx="|xxx"...
			waiting_for_tag_close,					// <div xxx /|>
			waiting_for_tag_pair,					// <div xxx> |xxx xxx </div>
			waiting_for_tag_pair_name,				// <div xxx> xxx xxx </|div>
			waiting_for_tag_pair_close,				// <div xxx> xxx xxx </div|>
			waiting_for_text,						// <p>| a is b </p>
			waiting_for_tag_or_text,				// |<div xxx=xxx... or <p>| a is b ...
			waiting_for_text_string,				// <p>"| a is b"</p>
		}
		std::unordered_set<std::string> known_tags = {};
		std::unordered_set<std::string> tag_allow_text = {};
		std::unordered_set<std::string> tag_allow_children = {};
		std::unordered_set<std::string> tag_no_pair = {};
		std::vector<status> status_stack{waiting_for_doctype};
		std::observer_ptr<Node> current_node;
		try
		{
			for (auto word : read_one_word(document_serialized))
			{
				if (status_stack.empty()) [[unlikely]]
					throw std::runtime_error{"status stack is empty"};
				else if (status_stack.back() == waiting_for_doctype)
				{
					if (std::set<std::string>{" ", "\t", "\n", "\r"}.contains(word))
						continue;
					else if (word == "<")
						status_stack.back() = waiting_for_doctype_name;
					else
						throw std::runtime_error{"expected \"<\", get \"{}\""_f(word)};
				}
				else if (status_stack.back() == waiting_for_doctype_name)
				{
					if (std::set<std::string>{" ", "\t", "\n", "\r"}.contains(word))
						continue;
					else if (lowwer_case(word) == "!doctype")
						status_stack.back() = waiting_for_doctype_attribute;
					else
						throw std::runtime_error{"expected \"!doctype\", get \"{}\""_f(word)};
				}
				else if (status_stack.back() == waiting_for_doctype_attribute)
				{
					if (std::set<std::string>{" ", "\t", "\n", "\r"}.contains(word))
						continue;
					else if (word == ">")
						status_stack.back() = waiting_for_tag;
					else
						document.Doctype.push_back(word);
				}
				else if (status_stack.back() == waiting_for_tag)
				{
					if (std::set<std::string>{" ", "\t", "\n", "\r"}.contains(word))
						continue;
					else if (word == "<")
						status_stack.push_back(waiting_for_tag_name);
					else
						throw std::runtime_error{"expected \"<\", get \"{}\""_f(word)};
				}
				else if (status_stack.back() == waiting_for_tag_name)
				{
					if (std::set<std::string>{" ", "\t", "\n", "\r"}.contains(word))
						continue;
					else if (std::set<std::string>{"=", "\"", "<", ">"}.contains(word))
						throw std::runtime_error{"expected tag name, get \"{}\""_f(word)};
					else if (word == "/")
						status_stack.back() = waiting_for_tag_pair_name;
					else
					{
						auto tag_name = lowwer_case(word);
						if (!known_tags.contains(tag_name))
							throw std::runtime_error{"unknown tag \"{}\""_f(word)};
						else if (current_node)
						{
							current_node->Children.emplace_back(Node{.Tag = tag_name, .Parent = current_node});
							current_node = std::get_if<Node>(&current_node->Children.back());
						}
						else
						{
							document.Children.emplace_back(Node{});
							current_node = std::get_if<Node>(&document.Children.back());
						}
						status_stack.back() = waiting_for_tag_attribute_name;
					}
				}
				else if (status_stack.back() == waiting_for_tag_attribute_name)
				{
					if (std::set<std::string>{" ", "\t", "\n", "\r"}.contains(word))
						continue;
					else if (std::set<std::string>{, "\"", "<"}.contains(word))
						throw std::runtime_error{"expected tag attribute name, get \"{}\""_f(word)};
					else if (word == "=")
					{
						if (current_node->Attributes.empty())
							throw std::runtime_error{"get \"=\" but do not fond attribute name"};
						else
							status_stack.back() = waiting_for_tag_attribute_value;
					}
					else if (word == "/")
						status_stack.back() = waiting_for_tag_close;
					else if (word == ">")
					{
						if (tags_no_pair.contains(current_node->Tag))
							status_stack.pop_back();
						else
							status_stack.back() = waiting_for_tag_pair;
						if (tags_allow_text.contains(current_node->Tag))
						{
							if (tag_allow_children)
						}
							status_stack.back() = waiting_for_text;
						else
							status_stack.back() = waiting_for_tag_pair_name;
					}
					else
						current_node->Attributes.emplace_back(word, nullopt);
				}
				else if (status_stack.back() == waiting_for_tag_attribute_value)
				{
					if (std::set<std::string>{" ", "\t", "\n", "\r"}.contains(word))
						continue;
					else if (std::set<std::string>{"=", "<", ">", "/"}.contains(word))
						throw std::runtime_error{"expected tag attribute value, get \"{}\""_f(word)};
					else if (word == "\"")
					{
						current_node->Attributes.back().Value = "";
						status_stack.back() = waiting_for_tag_attribute_value_quoted;
					}
					else
						current_node->Attributes.back().Value = word;
				}
				else if (status_stack.back() == waiting_for_tag_attribute_value_quoted)
				{
					if (word == "\"")
						status_stack.back() = waiting_for_tag_attribute_name;
					else
						current_node->Attributes.back().Value += word;
				}
				else if (status_stack.back() == waiting_for_tag_close)
				{
					if (std::set<std::string>{" ", "\t", "\n", "\r"}.contains(word))
						continue;
					else if (word == ">")
					{
						if 
					}
						status_stack.back() = waiting_for_tag;
					else
						throw std::runtime_error{"expected \">\", get \"{}\""_f(word)};
				}
								else if (status_stack.back() == waiting_for_tag_or_text)
				{
					if (std::set<std::string>{" ", "\t", "\n", "\r"}.contains(word.front()))
						continue;
					else if (word == "<")
					{
						status_stack.back() = waiting_for_tag_name;
						if (current_node)
						{
							current_node->Children.emplace_back(Node{.Parent = current_node});
							current_node = std::get_if<Node>(&current_node->Children.back());
						}
						else
						{
							document.Children.emplace_back(Node{});
							current_node = std::get_if<Node>(&document.Children.back());
						}
					}
					else
					{
						if (current_node)
						{
							if
							(
								!current_node->Children.empty()
								&& std::holds_alternative<std::string>(current_node->Children.back().Value)
							)
								current_node->Children.back()
									= std::get<std::string>(current_node->Children.back()) + word;
							else
								current_node->Children.emplace_back(word);
						}
						else
							throw std::runtime_error{"text in root is not allowed."};
					}
				}
			}
		}

	}
}
