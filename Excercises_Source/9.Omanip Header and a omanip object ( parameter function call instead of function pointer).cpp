//Im Header <iomanip> sind diese Klasse und ein friend - Operator << etwa der folgenden oder einer �hnlichen Art zu finden :
#include <iostream>

using namespace std; // for std::ostream 

template<typename T>
class omanip { // compiler-specific name  
	ostream& (*funktPtr)(ostream&, T); T arg;
public:
	omanip(ostream& (*f)(ostream&, T), T obj) : funktPtr(f), arg(obj) {} //constructor
	friend ostream& operator << (ostream&, omanip<T>&);
};

template<typename T>
ostream& operator <<(ostream& s, const omanip<T>& fobj) {
	return(*fobj.funktPtr)(s, fobj.arg);
}

//Omanip-object
//Ein omanip-Objekt hat zwei private Variable: funktPtr ist ein Zeiger auf eine Funktion, 
//-die eine Referenz auf einen ostream zur�ckgibt und 
//-einen Parameter des Typs ostream& sowie ein Objekt des Typs T erwartet. 

// Die Funktion setprecision() gibt ein Objekt vom Typ omanip zur�ck
//Dem Konstruktor eines omanipObjekts wird die Adresse einer Funktion (precision) und der Wert p �bergeben. 
//Function object is invoked with parameter in overloaded <<
omanip<int> setprecision(int p) {
	return omanip<int>(precision, p);
}