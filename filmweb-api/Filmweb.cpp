#include "Filmweb.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <algorithm>
#include "curl/curl.h"
#include "md5.h"
#include "SearchConverter.h"
#include "DataConverter.h"

namespace Filmweb {
	// Convert a wide Unicode string to an UTF8 string
	std::string utf8_encode(const std::wstring &wstr)
	{
		if (wstr.empty()) return std::string();
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// Convert an UTF8 string to a wide Unicode String
	std::wstring utf8_decode(const std::string &str)
	{
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	static std::string url_encode(const std::string &value) {
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;

		for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
			std::string::value_type c = (*i);

			// Keep alphanumeric and other accepted characters intact
			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}

			// Any other characters are percent-encoded
			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int((unsigned char)c);
			escaped << std::nouppercase;
		}

		return escaped.str();
	}

	Filmweb::Filmweb(const WConfig &conf)
		: httpProxyTunel_(false)
	{

		auto searchHost = conf[L"searchHost"];
		if (searchHost)
			searchHost_ = searchHost;

		auto dataHost = conf[L"dataHost"];
		if (dataHost)
			dataHost_ = dataHost;

		auto proxyHost = conf[L"proxyHost"];
		if (proxyHost)
			proxyHost_ = proxyHost_;

		auto proxyUser = conf[L"proxyUser"];
		if (proxyUser)
			proxyUser_ = proxyUser;

		auto httpProxyTunel = conf[L"httpProxyTunel"];
		if (httpProxyTunel) {
			std::wstring data = httpProxyTunel;
			std::transform(data.begin(),
				data.end(),
				data.begin(),
				::tolower);
			httpProxyTunel_ = data == L"true";
		}

		auto repeating = conf[L"repeating"];
		if (repeating)
			repeating_ = std::stoi(repeating);

		auto generalTimeout = conf[L"generalTimeout"];
		if (generalTimeout)
			generalTimeout_ = std::stoi(generalTimeout);

		auto dnsCacheTimeout = conf[L"dnsCacheTimeout"];
		if (dnsCacheTimeout)
			dnsCacheTimeout_ = std::stoi(dnsCacheTimeout);

		auto expect100Timeout = conf[L"expect100TimeoutMS"];
		if (expect100Timeout)
			expect100Timeout_ = std::stoi(expect100Timeout);

		auto connectionTimeout = conf[L"connectionTimeout"];
		if (connectionTimeout)
			connectionTimeout_ = std::stoi(connectionTimeout);

		curl_global_init(CURL_GLOBAL_DEFAULT);
	}

	Filmweb::~Filmweb() {
		curl_global_cleanup();
	}

	static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
		size_t realsize = size * nmemb;
		std::string& bufor = *((std::string*)userp);
		bufor.reserve(bufor.size() + realsize);
		char* content = (char*)contents;
		for (size_t i = 0; i < realsize; ++i)
			bufor.push_back(content[i]);
		return realsize;
	}

	void Filmweb::setError(int numer, const std::wstring& komunikat) {
		if (!komunikat.empty()) {
			errorNo_ = numer;
			errorStr_ = komunikat;
		}
		else {
			errorNo_ = 0;
			errorStr_.clear();
		}
	}

	bool Filmweb::send(const std::string& method, std::string& out) {
		for (int iteration = repeating_; iteration > 0; --iteration) {
			CURL* curl = curl_easy_init();
			if (curl) {
				//headers = curl_slist_append(headers, "User-Agent: Mozilla / 5.0 (Linux; Android 4.1.1; Galaxy Nexus Build / JRO03C) AppleWebKit / 535.19 (KHTML, like Gecko) Chrome / 18.0.1025.166 Mobile Safari / 535.19");

				curl_easy_setopt(curl, CURLOPT_URL, method.c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&out);

				// Ustawienia proxy
				if (!proxyHost_.empty()) {
					curl_easy_setopt(curl, CURLOPT_PROXY, proxyHost_);
					curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, proxyUser_);
					if (httpProxyTunel_)
						curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
				}

				curl_easy_setopt(curl, CURLOPT_TIMEOUT, generalTimeout_);
				curl_easy_setopt(curl, CURLOPT_EXPECT_100_TIMEOUT_MS, expect100Timeout_);
				curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, dnsCacheTimeout_);
				curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connectionTimeout_);

				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

				CURLcode res = curl_easy_perform(curl);
				if (res != CURLE_OK) {
					auto desc = curl_easy_strerror(res);
					setError(res, utf8_decode(desc ? desc : ""));
					curl_easy_cleanup(curl);
					continue;
				}
				//Czyszczenie
				curl_easy_cleanup(curl);
				errorNo_ = 0;
				//errorStr_.clear();
				break;
			}
			if (out.empty()) {
				setError(92, L"No data received");
				continue;
			}
		}

		if (errorNo_ != 0)
			return false;

		if (out.empty()) {
			setError(92, L"No data received");
			return false;
		}
		return true;
	}

	int Filmweb::getErrorNo() const {
		return errorNo_;
	}

	const std::wstring& Filmweb::getErrorStr() const {
		return errorStr_;
	}

	bool Filmweb::getFilmInfoFull(const SearchResult& element, Film& film) {
		std::stringstream sMethod, sQuery;
		std::string bufor;
		sMethod << "getFilmInfoFull [" << element.id_ << "]\\n";
		sQuery << utf8_encode(dataHost_) << "/api?methods=" << url_encode(sMethod.str()) << "&signature=1.0,";
		sMethod << "androidqjcGhW2JnvGT9dfCt3uT_jozR3s";
		sQuery << ::md5(sMethod.str()) << "&version=1.0&appId=android";
		//todo dopiero tutaj konwertuj.
		if (!send(sQuery.str(), bufor))
			return false;

		DataConverter conv;
		conv.convertResponse(utf8_decode(bufor), film);
		FILE *fp;
		fopen_s(&fp, "returnData.txt", "wb");
		fwrite(bufor.data(), sizeof(char), bufor.size(), fp);
		fclose(fp);
		return true;
	}

	bool Filmweb::getSearch(const std::wstring & text, std::vector<SearchResult>& result) {
		std::wstringstream sQuery;
		std::string bufor;
		sQuery << searchHost_ << text;
		if (!send(utf8_encode(sQuery.str()), bufor))
			return false;

		SearchConverter conv;
		return conv.convertResponse(utf8_decode(bufor),result);
	}

	/*std::string query = "api?methods=" + url_encode(method) + "&signature=1.0," + ::md5(std::string(method) + "androidqjcGhW2JnvGT9dfCt3uT_jozR3s") + "&version=1.0&appId=android";
	method = query.c_str();
	std::string serialized;
	if (method == NULL) {
	setError(91, "Method name is null.");
	return false;
	}*/
}
