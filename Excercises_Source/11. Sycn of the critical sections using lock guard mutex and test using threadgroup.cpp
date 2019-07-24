#include <thread> 
#include <iostream> 
#include <cstddef> 
#include <functional> // ref() 
#include <vector>
#include <mutex>
namespace { std::mutex mtx; }

//no standard for the threadgroup, but easy to construct. 
class ThreadGroup {
public: ThreadGroup() = default;
		ThreadGroup(const ThreadGroup&) = delete;
		ThreadGroup& operator=(const ThreadGroup&) = delete;
		template <typename F>
		void add_thread(F& f) {
			threads.emplace_back(std::ref(f)); // Thread speichern und starten 
		}
		void join_all() { // Auf Beendigung aller Threads warten 
			for (auto& t : threads) {
				t.join();
			}
		}
		std::size_t size() const {
			return threads.size();
		} // m�gliche weitere Methoden weggelassen 
private:
	std::vector<std::thread> threads;
};

class Gerade {
public:
	Gerade() : n(0) {}

	//data race here when not use mutex
	int next() {
		std::lock_guard<std::mutex> lock(mtx); /* Lock-Objekt anlegen; Correct Synchronization;
		�lock_guard constructor invokes mtx.lock()
		�release of lock automatically at end of scope (scoped locking)
		�destructor invokes mtx.unlock() */
		++n; //!!!critical section: absichtlich zwei Anweisungen statt n += 2;, which makes these two ++n lines !critical section!, not atomic 
		++n; //!!!critical section
		return n;
	}

	int getWert() const { return n; }
private:
	int n;
};

Gerade g;

void testeGerade() {
	for (int i = 0; i < 10000; ++i) {
		int wert = g.next();
		if (wert % 2 != 0) {
			std::cout << wert << " ist ungerade! Thread beendet.\n"; break; // Schleife und damit die Funktion beenden 
		}
	}
}
/*
Im sequenziellen Fall ist garantiert, dass next() eine gerade Zahl liefert.
Wenn jedoch zwei oder mehrere Threads auf dasselbe Objekt zugreifen, kann es Fehler geben.
Wenn nur zwei Threads A und B existieren und n den Anfangswert 0 hat, kann es zu folgender Abfolge kommen:
1. Thread A f�hrt next() bis einschlie�lich Zeile 7 aus und wird dann vom Betriebssystem in die Warteschlange wartender Threads gepackt,
damit Thread B drankommt. Das Attribut n hat den Wert 1.
2. Thread B f�hrt next() aus und erh�ht n zweimal. Es wird 3 zur�ckgegeben � Fehler!
3. Thread A kommt wieder dran und f�hrt next() zu Ende aus; es wird 4 zur�ckgegeben
*/

int main() {
	ThreadGroup threadgroup;
	for (int i = 0; i < 20; ++i) {
		threadgroup.add_thread(testeGerade);
	}
	threadgroup.join_all();
}

/*
Das Programm wird mal ungerade Zahlen ausgeben, mal auch nicht � abh�ngig von der Einplanung der Threads.
Der Effekt, dass zuf�llig mal der eine und mal der andere Thread zuerst die Daten �ndert, hei�t data race.
Eine Bedingung, die dazu f�hrt, hei�t race condition.
Die Ursache des Problems liegt darin, dass eine korrekte Abarbeitung der Funktion next() atomar (unteilbar) sein muss.
Ein Thread soll next() nur dann betreten d�rfen, wenn jeder andere Thread die Funktion verlassen hat.
Bereiche, in denen Daten ver�ndert werden und die dem Zugriff mehrerer Threads ausgesetzt sind, wie hier die Zeilen 7 und 8 der Klasse,
hei�en kritischer Bereich (englisch critical section). Der Zugriff auf einen kritischen Bereich muss synchronisiert werden,
indem sich die Zugriffe gegenseitig ausschlie�en (englisch mutual exclusion). Die Klasse mutex steuert den gegenseitigen Ausschluss.

*/


/*
Output without using std::lock_guard<std::mutex> lock(mtx);:
1161 ist ungerade! Thread beendet.
1159 ist ungerade! Thread beendet.
11371139 ist ungerade! Thread beendet.
1143 ist ungerade! Thread beendet.
1145 ist ungerade! Thread beendet.
1147 ist ungerade! Thread beendet.
1149 ist ungerade! Thread beendet.
1151 ist ungerade! Thread beendet.
1165 ist ungerade! Thread beendet.
1167 ist ungerade! Thread beendet.
1171 ist ungerade! Thread beendet.
1163 ist ungerade! Thread beendet.
1135 ist ungerade! Thread beendet.
1153 ist ungerade! Thread beendet.
1173 ist ungerade! Thread beendet.
1157 ist ungerade! Thread beendet.
 ist ungerade! Thread beendet.
1141 ist ungerade! Thread beendet.
1155 ist ungerade! Thread beendet.
1169 ist ungerade! Thread beendet.
*/