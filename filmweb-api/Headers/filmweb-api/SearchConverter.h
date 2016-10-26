#pragma once
#include "FilmwebSymbol.h"
#include "SearchResult.h"
#include <string>
#include <vector>

namespace Filmweb {
	class SearchConverter {
		const std::wstring endLine = L"\\a";
		const std::wstring endAttr = L"\\c";

		static bool concatToVector(const std::wstring& data, const std::wstring& endLines, std::vector<std::wstring>& table);
		bool convertToSearchResult(const std::wstring& line, SearchResult& out);
		bool typeTconv(const std::vector<std::wstring>& table, SearchResult& out);
		bool typeFconv(const std::vector<std::wstring>& table, SearchResult& out);

	public:
		SearchConverter() = default;
		~SearchConverter() = default;

		bool convertResponse(const std::wstring& data, std::vector<SearchResult>& table);
	};
}