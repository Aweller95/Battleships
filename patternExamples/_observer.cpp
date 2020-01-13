#include <iostream>
#include <fstream> 
#include <vector> //include support for vectors 
#include <functional> //include support for: std::reference_wrapper
#include <algorithm>  //include support for: std::find_if

using namespace std;

class observer {
  public:
  observer(string name){
    set_reference(name);
    makeFile();
  }

  ~observer(){
    closeFile();
  }

  void registerSubject(){

  }

	void set_reference(string reference) {
		_reference=reference;
	}

	void update_subject(string message)  { //send to all
    writefile(message);
		cout << _reference << " - " << message << "\n";
	}

	void notify(string message, observer& recipient)  { //send to specific user
    cout << getReference() << " sent a DM to " << recipient.getReference() << "\n";
    recipient.writefile("> DM from " + getReference() + ": " + message);
	}

  string getReference(){
    return _reference;
  }

  string getFileName(){
    return getReference() + ".txt";
  }

  void writefile(string message){
    outfile << message << endl;
  }

  void makeFile(){
    outfile.open(getFileName().c_str());
  }

  void closeFile(){
    outfile.close();
  }

	private:
		string _reference;
    ofstream outfile;
};

class subject {
  public:
    void register_observer(observer& o) {
      update_observers(o, "join");
      _observers.push_back(o);
    }

    void update_observers(string message){
      for (observer& o : _observers) { 
        // cout << "update_observers called\n";
        o.update_subject(message);
      }
    }

    void update_observers(observer& user, string action = "") {
      //in this section, the colon (:) tells the compiler to iterates the vector (_observers) using 'being' and 'end' iterators
      //see the deregister function for additional examples
        for (observer& o : _observers) { 
          if(action == "join"){
            o.update_subject(user.getReference() + " has joined the conversation\n");

          } else if (action == "leave"){
            o.update_subject(user.getReference() + " has left the conversation\n");
        }
      }
    }

	void deregister_observer(observer& o ) {
		//search the observer vector for the specific observer
		auto it = find_if( begin( _observers ), end( _observers ), [ &o ]( observer& element ) -> bool	{ return &element == &o; });
		_observers.erase( it ); //execute the built-in "erase" method to remove the specific observer
    update_observers(o, "leave");
	}

  private:
    vector <reference_wrapper <observer>> _observers;
};

// Notify generic observer objects when an event occurs.
//
// The observer pattern allows generic observer objects to be
// registered with a subject object and receive notifications
// when certain events occur.
//
// The `subject` class, defined on [21-44], contains a
// [`vector`](cpp/container/vector) of references to observers [43].
// Observers (also known as listeners), in this case, are objects that
// implement the `observer` class ([6-19]). The
// `register_observer` function ([24-26]) adds observers
// to this `vector`, which are later to be notified by the
// `notify_observers` function ([28-31]).
//
// We use [`reference_wrapper`] (https://en.cppreference.com/w/cpp/utility/functional/reference_wrapper)
// for the elements of the `vector` ([43]), because the standard
// containers require the element type to be assignable, which normal
// reference types are not.

// int main() {
// 	subject s; //instantiate an instance of the subject class 
// 	observer dog("dog"), cat("cat"), mouse("mouse"), duck("duck"); //instantiate instance for each possible observer class 

// 	s.register_observer(dog); //pass (by reference) a pointer to the specific obverser class
  
// }