#pragma once
#include "FilmwebSymbol.h"
#include "SearchResult.h"
#include "Config\Config.h"
#include <vector>

namespace Filmweb {
	class FILMWEB_API Filmweb {
	public:

		Filmweb(const Config &conf);
		~Filmweb();

		bool getDetails(int id);
		bool getSearch(const std::string& text, std::vector<SearchResult>& result);

		int getErrorNo() const;

		const char *getErrorStr() const;

	private:
		bool send(const char * method, std::string& out);

		void setError(int, const char*);

		std::string searchHost_;
		std::string dataHost_;

		std::string proxyHost_;
		std::string proxyUser_;
		bool httpProxyTunel_;

		int repeating_;
		int generalTimeout_;
		int dnsCacheTimeout_;
		int expect100Timeout_;
		int connectionTimeout_;

		int errorNo_;
		std::string errorStr_;
	};
}
