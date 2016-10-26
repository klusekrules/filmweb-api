#include "SearchConverter.h"

namespace Filmweb {

	bool SearchConverter::concatToVector(const std::wstring& data, const std::wstring& endLines, std::vector<std::wstring>& table) {
		auto pos = std::string::npos;

		if (data.empty())
			return false;

		do {
			auto begin = pos != std::wstring::npos ? pos + endLines.size() : 0;
			pos = data.find(endLines, begin);
			auto sub = data.substr(begin, pos == std::wstring::npos ? pos : pos - begin);
			table.emplace_back(std::move(sub));
		} while (pos != std::wstring::npos && data.size() > pos + endLines.size());

		return true;
	}

	bool SearchConverter::convertToSearchResult(const std::wstring& line, SearchResult& out) {
		std::vector<std::wstring> table;

		if (!concatToVector(line, endAttr, table))
			return false;

		if (table.empty())
			return false;

		if (table.size() >= 1)
			out.type_ = table[0][0];

		if (table.size() >= 2)
			out.id_ = stoi(table[1]);

		if (table.size() >= 4)
			out.name_ = table[3];

		if (table.size() >= 7)
			out.year_ = stoi(table[6]);

		return true;
	}

	bool SearchConverter::convertResponse(const std::wstring& data, std::vector<SearchResult>& table) {
		std::vector<std::wstring> lines;

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