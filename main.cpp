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

// Declare classes here initially to enable interdependence;
class clsShip;
class clsUser;
class clsGamestate;

const int NOVALUE = -1;

struct bulkhead{
  int x = NOVALUE;
  int y = NOVALUE;
  bool hit = false;
};

struct coord{
  int x;
  int y;
};

//UTILITIES CLASS
/* 
I am using a naming scheme for helper functions to start with capital letters. This is to allow for easy differentiation between other function classifications.
- Log() takes 3 optional params that will print to the console. Calling with no params prints a new line - I found that this was a cleaner way instead of using `cout << "\n"`.
- ClearConsole() prints an escape sequence using `cout` which will clear all data currently displayed on the console.
*/

// class clsUtilities{ USE THIS TO BE INHERITED BY SUBCLASSES ONCE TESTED
//   protected:
//     void Log(string message1 = "", string message2 = "", string message3 = ""){
//       cout << message1 << message2 << message3 << endl;
//     }

//     void ClearConsole(){
//       cout << "\033[2J\033[0;0H"; // escape sequence that clears the console;
//     }
// };

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
    clsUser(string name, int id){//constructor
      _name = name;
      _id = id;
    }

    string getName(){
      return _name;
    }

    int getId(){
      return _id;
    }

    vector < coord > getOccupied(){
      return _occupied;
    }
    
    void addOccupied(int x, int y){
      coord coordinate;
      coordinate.x = x;
      coordinate.y = y;
      _occupied.push_back(coordinate);
    }

    vector < coord > getAttacked(){
      return _attacked;
    }

    void addAttacked(int x, int y){
      coord coordinate;
      coordinate.x = x;
      coordinate.y = y;
      _attacked.push_back(coordinate);
    }
    
    bool checkCoord(int x, int y, bool target = false){ // Using polymorphism - change the behaviour of thos func depending if the user is a target or is checking their own ships;
      bool result = false;
      vector < coord > tempCoords;

      if(target){ // If user is being targeted -> return tiles that have been attacked already;
        for(int i = 0; i < getAttacked().size(); i++){
          if(getAttacked()[i].x == x && getAttacked()[i].y == y){
            result = true;
          }
        }
      } else { // If user is looking at their own board -> return tiles that are occupied by their own ships;
        for(int i = 0; i < getOccupied().size(); i++){
          if(getOccupied()[i].x == x && getOccupied()[i].y == y){
            result = true;
          }
        }
      }

      return result;
    }

    //DEBUG func
    void printAttacked(){
      for(int i = 0; i < _attacked.size(); i++){
        Log(to_string(_attacked[i].x), " ",to_string(_attacked[i].y));
      }
    }

  private:
    int _id;
    string _name;
    vector < coord > _occupied; // represents a players personal board;
    vector < coord > _attacked;
    bool _cpu = false;
};

//GAMESTATE CLASS - Combined Singleton + State Machine Design Patterns
class clsGamestate{
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

    void setBoardSize(int x, int y){
      _boardSize.x = x;
      _boardSize.y = y;
    }

    coord getBoardSize(){
      return _boardSize;
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
          Log("There is already a user with that name, please enter another name:");
          cin >> name;
        }

        clsUser newUser(name, _users.size() + 1);
        registerUser(newUser);
      }
      Log();
    }

    void initBoardSize(){
      int x, y;

      Log("Please enter the size of the X axis for player boards");
      cin >> x;

      Log("Please enter the size of the Y axis for player boards");
      cin >> y;

      setBoardSize(x, y);
    }

    void startNewGame(){
      initBoardSize();
      initPlayerCount();
      initPlayers();
    }

    void printAllUsers(){
      Log("Player List: ");

      if(_users.size()){
        for(int i = 0; i < _users.size(); i++){
          Log(to_string(_users[i].getId()), "." , _users[i].getName());
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

    clsUser getUserById(int id){
      for(int i = 0; i < _users.size(); i++){
        if(_users[i].getId() == id){
          return _users[i];
        }
      }
      cout << "USER NOT FOUND";
    }

    // void viewBoard(int userId, bool target = false){
    //   if(target){
    //     // should abstract this to a func;
    //     vector <coord> damagedZones;
    //     int userIndex;

    //     //retrieve zones for a player which have been fired upon;
    //     for(int i = 0; i < _users.size(); i++){
    //       if(_users[i].getId() == userId){
    //         damagedZones = _users[i].getAttacked();
    //         userIndex = i;
    //       }
    //     }

    //     Log("Viewing ", _users[userIndex].getName(), "'s board as target");
        
    //     for(int x = 0; x < _boardSize.x; x++){
    //       for(int y = 0; y < _boardSize.y; y++){
    //         //if x & y match any attacked coords - print 'X'
    //         if(checkCoord(x, y, userIndex, true)){
    //           cout << 1;
    //         } else {
    //           //otherwise print '.'
    //           cout << 0;
    //         }
    //       }
    //       cout << endl;
    //     }

    //   } else {
    //     // Log("Viewing ", getName(), "'s board as owner");

    //   }
    // }

    void deleteAllUsers(){
      _users.clear();
    }

  private:
    int _state;
    int _playerCount;
    vector < clsUser > _users;
    coord _boardSize;
    static clsGamestate* _inst;
};


clsGamestate* clsGamestate::_inst = NULL;

int main(){
  clsGamestate* state; // set variable 'Gamestate' as a pointer;
  state = clsGamestate::getInstance(); // assign the instance of clsGamestate;

  // state -> startNewGame();

  // state -> printAllUsers();
  
  clsUser alex("Alex", 1); // create user
  state -> registerUser(alex); // register alex
  Log();

  alex.addOccupied(2, 2);

  Log("Coord 2, 2 contains a ship?  ", to_string(alex.checkCoord(2, 2)));
  Log("Coord 1, 2 contains a ship?  ", to_string(alex.checkCoord(1, 2)));

  // state -> viewBoard(1, true);
}