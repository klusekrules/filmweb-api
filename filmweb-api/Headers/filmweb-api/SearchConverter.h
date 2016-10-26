#pragma once
#include "FilmwebSymbol.h"
#include "SearchResult.h"
#include <string>
#include <vector>

namespace Filmweb {
	class SearchConverter {
		const std::string endLine = "\\a";
		const std::string endAttr = "\\c";

		static bool concatToVector(const std::string& data, const std::string& endLines, std::vector<std::string>& table);
		bool convertToSearchResult(const std::string& line, SearchResult& out);

	public:
		SearchConverter() = default;
		~SearchConverter() = default;

		bool convertResponse(const std::string& data, std::vector<SearchResult>& table);
	};
}