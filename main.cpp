#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <typeinfo>
#include <limits>
#include <unistd.h>


using namespace std;

/* 
Name: Alexander Weller
email: alexander.weller@ada.ac.uk

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

struct bulkhead{
  int x = NOVALUE;
  int y = NOVALUE;
  bool hit = false;
};

struct udtCoord{
  int x;
  int y;
};

vector <string> roundEvents;

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

int getIntLength(int i){ // TODO: can go in utils class
  return trunc(log10(i)) + 1;
  // using log10 -> returns the value y in base 10. 
  // using trunc -> returns a rounded down result of the log10 function; 
  // example: (log10(100) = 2) + 1 = 3 -> 100 is a 3 digit number;
}

int rollDice(int mod){
  int random_number = (rand() % mod) + 1; // call the rand() function to get a random number, the param 'mod' is then used to calculate the modulus of the result + 1; if mod = 10, the random number will be between 1 & 10;
  return random_number;
}

void printRoundEvents(){
  Log("Round History");
  Log("________________________________");
  Log("════════════════════════════════");
  for(int i = 0; i < roundEvents.size(); i++){
    cout << to_string(i+1) + ". " << roundEvents[i] << "\n";
  }
  Log("________________________________");
  Log("════════════════════════════════");
}

void yToContinue(){
  char input;
  Log("Continue...? (y)");
  
  do {
    cin >> input;
  }while(input != 'y');
}

void enterToContinue(){
  Log("Press enter to continue...");
  cin.ignore();
}

void printRed(string message){
  cout << "\x1B[31m" + message + "\033[0m";
}

string setRed(string message){
  return "\x1B[31m" + message + "\033[0m";
}

string setBrightGreen(string message){
  return "\x1B[92m" + message + "\033[0m";
}

string setBrightGreen(char letter){
  string result = "\x1B[92m";
  string escSeq1 = "\033[0m";

  result.push_back(letter);


  return result + escSeq1;
}

string setCyan(string message){
  return "\x1B[96m" + message + "\033[0m";
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

string setYellow(string message){
  return "\x1B[93m" + message + "\033[0m";
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

void printSelectTargetTitle(){
  string line1 = "   ____      __           __    ______                       __  ";
  string line2 = "  / __/___  / /___  ____ / /_  /_  __/___ _ ____ ___ _ ___  / /_ ";
  string line3 = " _\\ \\ / -_)/ // -_)/ __// __/   / /  / _ `// __// _ `// -_)/ __/ ";
  string line4 = "/___/ \\__//_/ \\__/ \\__/ \\__/   /_/   \\_,_//_/   \\_, / \\__/ \\__/  ";
  string line5 = "                                               /___/             ";
  
  Log(setGreen(line1));
  Log(setGreen(line2));
  Log(setGreen(line3));
  Log(setGreen(line4));
  Log(setGreen(line5));
  Log();  
}

void printBattleTitle(){
  string line1 = "   ___         __   __   __    ";
  string line2 = "  / _ ) ___ _ / /_ / /_ / /___ ";
  string line3 = " / _  |/ _ `// __// __// // -_)";
  string line4 = "/____/ \\_,_/ \\__/ \\__//_/ \\__/ ";
                               
  Log(setGreen(line1));
  Log(setGreen(line2));
  Log(setGreen(line3));
  Log(setGreen(line4));
  Log();  
}

void printRoundOverTitle(){
  string line1 = "   ___                      __  ____                 ";
  string line2 = "  / _ \\ ___  __ __ ___  ___/ / / __ \\ _  __ ___  ____";
  string line3 = " / , _// _ \\/ // // _ \\/ _  / / /_/ /| |/ // -_)/ __/";
  string line4 = "/_/|_| \\___/\\_,_//_//_/\\_,_/  \\____/ |___/ \\__//_/   ";
                                                     

  Log(setGreen(line1));
  Log(setGreen(line2));
  Log(setGreen(line3));
  Log(setGreen(line4));
  Log(); 
}

void printGameOverTitle(){
  string line1 = "  _____                   ____                   __";
  string line2 = " / ___/___ _ __ _  ___   / __ \\ _  __ ___  ____ / /";
  string line3 = "/ (_ // _ `//  ' \\/ -_) / /_/ /| |/ // -_)/ __//_/ ";
  string line4 = "\\___/ \\_,_//_/_/_/\\__/  \\____/ |___/ \\__//_/  (_)  ";
                                                   

  Log(setGreen(line1));
  Log(setGreen(line2));
  Log(setGreen(line3));
  Log(setGreen(line4));
  Log(); 
}

void ClearConsole(){
  cout << "\033[2J\033[0;0H"; // escape sequence that clears the console;
}

void progressBar(){
  float progress = 0.0;
  
  while (progress < 1.0) {
      int barWidth = 70;

      cout << "[";
      int pos = barWidth * progress;
      
      for (int i = 0; i < barWidth; ++i) {
          if (i < pos) cout << "=";
          else if (i == pos) cout << ">";
          else cout << " ";
      }
      cout << "] " << int(progress * 100.0) << " %\r";
      cout.flush();

      progress += 0.16; // for demonstration only
  }
  cout << endl;
}

string cleanString(string text){
  string _local = text;
  cout << endl << "BEFORE: [" <<_local << "]\n";//TODO: remove this

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

  cout << "AFTER : [" <<_local << "]"; // TODO: Remove this

  return _local;
};

void printBoardKey(){
  Log("Key:");
  Log(setGreen("█") + " = Friendly battleship");
  Log(setYellow("☼") + " = Missed torpedo attack");
  Log(setRed("X") + " = Torpedo attack hit a battleship");
  Log();
}
 

// Easy CPU Functions
udtCoord cpuEasySelectCoords(int xSize, int ySize){
  udtCoord _tempCoord;

  _tempCoord.x = rollDice(xSize); //get a random number between 1 & xSize
  _tempCoord.y = rollDice(ySize); //get a random number between 1 & ySize
  return _tempCoord;
}

char cpuEasySelectHeading(){
  int selected = rollDice(4); //get a random number inclusively between 1 & 4

  switch(selected){
    case 1 : return 'u';
    case 2 : return 'd';
    case 3 : return 'l';
    case 4 : return 'r';
  }
}

int cpuSelectRandomTarget(int activePlayerCount, int userIdIgnore){
  int randId;

  do{
    randId = rollDice(activePlayerCount);
    cout << "randId = " << randId << endl;
  } while(randId == userIdIgnore);

  cout << "randId: " << randId << ", ignoreId: " << userIdIgnore << endl;
  return randId;
}
////

//SHIP CLASS
class clsShip{
  public:
    clsShip(string name, int length){
      _name = name;
      _length = length;
      _health = length;
      buildBulkheads();
    }

    void buildBulkheads(){
      for(int i = 0; i < _length; i++){
        bulkhead newBulkhead;
        _bulkheads.push_back(newBulkhead);
      }
    }

    void updateBulkheads(int x, int y, int xSize, int ySize, char direction){ // update all bulkheads of a ship to be places on the gameboard;
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

    int getHealth(){
      return _health;
    }

    void decrementHealth(){
      _health--;
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

    void setAnnounced(){
      _announced = true;
    }

    bool getAnnounced(){
      return _announced;
    }

  private:
    int _length;
    int _health;
    bool _announced = false;
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

    bool checkCollision(int x, int y, bool target = false){ //Check if an origin coord is occupied
    //add validation - can currently place coords outside of map
    //validateOriginCoord(); requires boardsize
    if(target){
      for(int i = 0; i < _attacked.size(); i++){
        if(_attacked[i].x == x && _attacked[i].y == y){
          return true;
        }
      } 
    } else {
      for(int i = 0; i < _occupied.size(); i++){
        if(_occupied[i].x == x && _occupied[i].y == y){
          return true;
        }
      } 
    }
      return false;
    }

    bool checkCollision(int x, int y, char direction, int length){// check all points that a ship will occupy based on direction
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

      for(int o = 0; o < _occupied.size(); o++){ //for each coordinate that is already occupied...
        for(int s = 0; s < shipPoints.size(); s++){ // check each 'shipPoints' coord if it matches
          if(_occupied[o].x == shipPoints[s].x && _occupied[o].y == shipPoints[s].y){
            return true; // if a match is found, return true
          }
        }
      }
      return false; //otherwise return false;
    }

    bool hasBeenAttacked(int x, int y){
      for(int i = 0; i < _attacked.size(); i++){
        if(_attacked[i].x == x && _attacked[i].y == y){
          // Log("Found matching attacked coord"); // DEBUG
          return true;
        }
      }
      // Log("No matching attacked coord found"); //DEBUG
      return false;
    }

    bool validateOriginCoord(int xSize, int ySize, int point){
      if(point > xSize || point < 1 || point > ySize){
        return false;
      }
      return true;
    }

    void viewBoard(int xSize, int ySize, bool target = false){ // view a players board, optional param 'target' will change if the board is drawn with occupied spaces or spaces that have been fired at;
      if(target){
        Log("Targeting ", getName(), "'s board");
      } else {
        Log("Your (", getName(), "'s) board");
      }

      cout << "   ╔";
      for(int x = 1; x <= xSize * 2; x++){ // print gameboard border graphics
        cout << "═";
      }
      cout << "╗";
      Log();

      for(int y = ySize; y >= 1; y--){
        int yLen = getIntLength(y); // get the num of digits of current iterator y

        if(yLen == 1){ // if y is a single digit, use this spacing
            cout << setCyan(to_string(y)) << "  ╠ ";
          } else if(yLen == 2){ // if y is double digit, change spacing;
            cout << setCyan(to_string(y)) << " ╠ ";
          } else { // else print standard space;
            cout << setCyan(to_string(y)) << "╠ ";
          }
        for(int x = 1; x <= xSize; x++){
          if(getAttackedOrOccupied(x, y, target)){ //if the current coord matches an already occupied or attacked coord (based on the param 'target')
            if(target){ //if target is true...
              if(checkCollision(x, y)){//...and this point contains a ship 
                cout << setRed("X ");// print an orange X if this coord has been attacked and is a hit;
              } else {
                cout << setYellow("☼ "); // print a yellow x if this coord has been attacked but is a miss;
              }
            } else if (checkCollision(x, y, true)){ // add another if here to check if the ship has been sunk <- ALEX HERE (TODO / DEBUG)
              cout << setRed("X "); // print an X if this coord is occupied by a ship and has been hit;
            } else if (checkCollision(x, y)){ 
              cout << setGreen("█ "); // print an S if this coord is occupied by a ship;
            } 
          }
          else if(hasBeenAttacked(x, y)){
              cout << setYellow("☼ "); // print a yellow x if this coord has been hit but is not occupied by a ship;
            } else if (x == xSize){
            cout << "▬";
          } else {
            cout << "▬ ";
          }
        }
        Log();
      }

      cout << "   ╚";
      for(int x = 1; x <= xSize * 2; x++){ // print gameboard graphics
        cout << "═";
      }
      cout << "╝";
      Log();

      cout << "     "; //5 chars
      for(int x = 1; x <= xSize; x++){ // print x axis label;
        if(getIntLength(x) == 2){ //if x is a 2 digit number...
          string strx = to_string(x);//store x as a string;

          cout << setBrightGreen(strx[0]); //print first digit of x;
          cout << "\033[1D" << "\033[1B";//move cursor down 1 row & back 1 column;
          cout << setBrightGreen(strx[1]); //print 2nd digit of x;
          cout << "\033[1C" << "\033[1A";//move cursor forward & up 1 space
        } else {
          cout << setBrightGreen(to_string(x)) << " ";
        }
      }
      Log();
      Log();
    }

    void viewBoard(int xSize, int ySize, int selectedX, int selectedY){ // view a players board with a point highlighted; using function overloading here to alter the behaviour of this func;
      Log("Your (", getName(), "'s) board");

      cout << "   ╔";
      for(int x = 1; x <= xSize * 2; x++){ // print x axis labels
        cout << "═";
      }
      cout << "╗";
      Log();
      
      for(int y = ySize; y >= 1; y--){
        if(getIntLength(y) == 1){
          cout << setCyan(to_string(y)) << "  ╠ ";
        } else if(getIntLength(y) == 2){ // if y is double digit, change spacing;
          cout << setCyan(to_string(y)) << " ╠ ";
        } else { // else print standard space;
          cout << setCyan(to_string(y)) << "╠ ";
        }
        
        for(int x = 1; x <= xSize; x++){
          if(getAttackedOrOccupied(x, y)){
            printGreen("█ ");
          } else if(x == selectedX && y == selectedY){
            printRed("▬ "); // prints the selected coordinate in green;
          } else if (x == xSize){
            cout << "▬ ";
          } else {
            cout << "▬ ";
          }
        }
        // cout << "║";  
        Log();
      }

      cout << "   ╚";
      for(int x = 1; x <= xSize * 2; x++){ // print x axis labels
        cout << "═";
      }
      cout << "╝";
      Log();
      cout << "     ";
      for(int x = 1; x <= xSize; x++){ // print x axis labels
        if(getIntLength(x) == 2){ //if x is a 2 digit number...
          string strx = to_string(x);//store x as a string;

          cout << setBrightGreen(strx[0]); //print first digit of x;
          cout << "\033[1D" << "\033[1B";//move cursor down 1 row & back 1 column;
          cout << setBrightGreen(strx[1]); //print 2nd digit of x;
          cout << "\033[1C" << "\033[1A";//move cursor forward & up 1 space
        } else {
          cout << setBrightGreen(to_string(x)) << " ";
        }
      }
      Log();
      Log();
    }

    void placeFleet(int xSize, int ySize){
      for(int i = 0; i < _ships.size(); i++){
        int x, y;
        char heading;
        bool canPlace = false;

        if(!isCPU()){
          ClearConsole();
          printPlacementTitle();

          viewBoard(xSize, ySize);
          Log(getName(), ", place your ", _ships[i].getName() + " (length: " + to_string(_ships[i].getLength()) + ")");
          Log();

          Log("Enter " + setBrightGreen("X") + " coord");
          cin >> x;

          while(!validateOriginCoord(xSize, ySize, x) || !cin){ //validate x coord
            Log("Invalid " + setBrightGreen("X") + " coordinate entered, please enter a coordinate between 1, " ,to_string(xSize));
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> x;
          }
          
          Log("Enter " + setCyan("Y") + " coord");
          cin >> y;

          while(!validateOriginCoord(xSize, ySize, y) || !cin){ //validate y coord
            Log("Invalid " + setCyan("Y") + " coordinate entered, please enter a coordinate between 1, " ,to_string(ySize));
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> y;
          }

          while(checkCollision(x, y)){ // checks if origin point collides with another ship
            Log("Placing your " + _ships[i].getName() + " here, will cause it to collide with another ship\n");

            Log("Please enter a new " + setBrightGreen("X") + " coordinate");
            cin >> x;

            while(!validateOriginCoord(xSize, ySize, x) || !cin){ //validate x coord
              Log("Invalid X coordinate entered, please enter a coordinate between 1, " ,to_string(xSize));
              cin.clear();
              cin.ignore(numeric_limits<streamsize>::max(), '\n');
              cin >> x;
            }
            
            Log("Please enter a new " + setCyan("Y") + " coordinate");
            cin >> y;

            while(!validateOriginCoord(xSize, ySize, y) || !cin){ //validate y coord
              Log("Invalid Y coordinate entered, please enter a coordinate between 1, " ,to_string(ySize));
              cin.clear();
              cin.ignore(numeric_limits<streamsize>::max(), '\n');
              cin >> y;
            }
          }

          ClearConsole();
          printPlacementTitle();
          viewBoard(xSize, ySize, x, y);
          Log();

          Log("Enter heading, up, down, left, right (u/d/l/r)"); // get user to input heading
          cin >> heading;

          while((heading != 'u' && heading != 'd' && heading != 'l' && heading != 'r') || !cin){ //validate heading - if its not u/d/l/r or is invalid type...
            cout << "\e[2F"; //move cursor up 2 lines;
            cout << "\e[0J"; // clear screen from cursor down;
            Log("Please enter a " + setRed("valid") + " heading (u/d/l/r)");
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> heading;
          }

          while(!canPlace){ // validate heading
            string selected;

            if(heading == 'r'){ // HORIZONTAL - HEADING RIGHT
              if(!(x + _ships[i].getLength() - 1 > xSize) && !checkCollision(x, y, heading, _ships[i].getLength())){ // check if the ship will go off of the map OR if it will intersect with another ship
                canPlace = true;
                break; // using break here to prevent unnecesary error logging after successful placement;
              }
              selected = "right";
            }

            if(heading == 'l'){ // HORIZONTAL - HEADING LEFT
              if(!(x - _ships[i].getLength() < 0) && !checkCollision(x, y, heading, _ships[i].getLength())){ // check if the ship will go off of the map OR if it will intersect with another ship
                canPlace = true;
                break;
              }
              selected = "left";
            } 

            if(heading == 'u'){ //VERTICAL - HEADING UP
              if(!(y + _ships[i].getLength() - 1 > ySize) && !checkCollision(x, y, heading, _ships[i].getLength())){ // check if the ship will go off of the map OR if it will intersect with another ship
                canPlace = true;
                break;
              }
              selected = "up";
            }

            if(heading == 'd'){ //VERTICAL - HEADING DOWN
              if(!(y - _ships[i].getLength() < 0) && !checkCollision(x, y, heading, _ships[i].getLength())){ // check if the ship will go off of the map OR if it will intersect with another ship
                canPlace = true;
                break;
              }
              selected = "down";
            }

            Log("Your ", _ships[i].getName(), " is too long to be placed at " + to_string(x) + ", " + to_string(y) + ", heading " + selected);
            Log("Choose a different heading (u/d/l/r)");
            cin >> heading;
          }

          _ships[i].updateBulkheads(x--, y--, xSize, ySize, heading); //update the currently selected ships bulkheads, with adjusted coords as the board is base 0;
          placeShip(_ships[i]); //Add the selected ships coords to the _occupied variable;
          ClearConsole();
          printPlacementTitle();
          viewBoard(xSize, ySize);//View the board after placement for visual confirmation;

        } else {
          bool canPlace = false;
          udtCoord _cpuCoords;
          char heading;

          ClearConsole();

          // Log("CPU PLACEMENT"); // DEBUG

          do{
            _cpuCoords = cpuEasySelectCoords(xSize, ySize); // select random x & y coords -> validate coords 
          } while(checkCollision(_cpuCoords.x, _cpuCoords.y) || !validateOriginCoord(xSize, ySize, _cpuCoords.x) || !validateOriginCoord(xSize, ySize, _cpuCoords.x));

          // Log("Coords generated: " + to_string(_cpuCoords.x) + " " + to_string(_cpuCoords.y)); // DEBUG

          //select a random heading
          while(!canPlace){ // validate heading
            string selected;        
            heading = cpuEasySelectHeading(); // Assign a random heading to the variable;

            // string msg = "Heading generated: ";// DEBUG
            // msg.push_back(heading); // DEBUG
            // Log(msg); // DEBUG

            if(heading == 'r'){ // HORIZONTAL - HEADING RIGHT
              if(!(_cpuCoords.x + _ships[i].getLength() - 1 > xSize) && !checkCollision(_cpuCoords.x, _cpuCoords.y, heading, _ships[i].getLength())){ // check if the ship will go off of the map OR if it will intersect with another ship
                canPlace = true;
                break; // using break here to prevent unnecesary error logging after successful placement;
              }
            }

            if(heading == 'l'){ // HORIZONTAL - HEADING LEFT
              if(!(_cpuCoords.x - _ships[i].getLength() < 0) && !checkCollision(_cpuCoords.x, _cpuCoords.y, heading, _ships[i].getLength())){ // check if the ship will go off of the map OR if it will intersect with another ship
                canPlace = true;
                break;
              }
            } 

            if(heading == 'u'){ //VERTICAL - HEADING UP
              if(!(_cpuCoords.y + _ships[i].getLength() - 1 > ySize) && !checkCollision(_cpuCoords.x, _cpuCoords.y, heading, _ships[i].getLength())){ // check if the ship will go off of the map OR if it will intersect with another ship
                canPlace = true;
                break;
              }
            }

            if(heading == 'd'){ //VERTICAL - HEADING DOWN
              if(!(_cpuCoords.y - _ships[i].getLength() < 0) && !checkCollision(_cpuCoords.x, _cpuCoords.y, heading, _ships[i].getLength())){ // check if the ship will go off of the map OR if it will intersect with another ship
                canPlace = true;
                break;
              }
            }
          }

          _ships[i].updateBulkheads(_cpuCoords.x, _cpuCoords.y, xSize, ySize, heading); //update the currently selected ships bulkheads;
          placeShip(_ships[i]); //Add the selected ships coords to the _occupied variable;

          // Log("DEBUG: VIEWING CPU PLAYER BOARD -> " + getName());//DEBUG
          // viewBoard(xSize, ySize);//DEBUG
        }
      }
    }
    
    void placeShip(clsShip ship){
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

    void updateShips(int x, int y){
      bool missed = true;

      for(int i = 0; i < _ships.size(); i++){ // for each registered ship
        for(int j = 0; j < _ships[i].getBulkheads().size(); j++){ //for each bulkhead
          if(x == _ships[i].getBulkheads()[j].x && y == _ships[i].getBulkheads()[j].y){ // if the passed in coords match a bulkhead;
            missed = false;
            roundEvents.push_back(getName() + "'s " + _ships[i].getName() + " has been " + setRed("hit!")); // If a ship has been hit, add it to the message queue;
            _ships[i].getBulkheads()[j].hit = true; // set the current bulkheads status hit = true;
            _ships[i].decrementHealth(); // decrease the current ships health by 1;
          }
        }

        if(_ships[i].getHealth() == 0 && !_ships[i].getAnnounced()){ //if any of the updated ships have been destroyed & they have not been announced;
          _ships[i].setAnnounced();
          roundEvents.push_back(getName() + "'s " + _ships[i].getName() + " has been " + setRed("destroyed!")); // Add this to the message queue
        }
      } 
      if(missed){
        roundEvents.push_back(getName() + " was attacked at: " + to_string(x) + ", " + to_string(y) + " but the shot " + setYellow("missed")); // add the attacked coord to the message queue;
      }
    }

    void addAttacked(int x, int y){
      udtCoord coordinate;

      //iterate over ships - check for matching coords - set 'hit' to true;
      updateShips(x, y);

      coordinate.x = x; // assign the param values to a coordinate
      coordinate.y = y;
      _attacked.push_back(coordinate); //push it to the attacked vector;
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

    bool isCPU(){
      return _cpu;
    }

    int calculateHealth(){ // check each ship that a player owns & check if they have remaining bulkheads;
      int remainingShips = 0;

      for(int i = 0; i < _ships.size(); i++){
        if(_ships[i].getHealth() != 0){
          remainingShips++;
        }
      }

      return remainingShips;
    }

    void setInactive(){
      _active = false;
    }

    bool isActive(){
      return _active;
    }

  private:
    int _id;
    string _name;
    vector < udtCoord > _occupied; // represents a players personal board;
    vector < udtCoord > _attacked; // represents where users have fired at this users board;
    vector < clsShip > _ships;
    bool _active = true;
    bool _cpu = false;
}; // TODO: Maybe a class for CPU players inheirits clsUser & includes the AI in that inheirited class?

//GAMESTATE CLASS - Combined Singleton, State Machine & observer Design Patterns
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

    void initPlayerCount(){
      int playerCount;

      ClearConsole();
      printConfigTitle();

      Log("Enter the number of players: ");
      cin >> playerCount;
      
      setPlayerCount(playerCount);
    }

    bool checkIfDuplicateUser(string newName){
      for(int i = 0; i < _users.size(); i++){
        if(_users[i].getName() == newName){
          return true;
        }
      }
      return false;
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

        Log("Is ", name, " a CPU player? (y/n)");
        while(userChoice != 'y' && userChoice != 'n'){
          cin >> userChoice;
        }

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

      Log("Please enter the size of the " + setBrightGreen("X") + " axis for player boards");
      cin >> x;

      Log("Please enter the size of the " + setCyan("Y") + " axis for player boards");
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

      while(userChoice != 'y' && userChoice != 'n'){
        Log("Ready to start the game? (y/n)");
        cin >> userChoice;
      }

      if(userChoice == 'n'){
        startNewGame();
      }

      if(userChoice == 'y'){
        ClearConsole();
        setState(1);
        updateUsers(); // calling when stage == 1;
        updateUsers(); // calling when stage == 2;
        updateUsers(); // calling when stage == 3;
      }
    }

    int getActivePlayers(){
      int count = 0;

      for(int i = 0; i < _users.size(); i++){
        if(_users[i].isActive()){
          count++;
        }
      }
      return count;
    }

    void updateUsers(){
      if(_state == 1){ // if stage is 'placement' - get players to place ships;
        for(int i = 0; i < _users.size(); i++){
          _users[i].placeFleet(getBoardSize().x, getBoardSize().y); // get user to place their boats;
          
          if(!_users[i].isCPU()){
            Log("Placement complete...");
            Log();
            enterToContinue();
          }
        }
        setState(2);

      } else if(_state == 2){ // if stage is 'play' - cycle through users to choose target & attack;
        while(getActivePlayers() > 1){ // while there is more than 1 active player
          for(int i = 0; i < _users.size(); i++){ // for each user
            if(_users[i].calculateHealth() == 0){ // if the user is dead;
                _users[i].setInactive();
                roundEvents.push_back(setRed("!!! All of " + _users[i].getName() + "'s ships have been destroyed !!!"));

              } else if(_users[i].isActive() && !_users[i].isCPU()){ // if the player is active & not a CPU player;
              pair <bool, int> foundUser;
              int targetId = 0;

              ClearConsole();
              printBattleTitle();
              printAllUsers(_users[i].getId());

              // VALIDATE PLAYER ID INPUT
              Log(_users[i].getName(), ", enter the " + setYellow("ID") + " of the player you want to attack: ");
              cin >> targetId;
              foundUser = checkUserExistsById(targetId);

              while(!cin || _users[i].getId() == targetId || !foundUser.first){// ask for a target id while the id is invalid / the id is of the current player / the type of input is invalid;
                cout << "\e[2F"; //move cursor up 4 lines;
                cout << "\e[0J"; // clear screen from cursor down;
                // Log();
                Log(_users[i].getName() + ", enter a " + setRed("valid") + " user " + setYellow("ID"));

                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin >> targetId;
                foundUser = checkUserExistsById(targetId);
              }
              ////

              ClearConsole();
              printBattleTitle();

              getUserById(foundUser.second).viewBoard(getBoardSize().x, getBoardSize().y, true); // view targets board;
              _users[i].viewBoard(getBoardSize().x, getBoardSize().y); // View current player board;
              printBoardKey();

              udtCoord validatedAttackCoord = selectTargetCoords(getUserById(foundUser.second), getBoardSize().x, getBoardSize().y); // get user to select attack coords;
              getUserById(foundUser.second).addAttacked(validatedAttackCoord.x, validatedAttackCoord.y); // add the validated attack coordinate to the targets board;
              
              ClearConsole();
              printBattleTitle();
              getUserById(foundUser.second).viewBoard(getBoardSize().x, getBoardSize().y, true); //view the targets board again with hit/miss feedback;
              _users[i].viewBoard(getBoardSize().x, getBoardSize().y); // View current player board;
              enterToContinue();
              

            } else if(_users[i].isActive() && _users[i].isCPU()){ // is active & isCPU
              int targetId = cpuSelectRandomTarget(getActivePlayers(), _users[i].getId()); // generate a random target
              int targetIndex = checkUserExistsById(targetId).second; //find the user in _users - return the index of the found user in _users;
              udtCoord attackCoord;

              ClearConsole();
              printBattleTitle();

              // CPU player chooses target;
              getUserById(targetIndex).viewBoard(getBoardSize().x, getBoardSize().y, true); //view the targets board before attacking;
              Log(_users[i].getName() + " is targeting " + getUserById(targetIndex).getName());
              // enterToContinue();// CHANGE TO WAIT
              progressBar();
              sleep(2);


              // CPU player chooses target coords;
              do {
              attackCoord = cpuEasySelectCoords(getBoardSize().x, getBoardSize().y); // generate random attack coords;
              } while(!getUserById(targetIndex).validateOriginCoord(getBoardSize().x, getBoardSize().y, attackCoord.x) || //if x coord..
                      !getUserById(targetIndex).validateOriginCoord(getBoardSize().x, getBoardSize().y, attackCoord.y) || //or y coord is invalid
                      getUserById(targetIndex).checkCollision(attackCoord.x, attackCoord.y, true)   //or the attack coord has already been attacked 
                    );

              Log(_users[i].getName() + " is attacking " + getUserById(targetIndex).getName() + " at: " + to_string(attackCoord.x) + ", " + to_string(attackCoord.y)); // DEBUG
              Log();

              getUserById(targetIndex).addAttacked(attackCoord.x, attackCoord.y); // add the validated attack coordinate to the targets board;

              ClearConsole();
              printBattleTitle();
              getUserById(targetIndex).viewBoard(getBoardSize().x, getBoardSize().y, true); //view the targets board again with hit/miss feedback;

              enterToContinue(); //TODO: CHANGE TO WAIT
            }
          }
          ClearConsole();
          printRoundOverTitle();
          printRoundEvents();
          roundEvents.clear();
          enterToContinue();
        }
        setState(3);

      } else if(_state == 3){ // if stage is 'winner' - display winner screen to remaining player(s);
        char input;
        
        ClearConsole();
        printGameOverTitle();
        Log(setGreen(_users[0].getName() + " HAS WON!"));
        Log();

        do{
          if(input == 'y'){
            startNewGame();
          } else if(input == 'n'){
            break;
          }
          Log("PLAY AGAIN? (y/n)");
          cin >> input;
        } while(input != 'y' || input != 'n'); 
      }
    }

    udtCoord selectTargetCoords(clsUser targetUser, int xSize, int ySize){
      udtCoord attackCoord;
      
      do {
        if(!targetUser.validateOriginCoord(xSize, ySize, attackCoord.x) || !targetUser.validateOriginCoord(xSize, ySize, attackCoord.y) || !cin){ // only print here on 2rd execution of this loop
          cout << "\e[4F"; //move cursor up 4 lines;
          cout << "\e[0J"; // clear screen from cursor down;

          Log(setRed("!Please enter valid coordinates!"));
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
          
          Log();
          Log("Enter the " + setBrightGreen("X") + " coordinate that you want to attack"); // convert to "do while"
          cin >> attackCoord.x;

          Log("Enter the " + setCyan("Y") + " coordinate that you want to attack");
          cin >> attackCoord.y;
        } else { // run this block first
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');

          Log("Enter the " + setBrightGreen("X") + " coordinate that you want to attack"); // convert to "do while"
          cin >> attackCoord.x;

          Log("Enter the " + setCyan("Y") + " coordinate that you want to attack");
          cin >> attackCoord.y;
        }
      } while(!targetUser.validateOriginCoord(xSize, ySize, attackCoord.x) || !targetUser.validateOriginCoord(xSize, ySize, attackCoord.y) || !cin); // check entered coords exist within the map;

      while(targetUser.checkCollision(attackCoord.x, attackCoord.y, true)){// while the attack coords have already been attacked OR the attack coords do not fit on the board -> ask for new coords
        cout << "\e[4F"; //move cursor up 4 lines;
        cout << "\e[0J"; // clear screen from cursor down;

        Log("This coordinate has already been attacked, please enter new coordinates");

        Log("Enter the " + setBrightGreen("X") + " coordinate that you want to attack");
        cin >> attackCoord.x;

        Log("Enter the " + setCyan("Y") + " coordinate that you want to attack");
        cin >> attackCoord.y;
      }
      return attackCoord;
    }

    void printAllUsers(){ // print all registered users;
      ClearConsole();
      printPlayersTitle();

      Log("ID ║ Name");
      Log("═══╬════════════════════════════");
      for(int i = 0; i < _users.size(); i++){
        string isCPU = _users[i].isCPU() ? "(CPU)" : "";

        if(getIntLength(i) == 2){
          Log(setYellow(to_string(_users[i].getId())) + " ║ " + _users[i].getName() + " " + isCPU);
        } else {
          Log(setYellow(to_string(_users[i].getId())) + "  ║ " + _users[i].getName() + " " + isCPU);
        }
      }    
      Log("═══╩════════════════════════════");
      Log();
    };

    void printAllUsers(int callerId){ //print all users excluding the user that matches the passes id;
      ClearConsole();
      printSelectTargetTitle();

      Log("ID ║ Name");
      Log("═══╬════════════════════════════");
      for(int i = 0; i < _users.size(); i++){
        if(_users[i].getId() != callerId){
          if(getIntLength(i) == 2){
          Log(setYellow(to_string(_users[i].getId())), " ║ " , _users[i].getName());
        } else {
          Log(setYellow(to_string(_users[i].getId())), "  ║ " , _users[i].getName());
          }
        }
      }
      Log("═══╩════════════════════════════");
    };

    void registerUser(clsUser& user){ // register a user to the gamestate by adding it to the _users vector;
      _users.push_back(user);
    }

    // vector < clsUser > getUsers(){ NOT USED
    //   return _users;
    // }

    pair<bool, int> checkUserExistsById(int id){
      pair <bool, int> result;
      result.first = false;
      result.second = -1;

      for(int i = 0; i < _users.size(); i++){
        if(_users[i].getId() == id){
          result.first = true; // if user is found
          result.second = i; // record the index the user was found at;
        }
      }
      return result;
    }

    clsUser& getUserById(int index){
      return _users[index];
    }

    void deleteAllUsers(){
      if(_users.size()){
        _users.clear();
      }
    }

    void registerShip(clsShip ship){ // registers a ship class to the gamestate to configure new user fleets;
      _fleetConfig.push_back(ship);
    }

    // void removeUserById(int id){
    //   for(int i = 0; i < _users.size(); i++){
    //     if(_users[i].getId() == id){
    //       Log("Setting ", _users[i].getName(), " as inactive");
    //       _users[i].setInactive();
    //     }
    //   }
    // }

    void setState(int i){ // Debug func;
      _state = i;
    }

  private:
    int _state = 0;
    int _activePlayer = 0;
    int _playerCount;
    vector < clsUser > _users;
    vector < clsShip > _fleetConfig;
    udtCoord _boardSize;
    static clsGamestate* _inst;
};


clsGamestate* clsGamestate::_inst = NULL;

int main(){
  srand(time(NULL)); // Seed random values generated with rand() with the current time;
  clsGamestate* state; // set variable 'Gamestate' as a pointer;
  state = clsGamestate::getInstance(); // assign the instance of clsGamestate;

  vector <clsShip> shipConfig;

  clsShip carrier("Aircraft Carrier", 5);
  clsShip battleship("Battleship", 4);
  clsShip submarine("Submarine", 3);
  clsShip cruiser("Cruiser", 3);
  clsShip patrolBoat("Patrol Boat", 2);

  shipConfig.push_back(carrier);
  shipConfig.push_back(battleship);
  shipConfig.push_back(submarine);
  shipConfig.push_back(cruiser);
  shipConfig.push_back(patrolBoat);

  // state -> registerShip(carrier); 
  // state -> registerShip(battleship); 
  // state -> registerShip(submarine); 
  // state -> registerShip(cruiser); 
  // state -> registerShip(patrolBoat); 

  // clsUser user1("Alex", 1, false, shipConfig);
  clsUser user2("Sofia", 1, true, shipConfig);
  clsUser user3("Jimmy", 2, true, shipConfig);
  clsUser user4("Tim", 3, true, shipConfig);
  clsUser user5("Boz", 4, true, shipConfig);

  // state -> registerUser(user1);
  state -> registerUser(user2);
  state -> registerUser(user3);
  state -> registerUser(user4);
  state -> registerUser(user5);

  state -> setState(1);
  state -> setBoardSize(10, 10);

  state -> updateUsers();
  state -> updateUsers();
  state -> updateUsers();

  // state -> startNewGame();
}
