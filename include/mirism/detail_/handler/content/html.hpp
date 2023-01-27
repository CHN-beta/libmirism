// a simple html parser
struct Node
{
	std::string name;
	std::deque<std::pair<std::string, std::optional<std::string>>> attributes;
	std::deque<std::variant<std::string, std::shared_ptr<Node>>> children;
};

struct html
{
	std::string header;
	std::deque<std::shared_ptr<Node>> nodes;
};

// parse std::string to html
html parse(std::string html_string)
{
	html result;
	for (std::size_t i = 0; i < html_string.size();)
	{
		if (html_string[i] == '<')
		{
			if (html_string[i + 1] == '!')
			{
				i = html_string.find('>', i + 1);
				result.header = html_string.substr(0, i + 1);
			}
			else if (html_string[i + 1] == '/')
			{
				i = html_string.find('>', i + 1);
			}
			else
			{
				std::size_t end = html_string.find('>', i + 1);
				std::string node_string = html_string.substr(i + 1, end - i - 1);
				std::string name;
				std::deque<std::pair<std::string, std::optional<std::string>>> attributes;
				std::deque<std::variant<std::string, std::shared_ptr<Node>>> children;
				for (std::size_t j = 0; j < node_string.size();)
				{
					if (node_string[j] == ' ')
					{
						++j;
					}
					else if (node_string[j] == '/')
					{
						j = node_string.size();
					}
					else if (node_string[j] == '>')
					{
						j = node_string.size();
					}
					else
					{
						std::size_t end = node_string.find_first_of(" /=", j);
						std::string name = node_string.substr(j, end - j);
						if (node_string[end] == ' ')
						{
							attributes.emplace_back(name, std::nullopt);
							j = end + 1;
						}
						else if (node_string[end] == '=')
						{
							std::size_t end2 = node_string.find_first_of("\"'", end + 1);
							std::string value = node_string.substr(end + 2, end2 - end - 2);
							attributes.emplace_back(name, value);
							j = end2 + 1;
						}
						else
						{
							attributes.emplace_back(name, std::nullopt);
							j = end;
						}
					}
				}
				std::shared_ptr<Node> node = std::make_shared<Node>(Node{name, attributes
}