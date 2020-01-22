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

// Declare classes here initially to enable interdependence of classes;
class clsShip;
class clsUser;
class clsGamestate;

const int NOVALUE = -1;
const int VERT = 1;
const int HORZ = 2;

struct bulkhead{
  int x = NOVALUE;
  int y = NOVALUE;
  bool hit = false;
};

struct udtCoord{
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

string cleanString(string text){
  string _local = text;
  cout << endl << "BEFORE: [" <<_local << "]\n";

  for(int i = 0; i < _local.length(); i++){
    if(_local[i] == ' ' && _local[i+1] != ' '){//if singlespace;
      _local.erase(i, 1);
    }else if(_local[i] == ' ' && _local[i+1] == ' '){ // if doublespace;
      _local.erase(i, 2);
    } 
  }

  if(_local[0] == ' '){
    _local.erase(0, 1);
  }

  if(_local[_local.size()] == ' '){
    _local.erase(_local.size(), 1);
  }

  cout << "AFTER : [" <<_local << "]";

  return _local;
};

//SHIP CLASS
class clsShip{
  public:
    clsShip(string name, int length){
      _name = name;
      _length = length;
      buildBulkheads();
    }

    void buildBulkheads(){
      for(int i = 0; i < _length; i++){
        bulkhead newBulkhead;
        _bulkheads.push_back(newBulkhead);
      }
    }

    void updateBulkheads(int x, int y, int xSize, int ySize){
      for(int i = 0; i < _bulkheads.size(); i++){
        if(getOrientation() == 'h'){
          _bulkheads[i].x = x;
          _bulkheads[i].y = y + i;

        } else if(getOrientation() == 'v'){
          _bulkheads[i].x = x + i;
          _bulkheads[i].y = y;
        }
      }
    }

    int getOrientation(){
      return _orientation;
    }

    void setOrientation(char orient){
      _orientation = orient;
    }

    vector <bulkhead> getBulkheads(){
      return _bulkheads;
    }

    string getName(){
      return _name;
    }

    int getLength(){
      return _length;
    }

  private:
    char _orientation;
    int _length;
    string _name;
    vector < bulkhead > _bulkheads;
};

//USER CLASS
class clsUser{ //Observer
  public:
    clsUser(string name, int id, bool isCPU, vector < clsShip > config){//constructor
      _name = name;
      _id = id;
      _cpu = isCPU;
      buildFleet(config);
    }

    string getName(){
      return _name;
    }

    int getId(){
      return _id;
    }

    vector < udtCoord > getOccupied(){
      return _occupied;
    }

    void buildFleet(vector < clsShip > fleetConfig){
      for(int i = 0; i < fleetConfig.size(); i++){
        _ships.push_back(fleetConfig[i]);
      }
    }

    bool validateOriginCoord(int xSize, int ySize, int point){
      if(point > xSize || point < 0 || point > ySize){
        return false;
      }
      return true;
    }

    void viewBoard(int xSize, int ySize, bool target = false){
      if(target){
        Log("Targeting ", getName(), "'s board");
        Log();
        
        for(int x = 0; x < xSize; x++){
          for(int y = 0; y < ySize; y++){

            if(getAttackedOrOccupied(x, y, true)){
              cout << "X ";
            } else {
              cout << "_ ";
            }
          }
          Log();
        }

      } else {
        Log("Viewing your (" + getName() + "'s) board");
        Log();

        for(int x = 0; x < xSize; x++){
          for(int y = 0; y < ySize; y++){

            if(getAttackedOrOccupied(x, y)){
              cout << "S ";
            } else {
              cout << "_ ";
            }
          }
          Log();
        }
      }
      Log();
    }

