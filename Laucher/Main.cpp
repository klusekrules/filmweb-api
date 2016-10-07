#include "Config\Config.h"
#include "Logger\Log.h"
#include "filmweb-api\Filmweb.h"
#include <iomanip>

using namespace std;

string url_encode(const string &value) {
	ostringstream escaped;
	escaped.fill('0');
	escaped << hex;

	for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		string::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << uppercase;
		escaped << '%' << setw(2) << int((unsigned char)c);
		escaped << nouppercase;
	}

	return escaped.str();
}

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
	log.loguj(SLog::Log::Info, c["nazwa"]);
	
	Filmweb box;
	/*std::string method = "getFilmInfoFull [10637]\\n";
	std::string query = "api?methods=" + url_encode(method) + "&signature=1.0," + Filmweb::md5(method + "androidqjcGhW2JnvGT9dfCt3uT_jozR3s") + "&version=1.0&appId=android";
	box.send(query.c_str());*/
	return 0;
}