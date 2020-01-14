#include <iostream>
#include <string>
#include <vector>

using namespace std;

/* 

NOTES

- Gamestate contains a vector of users; The state registers new users;
- User contains a vector of ships;
- Board needs to know about players & ships - must subscribe to both User and Gamestate;
-- Board needs to be able to iterate through all users & then iterate through each users ship vector;

*/

const int NOVALUE = -1;

struct bulkhead{
  int x = NOVALUE;
  int y = NOVALUE;
  bool hit = false;
};

struct coordinate{
  int x;
  int y;
};

struct board{
  vector <int> xAxis;
  vector <int> yAxis;
};

//HELPERS
/* 
I am using a naming scheme for helper functions to start with capital letters. This is to allow for easy differentiation between other function classifications.
- Log() takes 3 optional params that will print to the console. Calling with no params prints a new line - I found that this was a cleaner way instead of using ```cout << "\n"```.
*/
void Log(string message1 = "", string message2 = "", string message3 = ""){
  cout << message1 << message2 << message3 << endl;
}

void ClearConsole(){
  cout << "\033[2J\033[0;0H"; // escape sequence that clears the console;
}

//SHIP CLASS
class clsShip{
  public:
    clsShip(string name, int length){
      _name = name;
      _length = length;
      buildBulkheads();
    }

    void buildBulkheads(void){
      for(int i = 0; i < _length; i++){
        bulkhead newBulkhead;
        _bulkheads.push_back(newBulkhead);
      }
    }

    vector <bulkhead> getBulkheads(void){
      return _bulkheads;
    }

    string getName(){
      return _name;
    }

    int getLength(){
      return _length;
    }

  private:
    int _id;
    int _length;
    string _name;
    vector<bulkhead> _bulkheads;
};

//USER CLASS
class clsUser{ //Observer
  public:
    clsUser(string name){
      _name = name;
    }

    string getName(){
      return _name;
    }

    void setId(int id){
      _id = id;
    }
  
  private:

    int _id;
    string _name;
    bool _cpu = false;
};

//GAMESTATE CLASS
class clsGamestate {
  public:
    //used to allow clients to access the instance of Gamestate;
    static clsGamestate* getInstance() {
			if (_inst == NULL) {
				_inst = new clsGamestate();
			}
			return(_inst);
		};
    
    int getPlayerCount(){
      return _playerCount;
    }

    void setPlayerCount(int count){
      _playerCount = count;
    }

    void initPlayerCount(){
      int playerCount;

      Log("Enter the number of players: ");
      cin >> playerCount;
      
      setPlayerCount(playerCount);
    }

    bool checkIfDuplicateUser(string newName){
      bool result = false;

      for(int i = 0; i < _users.size(); i++){
        if(_users[i].getName() == newName){
          result = true;
        }
      }

      return result;
    }

    void initPlayers(){
      for(int i = 0; i < _playerCount; i++){
        string name;
        Log("Enter player ", to_string(i+1), "'s name:");
        cin >> name;

        while(checkIfDuplicateUser(name)){
          Log("There is already a user playing with that name, please enter another name:");
          cin >> name;
        }

        clsUser newUser(name);
        registerUser(newUser);
      }
      Log();
    }

    void initNewGame(){
      initPlayerCount();
      initPlayers();
    }

    void printAllUsers(){
      Log("Player List: ");

      if(_users.size()){
        for(int i = 0; i < _users.size(); i++){
          Log(to_string(i+1), "." , _users[i].getName());
        }
      } else {
        Log("!! No users registered !!");
      }

      Log();
    };
        
    void registerUser(clsUser& user){
      _users.push_back(user);
    }

    vector < clsUser > getUsers(){
      return _users;
    }
  
    void deleteAllUsers(){
      _users.clear();
    }

  private:
    int _state;
    int _playerCount;
    vector < clsUser > _users;
    int xAxisSize;
    int yAxisSize;
    static clsGamestate* _inst;
};

//Think about inter-dependency & using friend functions;
clsGamestate* clsGamestate::_inst = NULL;

int main(){
  clsGamestate* state; // set variable 'Gamestate' as a pointer;
  state = clsGamestate::getInstance(); // assign the instance of clsGamestate;

  state -> initNewGame();

  state -> printAllUsers();
}