#pragma once
#include "FilmwebSymbol.h"
#include "Film.h"
#include <string>
#include <vector>

namespace Filmweb {
	class DataConverter {
		const std::wstring separator = L",";
		const std::wstring nawiasOtwierajacy = L"[";
		const std::wstring nawiasZamykajacy = L"]";
		const std::wstring cudzyslow = L"\"";
		const std::wstring escapeChar = L"\\";

		static std::wstring extractData(const std::wstring& data, const std::wstring& prefix, const std::wstring& sufix);
		std::wstring extractField(const std::wstring& data, size_t& pos);
		bool concatToVector(const std::wstring& data, std::vector<std::wstring>& table);

	public:
		DataConverter() = default;
		~DataConverter() = default;

		bool convertResponse(const std::wstring& data, Film& film);
	};
}