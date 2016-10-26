#pragma once
#include "FilmwebSymbol.h"
#include <string>

namespace Filmweb {
	struct FILMWEB_API SearchResult {
		char type_;
		int id_;
		std::wstring name_;
		int year_;
	};
}