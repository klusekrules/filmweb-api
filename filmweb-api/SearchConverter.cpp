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
		} while (pos != std::wstring::npos);

		return true;
	}


	bool SearchConverter::typeTconv(const std::vector<std::wstring>& table, SearchResult& out) {
		if (table.size() < 3)
			return false;
		
		out.type_ = static_cast<char>(table[0][0]);
		out.id_ = stoi(table[1]);
		out.nazwaOryginalna_ = table[2];

		return true;
	}
	
	bool SearchConverter::typeFconv(const std::vector<std::wstring>& table, SearchResult& out) {
		if (table.size() < 8)
			return false;

		out.type_ = static_cast<char>(table[0][0]);
		out.id_ = stoi(table[1]);
		out.obraz_ = table[2];
		out.nazwaOryginalna_ = table[3];
		out.nazwaPolski_ = table[4];
		out.nazwaInna_ = table[5];
		out.year_ = stoi(table[6]);
		out.wRolachGlownych_ = table[7];

		return true;
	}

	bool SearchConverter::convertToSearchResult(const std::wstring& line, SearchResult& out) {
		std::vector<std::wstring> table;

		if (!concatToVector(line, endAttr, table))
			return false;

		if (table.empty() || table[0].empty())
			return false;

		switch (table[0][0]) {
		case 't': return typeTconv(table, out);
		case 'f': return typeFconv(table, out);
		case 's': return typeFconv(table, out);
		case 'g': return typeFconv(table, out);
		}
		return false;
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