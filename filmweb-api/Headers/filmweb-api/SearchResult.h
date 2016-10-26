#pragma once
#include "FilmwebSymbol.h"
#include <string>

namespace Filmweb {
	struct FILMWEB_API SearchResult {
		char type_;
		int id_;
		std::wstring obraz_;
		std::wstring nazwaOryginalna_;
		std::wstring nazwaPolski_;
		std::wstring nazwaInna_;
		int year_;
		std::wstring wRolachGlownych_;
	};
}