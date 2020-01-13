#include <iostream>

using namespace std; //set default namespace

class Singleton {
	public:
		//this is how clients can access the single instance
		static Singleton* getInstance() {
			if (_inst == NULL) {
				_inst = new Singleton();
			}
			return(_inst);
		};

		void setValue(int val) { _value = val; }
		int getValue() { return(_value); }

	private:
		int _value;
		static Singleton* _inst; //used to hold a pointer to a specific instance
		Singleton() : _value(0) {} //private constructor
		//the above code line used an initialistion list, because 'const' must be initialised (once)
		//and is not owned by a specific instance we can use the method above to initialise the values 
};


//initialise the static Singleton pointer
Singleton* Singleton::_inst = NULL;


// int main() {
	// Singleton* instance1; //set the variable instance1 as a pointer (*)
	// Singleton* instance2;
	
	// instance1 = Singleton::getInstance();
	// instance2 = Singleton::getInstance();
	
	// instance2->setValue(10); //use the arrow operator (->) to access members using pointers
	// cout << instance1->getValue() << "\n";

	// instance1->setValue(20); //use the arrow operator (->) to access members using pointers
	// cout << instance2->getValue() << "\n";
// }