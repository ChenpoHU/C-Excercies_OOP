#include <iostream> 
#include <string> 
#include <locale> 
#include <type_traits> // is_signed 
using namespace std;

int main() {
	locale loc;
	cout << "loc.name()= " << loc.name() << '\n';
	locale dt("de_DE");
	cout << "dt.name()= " << dt.name() << '\n';
	locale vorher = locale::global(dt); // dt f�r alles setzen

	string s("�������");

	cout.imbue(dt);
	if (s.length() > 7) {
		cerr << "FEHLER: DIESES PROGRAMM IST NICHT in ISO 8859 gespeichert" " und wird nicht wie beabsichtigt funktionieren! Abbruch!\n";
		return 1;
	}
	cout << "\nProgramm funktioniert nur richtig bei auf ISO 8859" " eingestellter Konsole! \n";
	for (unsigned int i = 0; i < s.length(); ++i) {
		cout << "Zeichen " << i << ": " << s[i] << " ";
		int intwert = static_cast<int>(s[i]);
		if (is_signed<char>::value) { // ggf. Korrektur f�r signed char 
			intwert += 256;
		}
		cout << intwert << '\n';
	}
	for (auto c : s) { //can change, but the original won�t be changed 
		cout << toupper(c, dt) << tolower(c, dt) << " ";
	}
	for (auto& c : s) { //change the original by using reference; when only want to read, (const auto& c: s)
		c = toupper(c, dt);
	}
	cout << "\nNach toupper: " << s << '\n';
	// Andere M�glichkeit, jetzt mit tolower: 
	use_facet<ctype<char>>(dt).tolower(&s[0], s.c_str() + s.length());
	cout << "Nach tolower: " << s << '\n';
}

/*
Output Should
$ ./narrow.exe loc.name()= C dt.name()= de_DE
Programm funktioniert nur richtig bei auf ISO 8859 eingestellter Konsole! 
Zeichen 0: �   196 
Zeichen 1: �   214 
Zeichen 2: �   220 
Zeichen 3: �   228 
Zeichen 4: �   246 
Zeichen 5: �   252 
Zeichen 6: �   223 
�� �� �� �� �� �� �� 
Nach toupper: ������� 
Nach tolower: �������
*/

/*
Output when use string s("\xC3\x84\xC3\x96\xC3\x9C\xC3\xA4\xC3\xB6\xC3\xBC\xC3\x9F");
loc.name()= C
dt.name()= de_DE
FEHLER: DIESES PROGRAMM IST NICHT in ISO 8859 gespeichert und wird nicht wie beabsichtigt funktionieren! Abbruch!
*/