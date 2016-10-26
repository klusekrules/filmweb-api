#pragma once
#include "FilmwebSymbol.h"
#include "SearchResult.h"
#include "Film.h"
#include "Config\Config.h"
#include <vector>

namespace Filmweb {
	class FILMWEB_API Filmweb {
	public:

		Filmweb(const WConfig &conf);
		~Filmweb();

		bool getFilmInfoFull(const SearchResult& element, Film& film);
		bool getSearch(const std::wstring& text, std::vector<SearchResult>& result);

		int getErrorNo() const;

		const std::wstring& getErrorStr() const;

	private:
		bool send(const std::string& method, std::string& out);

		void setError(int, const std::wstring& );

		std::wstring searchHost_;
		std::wstring dataHost_;

		std::wstring proxyHost_;
		std::wstring proxyUser_;
		bool httpProxyTunel_;

		int repeating_;
		int generalTimeout_;
		int dnsCacheTimeout_;
		int expect100Timeout_;
		int connectionTimeout_;

		int errorNo_;
		std::wstring errorStr_;
	};
}
