#include "Logger\Log.h"
#include "filmweb-api\Filmweb.h"
#include <iomanip>
#include <iostream>
#include <clocale>
#include <Windows.h>

#include <io.h>
#include <fcntl.h>

using namespace std;

int main() {
	_setmode(_fileno(stdout), _O_U16TEXT);
	Config c("config.txt");
	/*auto& log = SLog::Log::pobierzInstancje();
	log.dodajGniazdoWyjsciowe([](SLog::Log::TypLogow typ, const std::string& czas, const std::string& komunikat)->void {
		static std::fstream plik("out.log", std::fstream::app);
		std::string sTyp;
		switch (typ)
		{
		case SLog::Log::Debug: sTyp = " [DEBUG] ";
			break;
		case SLog::Log::Info: sTyp = " [INFO] ";
			break;
		case SLog::Log::Warning: sTyp = " [WARNING] ";
			break;
		case SLog::Log::Error: sTyp = " [ERROR] ";
			break;
		case SLog::Log::All:
		default:
			break;
		}
		plik << czas << sTyp << komunikat << std::endl << std::flush;
	});*/
	
	Filmweb::Filmweb box(c);
	std::vector<Filmweb::SearchResult> data;
	Filmweb::Film film;
	if (box.getSearch(std::wstring(L"Siedem"), data))
		box.getFilmInfoFull(data[1], film);

	std::wcout << film.zarysFabu³y_ << std::endl << std::flush;

	return 0;
}

