#pragma once
#include "FilmwebSymbol.h"

class FILMWEB_API Filmweb {
public:

	Filmweb();
	~Filmweb();

	bool send(const char * method);

	int getErrorNo() const;

	const char *getErrorStr() const;
private:

	void setError(int, const char*);
	
	char* host_;
	char* errorStr_;

	char* proxyHost_;
	char* proxyUser_;
	bool httpProxyTunnel_;

	int errorNo_;
	bool logPackets_;
	bool logSSLPackets_;
	int repeating_;
	unsigned int generalTimeout_;
	unsigned int dnsCacheTimeout_;
	unsigned int expect100TimeoutMS_;
	unsigned int connectionTimeout_;


	char* userName_;
	char* authKey_;
	char* storeId_;
	char* storePass_;
	int timeSession_;

};