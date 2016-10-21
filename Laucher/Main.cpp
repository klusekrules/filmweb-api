#include "Logger\Log.h"
#include "filmweb-api\Filmweb.h"
#include <iomanip>

using namespace std;

int main() {
	Config c("config.txt");
	auto& log = SLog::Log::pobierzInstancje();
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
	});
	
	Filmweb::Filmweb box(c);
	box.getDetails(10637);
	box.getSearch(std::string("Minions"));
	return 0;
}