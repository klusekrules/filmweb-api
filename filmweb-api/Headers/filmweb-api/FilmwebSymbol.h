#pragma once
#pragma warning( disable : 4251 )
#pragma warning( disable : 4275 )
#ifdef _WIN32
#ifdef FILMWEB_EXPORT

#define FILMWEB_API __declspec(dllexport)
#define FILMWEB_EXPIMP_TEMPLATE

#elif defined(FILMWEB_IMPORT)

#define FILMWEB_API __declspec(dllimport)
#define FILMWEB_EXPIMP_TEMPLATE extern

#else

#define FILMWEB_EXPIMP_TEMPLATE
#define FILMWEB_API

#endif
#else

#define FILMWEB_EXPIMP_TEMPLATE
#define FILMWEB_API

#endif
