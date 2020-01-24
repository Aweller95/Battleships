#include <iostream>
#include <string>
#include <vector>
#include<cmath>

using namespace std;

/* 
NOTES

Welcome to Alex Wellers...
   ___         __   __   __           __    _          
  / _ ) ___ _ / /_ / /_ / /___  ___  / /   (_)___   ___
 / _  |/ _ `// __// __// // -_)(_-< / _ \ / // _ \ (_-<
/____/ \_,_/ \__/ \__//_/ \__//___//_//_//_// .__//___/
                                           /_/         

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

void enterToContinue(){
  do {
  cout << '\n' << "Press enter to start...";
  } while (cin.get() != '\n');
}

void printRed(string message){
  cout << "\x1B[31m" + message + "\033[0m";
}

string setRed(string message){
  return "\x1B[31m" + message + "\033[0m";
}

void printGreen(string message){
  cout << "\x1B[32m" + message + "\033[0m";
}

string setGreen(string message){
  return "\x1B[32m" + message + "\033[0m";
}

void printOrange(string message){
  cout << "\x1B[33m" + message + "\033[0m";
}

string setOrange(string message){
  return "\x1B[33m" + message + "\033[0m";
}

void printTitle(){  
  string line1 = "   ___         __   __   __           __    _          ";
  string line2 = "  / _ ) ___ _ / /_ / /_ / /___  ___  / /   (_)___   ___";
  string line3 = " / _  |/ _ `// __// __// // -_)(_-< / _ \\ / // _ \\ (_-<";
  string line4 = "/____/ \\_,_/ \\__/ \\__//_/ \\__//___//_//_//_// .__//___/";
  string line5 = "                                           /_/         ";   

  Log(setGreen(line1));
  Log(setGreen(line2));
  Log(setGreen(line3));
  Log(setGreen(line4));
  Log(setGreen(line5));
  Log();
}

void printConfigTitle(){  

  string line1 = "  _____             ___ _      ";
  string line2 = " / ___/___   ___   / _/(_)___ _";
  string line3 = "/ /__ / _ \\ / _ \\ / _// // _ `/";
  string line4 = "\\___/ \\___//_//_//_/ /_/ \\_, / ";
  string line5 = "                        /___/  ";

  Log(setGreen(line1));
  Log(setGreen(line2));
  Log(setGreen(line3));
  Log(setGreen(line4));
  Log(setGreen(line5));
  Log();
}

void printPlayersTitle(){
  string line1 = "   ___   __                         ";
  string line2 = "  / _ \\ / /___ _ __ __ ___  ____ ___";
  string line3 = " / ___// // _ `// // // -_)/ __/(_-<";
  string line4 = "/_/   /_/ \\_,_/ \\_, / \\__//_/  /___/";
  string line5 = "               /___/                ";

  Log(setGreen(line1));
  Log(setGreen(line2));
  Log(setGreen(line3));
  Log(setGreen(line4));
  Log(setGreen(line5));
  Log();
}

void printPlacementTitle(){
  string line1 = "   ___   __                                 __ ";
  string line2 = "  / _ \\ / /___ _ ____ ___  __ _  ___  ___  / /_";
  string line3 = " / ___// // _ `// __// -_)/  ' \\/ -_)/ _ \\/ __/";
  string line4 = "/_/   /_/ \\_,_/ \\__/ \\__//_/_/_/\\__//_//_/\\__/ ";
                            
  Log(setGreen(line1));
  Log(setGreen(line2));
  Log(setGreen(line3));
  Log(setGreen(line4));
  Log();                 
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

    void updateBulkheads(int x, int y, int xSize, int ySize, char direction){
      for(int i = 0; i < _bulkheads.size(); i++){
        if(direction == 'l'){
          _bulkheads[i].x = x - i;
          _bulkheads[i].y = y;

        } else if(direction == 'r'){
          _bulkheads[i].x = x + i;
          _bulkheads[i].y = y;
          
        } else if(direction == 'u'){
          _bulkheads[i].x = x;
          _bulkheads[i].y = y + i;

        } else if(direction == 'd'){
          _bulkheads[i].x = x;
          _bulkheads[i].y = y - i;
        }
      }
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

    bool checkIfCollision(int x, int y){ //Check if an origin coord is occupied
      for(int i = 0; i < _occupied.size(); i++){
        if(_occupied[i].x == x && _occupied[i].y == y){
          return true;
        }
      }
      return false;
    }

    bool checkIfCollision(int x, int y, char direction, int length){// check all points that a ship will occupy based on direction
      vector <udtCoord> shipPoints;

      for(int i = 0; i < length; i++){ // populate shipPoints with empty coords based on ship length;
        udtCoord tempCoord;
        shipPoints.push_back(tempCoord);
      }

      for(int i = 0; i < length; i++){ // assign the values that the ship will occupy;
        if(direction == 'l'){
            shipPoints[i].x = x - i;
            shipPoints[i].y = y;

          } else if(direction == 'r'){
            shipPoints[i].x = x + i;
            shipPoints[i].y = y;
            
          } else if(direction == 'u'){
            shipPoints[i].x = x;
            shipPoints[i].y = y + i;

          } else if(direction == 'd'){
            shipPoints[i].x = x;
            shipPoints[i].y = y - i;
          }
      }

      for(int o = 0; o < _occupied.size(); o++){ //for each occupied coord
        for(int s = 0; s < shipPoints.size(); s++){ // check each shipPoints coord if it matches
          if(_occupied[o].x == shipPoints[s].x && _occupied[o].y == shipPoints[s].y){
            return true;
          }
        }
      }
      return false;
    }

    bool validateOriginCoord(int xSize, int ySize, int point){
      if(point > xSize || point < 1 || point > ySize){
        return false;
      }
      return true;
    }

    int getIntLength(int i){
      return trunc(log10(i)) + 1;
      // using log10 -> returns the value y in base 10. 
      // using trunc -> returns a rounded down result of the log10 function; 
      //example: (log10(100) = 2) + 1 = 3 -> 100 is a 3 digit number;
    }

    void viewBoard(int xSize, int ySize, bool target = false){ // view a players board, optional param target will change if the board is drawn with occupied spaces or spaces that have been fired at;

      if(target){
        Log("Targeting ", getName(), "'s board");
        Log();
      }

      for(int y = ySize; y >= 1; y--){
        int yLen = getIntLength(y); // get the num of digits of current iterator y

        if(yLen == 1){
            cout << y << "  | ";
          } else if(yLen == 2){ // if y is double digit, change spacing;
            cout << y << " | ";
          } else { // else print standard space;
            cout << y << "| ";
          }

        for(int x = 1; x <= xSize; x++){
          if(getAttackedOrOccupied(x, y, target)){
            if(target){
              cout << setOrange("X "); // print an X if this coord has been attacked;
            } else {
              cout << setGreen("S "); // print an S if this coord is occupied by a ship;
            }
          } else {
            cout << "_ ";
          }
        }
        Log();
      }

      cout << "     ";
      for(int x = 1; x <= xSize; x++){ // print x axis labels
        // int xLen = getIntLength(x); // get the num of digits of current iterator x
        cout << x << " ";
      }
      Log();
      Log();
    }

    void viewBoard(int xSize, int ySize, int selectedX, int selectedY){ // view a players board with a point highlighted; // using function overloading here to alter the behaviour of this func;
        for(int y = ySize; y >= 1; y--){

          if(getIntLength(y) == 1){
            cout << y << "  | ";
          } else if(getIntLength(y) == 2){ // if y is double digit, change spacing;
            cout << y << " | ";
          } else { // else print standard space;
            cout << y << "| ";
          }
          
          for(int x = 1; x <= xSize; x++){

            if(getAttackedOrOccupied(x, y)){
              printOrange("S ");
            } else if(x == selectedX && y == selectedY){
              printGreen("_ "); // prints the selected coordinate in green;
            } else {
              cout << "_ ";
            }
          }
          Log();
        }
        cout << "     ";
        for(int x = 1; x <= xSize; x++){ // print x axis labels
          cout << x << " ";
        }
        Log();
    }

    void placeShips(int xSize, int ySize){
      for(int i = 0; i < _ships.size(); i++){
        int x, y;
        char orient, direction;

        ClearConsole();
        printPlacementTitle();

        Log(getName(), ", place your ", _ships[i].getName() + " (length: " + to_string(_ships[i].getLength()) + ")");
        Log();
        viewBoard(xSize, ySize); // TODO: Prompt user to view board to prevent other users seeing it;

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

        while(checkIfCollision(x, y)){ // checks if origin point collides with another ship
          Log("Placing your" + _ships[i].getName() + " here, will cause it to collide with another ship\n");
          Log("Please enter a new X coordinate");
          cin >> x;
          
          Log("Please enter a new Y coordinate");
          cin >> y;
        }

        ClearConsole();
        printPlacementTitle();
        Log(getName(), ", place your ", _ships[i].getName() + " (length: " + to_string(_ships[i].getLength()) + ")");
        Log();
        viewBoard(xSize, ySize, x, y);
        Log();

        Log("Enter heading, up, down, left, right (u/d/l/r)"); // get user to input direction
        cin >> direction;

        while(direction != 'u' && direction != 'd' && direction != 'l' && direction != 'r' ){ //validate direction
          Log("Please enter a valid heading (u/d/l/r)");
          cin >> direction;
        }

        bool canPlace = false;

        while(!canPlace){
          string selected;

          if(direction == 'r'){ // HORIZONTAL - HEADING RIGHT
            if(!(x + _ships[i].getLength() >= xSize) && !checkIfCollision(x, y, direction, _ships[i].getLength())){ // check if the ship will go off of the map & if it will not intersect with another ship
              canPlace = true;
              break;
            }
            selected = "right";
          }

          if(direction == 'l'){ // HORIZONTAL - HEADING LEFT
            if(!(x - _ships[i].getLength() <= 0) && !checkIfCollision(x, y, direction, _ships[i].getLength())){
              canPlace = true;
              break;
            }
            selected = "left";
          } 

          if(direction == 'u'){ //VERTICAL - HEADING UP
            if(!(y + _ships[i].getLength() >= ySize) && !checkIfCollision(x, y, direction, _ships[i].getLength())){
              canPlace = true;
              break;
            }
            selected = "up";
          }

          if(direction == 'd'){ //VERTICAL - HEADING DOWN
            if(!(y - _ships[i].getLength() <= 0) && !checkIfCollision(x, y, direction, _ships[i].getLength())){
              canPlace = true;
              break;
            }
            selected = "down";
          }

          Log("Your ", _ships[i].getName(), " is too long to be placed at " + to_string(x) + ", " + to_string(y) + ", heading " + selected);
          Log("Choose a different heading (u/d/l/r)");
          cin >> direction;
        }

        _ships[i].updateBulkheads(x--, y--, xSize, ySize, direction);
        placeShip(_ships[i]);
        ClearConsole();
        printPlacementTitle();
        Log();
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

    //DEBUG function to print a users occupied coords to the console;
    void printOccupied(){
      Log("Printing occupied coords for ", getName());
      for(int i = 0; i < _occupied.size(); i++){
        Log(to_string(_occupied[i].x), ", ",to_string(_occupied[i].y));
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
      printConfigTitle();

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
      printPlayersTitle();

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
      printConfigTitle();

      Log("Please enter the size of the X axis for player boards");
      cin >> x;

      Log("Please enter the size of the Y axis for player boards");
      cin >> y;

      setBoardSize(x, y);
    }

    void startNewGame(){ // controls sequence of events for a game to be played;
      deleteAllUsers(); //reset the game by destroying all active players;
      ClearConsole();
      printTitle();
      enterToContinue();

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
          updateUsers(); // calling when stage == 1;
          Log("Placement ended => STATE = 2 (Play)  calling updateUsers on line 700");
          updateUsers(); // calling when stage == 2;
        }
      }
    }

    void updateUsers(){
      if(_stage == 1){ // if stage is 'placement' - get players to place ships;
        for(int i = 0; i < _users.size(); i++){
          _users[i].placeShips(getBoardSize().x, getBoardSize().y); // get user to place their boats;
          enterToContinue(); // wait for user to input to continue
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
      printPlayersTitle();

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


  clsShip carrier("Aircraft Carrier", 5);
  clsShip battleship("Battleship", 4);
  clsShip submarine("Submarine", 3);
  clsShip cruiser("Cruiser", 3);
  clsShip patrolBoat("Patrol Boat", 2);

  state -> registerShip(carrier); 
  state -> registerShip(battleship); 
  state -> registerShip(submarine); 
  state -> registerShip(cruiser); 
  state -> registerShip(patrolBoat); 

  state -> startNewGame();
}
