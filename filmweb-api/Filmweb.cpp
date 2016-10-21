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

Filmweb::Filmweb(const Config &conf)
	: httpProxyTunel_(false)
{

	auto searchHost = conf["searchHost"];
	if (searchHost)
		searchHost_ = searchHost;

	auto dataHost = conf["dataHost"];
	if (dataHost)
		dataHost_ = dataHost;

	auto proxyHost = conf["proxyHost"];
	if (proxyHost)
		proxyHost_ = proxyHost_;

	auto proxyUser = conf["proxyUser"];
	if (proxyUser)
		proxyUser_ = proxyUser;

	auto httpProxyTunel = conf["httpProxyTunel"];
	if (httpProxyTunel) {
		std::string data = httpProxyTunel;
		std::transform(data.begin(),
			data.end(),
			data.begin(),
			::tolower);
		httpProxyTunel_ = data == "true";
	}

	auto repeating = conf["repeating"];
	if (repeating)
		repeating_ = std::stoi(repeating);

	auto generalTimeout = conf["generalTimeout"];
	if (generalTimeout)
		generalTimeout_ = std::stoi(generalTimeout);

	auto dnsCacheTimeout = conf["dnsCacheTimeout"];
	if (dnsCacheTimeout)
		dnsCacheTimeout_ = std::stoi(dnsCacheTimeout);

	auto expect100Timeout = conf["expect100TimeoutMS"];
	if (expect100Timeout)
		expect100Timeout_ = std::stoi(expect100Timeout);

	auto connectionTimeout = conf["connectionTimeout"];
	if (connectionTimeout)
		connectionTimeout_ = std::stoi(connectionTimeout);
	
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

Filmweb::~Filmweb() {
	curl_global_cleanup();
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	std::vector <char>& bufor = *((std::vector <char> *)userp);
	bufor.reserve(bufor.size() + realsize);
	char* content = (char*)contents;
	for (size_t i = 0; i < realsize; ++i)
		bufor.push_back(content[i]);
	return realsize;
}

void Filmweb::setError(int numer, const char* komunikat) {
	if (komunikat != NULL) {
		errorNo_ = numer;
		errorStr_ = komunikat;
	} else {
		errorNo_ = 0;
		errorStr_.clear();
	}
}

bool Filmweb::send(const char * method, std::vector <char>& out) {
	for (int iteration = repeating_; iteration >0; --iteration) {
		CURL* curl = curl_easy_init();
		if (curl) {
			//headers = curl_slist_append(headers, "User-Agent: Mozilla / 5.0 (Linux; Android 4.1.1; Galaxy Nexus Build / JRO03C) AppleWebKit / 535.19 (KHTML, like Gecko) Chrome / 18.0.1025.166 Mobile Safari / 535.19");
			
			curl_easy_setopt(curl, CURLOPT_URL, method);
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
				setError(res, curl_easy_strerror(res));
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
			setError(92, "No data received");
			continue;
		}
	}

	if (errorNo_ != 0)
		return false;

	if (out.empty()) {
		setError(92, "No data received");
		return false;
	}
	return true;
}

int Filmweb::getErrorNo() const {
	return errorNo_;
}

const char* Filmweb::getErrorStr() const {
	return errorStr_.c_str();
}

bool Filmweb::getDetails(int id){
	std::stringstream sMethod, sQuery;
	std::vector<char> bufor;
	sMethod << "getFilmInfoFull [" << id << "]\\n";
	sQuery << dataHost_ << "/api?methods=" << url_encode(sMethod.str()) << "&signature=1.0,";
	sMethod << "androidqjcGhW2JnvGT9dfCt3uT_jozR3s";
	sQuery << ::md5(sMethod.str()) << "&version=1.0&appId=android";
	if (!send(sQuery.str().c_str(), bufor))
		return false;
	FILE *fp;
	fopen_s(&fp, "returnData.txt", "wb");
	fwrite(bufor.data(), sizeof(char), bufor.size(), fp);
	fclose(fp);
	return true;
}

bool Filmweb::getSearch(std::string & text){
	std::stringstream sQuery;
	std::vector<char> bufor;
	sQuery << searchHost_ << url_encode(text);
	if (!send(sQuery.str().c_str(), bufor))
		return false;
	FILE *fp;
	fopen_s(&fp, "returnData.txt", "wb");
	fwrite(bufor.data(), sizeof(char), bufor.size(), fp);
	fclose(fp);
	return false;
}

/*std::string query = "api?methods=" + url_encode(method) + "&signature=1.0," + ::md5(std::string(method) + "androidqjcGhW2JnvGT9dfCt3uT_jozR3s") + "&version=1.0&appId=android";
method = query.c_str();
std::string serialized;
if (method == NULL) {
setError(91, "Method name is null.");
return false;
}*/
