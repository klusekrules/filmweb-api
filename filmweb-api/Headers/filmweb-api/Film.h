#pragma once
#include "FilmwebSymbol.h"
#include <string>

namespace Filmweb {
	struct FILMWEB_API Film {
		Film() = default;
		~Film() = default;
		int id_;
		std::string tytulOryginalny_;
		std::string tytulPolski_;
		std::string gatunek_;
		std::string krajProdukcji_;
		std::string zarysFabu³y_;
		int liczbaGlosow_;
		float ocena_;
		std::string strona_;
		std::string plakat_;
		int rokProdukcji_;
		int dlugosc_;
	};
}
