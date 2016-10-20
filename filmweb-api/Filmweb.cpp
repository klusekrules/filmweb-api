#include "Filmweb.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <functional>
#include "curl/curl.h"
#include "md5.h"

std::string url_encode(const std::string &value) {
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

Filmweb::Filmweb()
	: host_(NULL), errorStr_(NULL), proxyHost_(NULL), proxyUser_(NULL), httpProxyTunnel_(NULL)
{
	logPackets_ = true;
	logSSLPackets_ = true;
	//size_t len = strlen("http://www.filmweb.pl/search") + 1;
	size_t len = strlen("https://ssl.filmweb.pl") + 1;
	host_ = new char[len];
	//strcpy(host_, "http://www.filmweb.pl/search");
	strcpy(host_, "https://ssl.filmweb.pl");
	proxyHost_ = NULL;
	proxyUser_ = NULL;
	httpProxyTunnel_ = false;
	repeating_ = 1;
	generalTimeout_ = 10;
	dnsCacheTimeout_ = 10;
	expect100TimeoutMS_ = 10;
	connectionTimeout_ = 10;
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

Filmweb::~Filmweb() {
	if (host_ != NULL)
		delete[] host_;
	if (errorStr_ != NULL)
		delete[] errorStr_;
	if (proxyHost_ != NULL)
		delete[] proxyHost_;
	if (proxyUser_ != NULL)
		delete[] proxyUser_;
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

static void dump(const char *text, FILE *stream, unsigned char *ptr, size_t size, bool detail) {
	size_t i;
	size_t c;
	unsigned int width = 0x10;

	fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n", text, (long)size, (long)size);

	if (!detail)
		return;

	for (i = 0; i<size; i += width)
	{
		fprintf(stream, "%4.4lx: ", (long)i);

		/* show hex to the left */
		for (c = 0; c < width; c++)
		{
			if (i + c < size)
				fprintf(stream, "%02x ", ptr[i + c]);
			else
				fputs(" ", stream);
		}

		/* show data on the right */
		for (c = 0; (c < width) && (i + c < size); c++)
			fputc((ptr[i + c] >= 0x20) && (ptr[i + c]<0x80) ? ptr[i + c] : '.', stream);

		fputc('\n', stream); /* newline */
	}
}

static int my_trace(CURL*, curl_infotype type, char *data, size_t size, void *userp, bool detail) {
	const char *text;
	FILE* fp = (FILE*)userp;

	bool bDetail = true;

	switch (type)
	{
	case CURLINFO_TEXT:
		fprintf(fp, "== Info: %s", data);
	default: 
		return 0;

	case CURLINFO_HEADER_OUT:
		text = "=> Send header";
		break;
	case CURLINFO_DATA_OUT:
		text = "=> Send data";
		break;
	case CURLINFO_SSL_DATA_OUT:
		bDetail = detail;
		text = "=> Send SSL data";
		break;
	case CURLINFO_HEADER_IN:
		text = "<= Recv header";
		break;
	case CURLINFO_DATA_IN:
		text = "<= Recv data";
		break;
	case CURLINFO_SSL_DATA_IN:
		bDetail = detail;
		text = "<= Recv SSL data";
		break;
	}

	dump(text, fp, (unsigned char *)data, size, bDetail);
	return 0;
}

static int traceALL(CURL* curl, curl_infotype type, char *data, size_t size, void *userp) {
	return my_trace(curl, type, data, size, userp, true);
}

static int traceWithoutSSL(CURL* curl, curl_infotype type, char *data, size_t size, void *userp) {
	return my_trace(curl, type, data, size, userp, false);
}


void Filmweb::setError(int numer, const char* komunikat) {
	if (komunikat != NULL) {
		errorNo_ = numer;
		if (errorStr_ != NULL) {
			delete[] errorStr_;
		}
		errorStr_ = new char[strlen(komunikat) + 1];
		strcpy(errorStr_, komunikat);
	}
	else {
		errorNo_ = 0;
		if (errorStr_ != NULL) {
			delete[] errorStr_;
		}
		errorStr_ = NULL;
	}
}

bool Filmweb::send(const char * method) {

	std::string query = "api?methods=" + url_encode(method) + "&signature=1.0," + ::md5(std::string(method) + "androidqjcGhW2JnvGT9dfCt3uT_jozR3s") + "&version=1.0&appId=android";
	method = query.c_str();
	std::string serialized;
	if (method == NULL) {
		setError(91, "Method name is null.");
		return false;
	}

	FILE* fp;
	fp = fopen("output.log", "a+");


	std::vector <char> bufor;

	for (int iteration = repeating_; iteration >0; --iteration) {
		CURL* curl = curl_easy_init();
		if (curl) {
			//headers = curl_slist_append(headers, "User-Agent: Mozilla / 5.0 (Linux; Android 4.1.1; Galaxy Nexus Build / JRO03C) AppleWebKit / 535.19 (KHTML, like Gecko) Chrome / 18.0.1025.166 Mobile Safari / 535.19");
			std::string s;
			s.append(host_);
			s.append("/");
			s.append(method);

			curl_easy_setopt(curl, CURLOPT_URL, s.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&bufor);

			// Ustawienia proxy
			if (proxyHost_ != NULL) {
			curl_easy_setopt(curl, CURLOPT_PROXY, proxyHost_);
			curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, proxyUser_);
			if (httpProxyTunnel_)
			curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
			}

			curl_easy_setopt(curl, CURLOPT_TIMEOUT, generalTimeout_);
			curl_easy_setopt(curl, CURLOPT_EXPECT_100_TIMEOUT_MS, expect100TimeoutMS_);
			curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, dnsCacheTimeout_);
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connectionTimeout_);

			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

			CURLcode res = curl_easy_perform(curl);
			if (res != CURLE_OK) {
				setError(res, curl_easy_strerror(res));

				auto str = std::string("Failed: ") + curl_easy_strerror(res) + "\n";
				fwrite(str.c_str(), str.length(), 1, fp);
				curl_easy_cleanup(curl);
				continue;
			}
			//Czyszczenie
			curl_easy_cleanup(curl);
			errorNo_ = 0;
			//errorStr_.clear();
			break;
		}
		if (bufor.empty()) {
			setError(92, "No data received");
			continue;
		}

	}

	if (errorNo_ != 0) {
		return false;
	}

	if (bufor.empty()) {
		setError(92, "No data received");
		return false;
	}

	//Przygotowanie do parsowania odebranych danych
	fwrite(&bufor[0], bufor.size(), 1, fp);
	return true;
}

int Filmweb::getErrorNo() const {
	return errorNo_;
}

const char* Filmweb::getErrorStr() const {
	return errorStr_;
}