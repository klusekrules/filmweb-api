#include "SearchConverter.h"

namespace Filmweb {

	bool SearchConverter::concatToVector(const std::string& data, const std::string& endLines, std::vector<std::string>& table) {
		auto pos = std::string::npos;

		if (data.empty())
			return false;

		do {
			auto begin = pos != std::string::npos ? pos + endLines.size() : 0;
			pos = data.find(endLines, begin);
			auto sub = data.substr(begin, pos == std::string::npos ? pos : pos - begin);
			table.emplace_back(std::move(sub));
		} while (pos != std::string::npos && data.size() > pos + endLines.size());

		return true;
	}

	bool SearchConverter::convertToSearchResult(const std::string& line, SearchResult& out) {
		std::vector<std::string> table;

		if (!concatToVector(line, endAttr, table))
			return false;

		if (table.size() < 7)
			return false;

		out.type_ = table[0][0];
		out.id_ = stoi(table[1]);
		out.name_ = table[3];
		out.year_ = stoi(table[6]);

		return true;
	}

	bool SearchConverter::convertResponse(const std::string& data, std::vector<SearchResult>& table) {
		std::vector<std::string> lines;

		if (!concatToVector(data, endLine, lines))
			return false;

		if (lines.empty())
			return false;

		for (auto& line : lines) {
			SearchResult temp;
			if (!convertToSearchResult(line, temp))
				return false;
			table.emplace_back(std::move(temp));
		}

		return true;
	}
}