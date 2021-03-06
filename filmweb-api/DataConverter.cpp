#include "DataConverter.h"

namespace Filmweb {
	
	std::wstring DataConverter::extractData(const std::wstring & data, const std::wstring& prefix, const std::wstring& sufix){
		auto begin = data.find(prefix);
		auto end = data.rfind(sufix);
		begin = begin == std::wstring::npos ? 0 : begin + prefix.size();
		return std::move(data.substr(begin, end == std::wstring::npos ? end : end - begin));
	}

	std::wstring DataConverter::extractField(const std::wstring& data, size_t& pos) {
		if (pos == std::wstring::npos)
			return std::wstring();

		int bracketLevel = 0;
		bool quotamark = false;
		bool endField = false;
		auto tempPos = pos;
		auto begin = pos;

		for (; tempPos < data.size() && !endField; ++tempPos) {
			switch (data[tempPos]) {
			case '"': quotamark = !quotamark;
				break;
			case '[': ++bracketLevel;
				break;
			case ']': --bracketLevel;
				break;
			case '\\': ++tempPos;
				break;
			case ',': if (bracketLevel == 0 && !quotamark) endField = true;
				break;
			}

			if (bracketLevel < 0)
				throw "Invalid data";
		}
	
		pos = tempPos >= data.size() ? std::wstring::npos : tempPos;
		return std::move(data.substr(begin, pos == std::wstring::npos ? pos : pos - begin - 1));
	}


	bool DataConverter::concatToVector(const std::wstring& data, std::vector<std::wstring>& table) {
		size_t pos = 0;
		if (data.empty())
			return false;
		while (pos != std::wstring::npos)
			table.emplace_back(extractField(data, pos));
		return true;
	}

	std::wstring DataConverter::removeEscapeChars(const std::wstring & data) {
		std::wstring result;
		result.reserve(data.size());
		auto pos = std::wstring::npos;
		decltype(pos) offset = 0;
		do {
			pos = data.find(escapeChar, offset);
			result.append(data, offset, pos == std::wstring::npos ? pos : pos - offset);
			offset = pos + escapeChar.size();
		} while (pos != std::wstring::npos);
		return result;
	}
	
	bool DataConverter::convertResponse(const std::wstring& data, Film& film) {
		std::vector<std::wstring> lines;

		if (!concatToVector(extractData(data,nawiasOtwierajacy,nawiasZamykajacy),lines))
			return false;
		if (lines.size() < 20)
			return false;
		film.tytulPolski_ = removeEscapeChars(extractData(lines[0], cudzyslow, cudzyslow));
		film.tytulOryginalny_ = removeEscapeChars(extractData(lines[1], cudzyslow, cudzyslow));
		film.ocena_ = std::stof(lines[2]);
		film.liczbaGlosow_ = std::stoi(lines[3]);
		film.gatunek_ = removeEscapeChars(extractData(lines[4], cudzyslow, cudzyslow));
		film.rokProdukcji_ = std::stoi(lines[5]);
		film.dlugosc_ = std::stoi(lines[6]);
		film.strona_ = removeEscapeChars(extractData(lines[8], cudzyslow, L"/discussion" + cudzyslow));
		film.plakat_ = removeEscapeChars(extractData(lines[11], cudzyslow, cudzyslow));
		film.premieraSwiat_ = removeEscapeChars(extractData(lines[13], cudzyslow, cudzyslow));
		film.premieraPolska_ = removeEscapeChars(extractData(lines[14], cudzyslow, cudzyslow));
		film.krajProdukcji_ = removeEscapeChars(extractData(lines[18], cudzyslow, cudzyslow));
		film.zarysFabu�y_ = removeEscapeChars(extractData(lines[19],cudzyslow,cudzyslow));
		return true;
	}
}