    void placeShips(int xSize, int ySize){
      for(int i = 0; i < _ships.size(); i++){
        int x, y;
        char orient;

        // ClearConsole();

        Log(getName(), " is being asked to place their ", _ships[i].getName());

        viewBoard(xSize, ySize);

        Log("Enter X coord");
        cin >> x;

        while(!validateOriginCoord(xSize, ySize, x)){ //validate x coord
          Log("Invalid X coordinate entered, please enter a coordinate between 0, " ,to_string(xSize));
          cin >> x;
        }
        
        Log("Enter Y coord");
        cin >> y;

        while(!validateOriginCoord(xSize, ySize, y)){ //validate y coord
          Log("Invalid X coordinate entered, please enter a coordinate between 0, " ,to_string(ySize));
          cin >> y;
        }

        Log("Enter orientation (h/v)");
        cin >> orient;

        while(orient != 'h' && orient != 'v'){ //validate orientation
          Log("Please enter a valid orientation (h/v)");
          cin >> orient;
        }

        Log("Placing ship...");

        _ships[i].setOrientation(orient); // set orientation

        
        if(orient == 'h'){
          while(y + _ships[i].getLength() > ySize){ // HORIZONTAL
            Log("Your ", _ships[i].getName(), " is too long to be placed horizontally");

            Log("Please enter a new valid X coordinate");
            cin >> x;

            Log("Please enter a new valid Y coordinate");
            cin >> y;
          }
        } else if(orient == 'v'){
          while(x + _ships[i].getLength() > xSize){ //VERTICAL
            Log("Ship is too long to be placed vertically");

            Log("Please enter a new valid X coordinate");
            cin >> x;

            Log("Please enter a new valid Y coordinate");
            cin >> y;
          }
        }
        

        _ships[i].updateBulkheads(x, y, xSize, ySize);
        placeShip(_ships[i]);
        viewBoard(xSize, ySize);
      }
    }
    
    void placeShip(clsShip ship){
      Log(getName() + " placed their ", ship.getName());
      Log();

      for(int i = 0; i < ship.getBulkheads().size(); i++){
        udtCoord coordinate;
        coordinate.x = ship.getBulkheads()[i].x;
        coordinate.y = ship.getBulkheads()[i].y;

        _occupied.push_back(coordinate);
      }
    }

    vector < udtCoord > getAttacked(){
      return _attacked;
    }

    void addAttacked(int x, int y){
      udtCoord coordinate;
      Log(getName(), " was attacked at: ", to_string(x) + ", " + to_string(y));
      Log();

      coordinate.x = x;
      coordinate.y = y;
      _attacked.push_back(coordinate);
    }
    
    bool getAttackedOrOccupied(int x, int y, bool target = false){ // Using polymorphism - behaviour of this func changes depending if the user is a target or is checking their own ships;
      int result = false;
      vector < udtCoord > tempCoords;

      if(target){ // If user is being targeted -> return tiles that have been attacked already;
        for(int i = 0; i < getAttacked().size(); i++){
          if(getAttacked()[i].x == x && getAttacked()[i].y == y){ // if the coords match return true;
            result = true;
          }
        }
      } else { // If user is looking at their own board -> return tiles that are occupied by their own ships;
        for(int i = 0; i < getOccupied().size(); i++){
          if(getOccupied()[i].x == x && getOccupied()[i].y == y){ // if the coords match return true;
            result = true;
          }
        }
      }
      return result;
    }

    //DEBUG function to print a users attacked coords to the console;
    void printAttacked(){
      Log("Printing attacked coords for ", getName());
      for(int i = 0; i < _attacked.size(); i++){
        Log(to_string(_attacked[i].x), ", ",to_string(_attacked[i].y));
      }
      Log();
    }

    void isCPU(){ //DEBUG FUNC
      Log(getName(), " isCPU -> ", to_string(_cpu));
    }

  private:
    int _id;
    string _name;
    vector < udtCoord > _occupied; // represents a players personal board;
    vector < udtCoord > _attacked; // represents where users have fired at this users board;
    vector < clsShip > _ships;
    bool _ready = false;
    bool _cpu = false;
}; // Maybe a class for CPU players inheirits clsUser & includes the AI in that inheirited class?

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

    void setPlayerCount(int count){
      _playerCount = count;
    }

    int getPlayerCount(){
      return _playerCount;
    }

    void setBoardSize(int x, int y){
      _boardSize.x = x;
      _boardSize.y = y;
    }

    udtCoord getBoardSize(){
      return _boardSize;
    }

