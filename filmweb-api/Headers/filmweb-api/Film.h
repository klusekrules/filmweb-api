#pragma once
#include "FilmwebSymbol.h"
#include <string>

namespace Filmweb {
	struct FILMWEB_API Film {
		Film() = default;
		~Film() = default;
		int id_;
		std::wstring tytulOryginalny_;
		std::wstring tytulPolski_;
		std::wstring gatunek_;
		std::wstring krajProdukcji_;
		std::wstring zarysFabu³y_;
		int liczbaGlosow_;
		float ocena_;
		std::wstring strona_;
		std::wstring plakat_;
		int rokProdukcji_;
		int dlugosc_;
		std::wstring premieraSwiat_;
		std::wstring premieraPolska_;
		std::wstring wRolachGlownych_;
	};
}