    void initPlayerCount(bool ai = false){
      int playerCount;

      ClearConsole();

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
      char userChoice;
      bool isCPU = false;
      ClearConsole();

      for(int i = 0; i < _playerCount; i++){
        string name;

        Log("Enter player ", to_string(i+1), "'s name:");
        cin >> name;

        while(checkIfDuplicateUser(name)){ // While another user with the same name exists - ask the user for a unique name;
          Log("There is already a user with that name, please enter another name:");
          cin >> name;
        }

        // Log("Is ", name, " a CPU player? (y/n)");
        // while(userChoice != 'y' && userChoice != 'n'){
        //   cin >> userChoice;
        // }

        if(userChoice == 'y'){
          isCPU = true;
        }

        clsUser newUser(name, _users.size() + 1, isCPU, _fleetConfig); // Create a new instance of a user with name and Id;
        registerUser(newUser); // register the new user;
        userChoice = ' '; //reset userchoice;
        isCPU = false; //reset isCPU;
      }
    }

    void initBoardSize(){
      int x, y;

      ClearConsole();

      Log("Please enter the size of the X axis for player boards");
      cin >> x;

      Log("Please enter the size of the Y axis for player boards");
      cin >> y;

      setBoardSize(x, y);
    }

    void startNewGame(){ // controls sequence of events for a game to be played;
      deleteAllUsers(); //reset the game by destroying all active players;

      initBoardSize();
      initPlayerCount();
      initPlayers();

      printAllUsers();
      startGame();
    }

    void startGame(){
      char userChoice;

      if(_activePlayer == 0){
        while(userChoice != 'y' && userChoice != 'n'){
          Log("Ready to start the game? (y/n)");
          cin >> userChoice;
        }

        if(userChoice == 'n'){
          startNewGame();
        }

        if(userChoice == 'y'){
          ClearConsole();
          Log("Game starting...");
          _stage = 1;
          updateUsers();
        }
      }
    }

    void progressTurn(){
      char choice;
      
      Log("Ready to continue? (y/n)");
      cin >> choice;

      while(choice != 'y' && choice != 'n'){
        Log("Please enter 'y' or 'n'");
        cin >> choice;
      }
    }

    void updateUsers(){
      if(_stage == 1){ // if stage is 'placement' - get players to place ships;
        for(int i = 0; i < _users.size(); i++){
          _users[i].placeShips(getBoardSize().x, getBoardSize().y); // get user to place their boats;
          progressTurn(); // wait for user to input to continue
          ClearConsole();
        }
        _stage = 2;
      } else if(_stage == 2){ // if stage is 'play' - cycle through users to choose target & attack;
        //If _playerCount > 1;
        //promptForTarget(_activePlayer); 
      } else if(_stage == 3){ // if stage is 'winner' - display winner screen to remaining player(s);
        //display winner screen;
        //ask if they want to play again;
      }
    }

    void printAllUsers(){
      ClearConsole();
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

    clsUser& getUserById(int id){
      for(int i = 0; i < _users.size(); i++){
        if(_users[i].getId() == id){
          return _users[i];
        }
      }
      cout << "USER NOT FOUND";
    }

    

    void deleteAllUsers(){
      if(_users.size()){
        _users.clear();
      }
    }

    void registerShip(clsShip ship){
      _fleetConfig.push_back(ship);
    }

  private:
    int _stage = 0;
    int _activePlayer = 0;
    int _playerCount;
    vector < clsUser > _users;
    vector < clsShip > _fleetConfig;
    udtCoord _boardSize;
    static clsGamestate* _inst;
};


clsGamestate* clsGamestate::_inst = NULL;

int main(){
  clsGamestate* state; // set variable 'Gamestate' as a pointer;
  state = clsGamestate::getInstance(); // assign the instance of clsGamestate;
  

  clsShip destroyer("Destroyer", 4); //will be read from a file + built at runtime
  clsShip patrolBoat("Patrol Boat", 2);

  state -> registerShip(destroyer); //register the ship config for the game
  state -> registerShip(patrolBoat); //call register ship loop at runtime based on file content

  state -> startNewGame();
}
