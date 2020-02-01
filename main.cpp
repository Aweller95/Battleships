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
AI Design Document: https://docs.google.com/spreadsheets/d/1qIGcY03PQ1ut_RpV3RzkFJiKKVMFKJbYgjXU_Tgb5kQ/edit?usp=sharing
--> I have also thought about x, y z for AI targeting algorithm;

Welcome to Alex Wellers...
   ___         __   __   __           __    _          
  / _ ) ___ _ / /_ / /_ / /___  ___  / /   (_)___   ___
 / _  |/ _ `// __// __// // -_)(_-< / _ \ / // _ \ (_-<
/____/ \_,_/ \__/ \__//_/ \__//___//_//_//_// .__//___/
                                           /_/         

- Gamestate contains a vector of users; The state registers new users;
- User contains a vector of ships, a vector of occupied coordinates and a vector of coordinates that they have been attacked at;
*/

// I declare classes here initially to enable interdependence;
class clsShip;
class clsUser;
class clsGamestate;

const int NOVALUE = -1;

struct bulkhead{
  int x = NOVALUE;
  int y = NOVALUE;
  bool hit = false;
  bool sunk = false;
};

struct udtCoord{
  int x;
  int y;
};

vector <string> roundEvents; // stores strings that describe various events that occur during a round;

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
  // call the rand() function to get a random number
  // the parameter 'mod' is used to calculate the modulus of the result + 1; 
  // if mod = 10, the random number will be between 1 & 10;
  int random_number = (rand() % mod) + 1; 
  return random_number;
}

void printRoundEvents(){ // TODO: move to state
  Log("Round History");
  Log("________________________________");
  Log("════════════════════════════════");
  for(int i = 0; i < roundEvents.size(); i++){
    cout << to_string(i+1) + ". " << roundEvents[i] << "\n";
  }
  Log("________________________________");
  Log("════════════════════════════════");
}

void yToContinue(){ // TODO: Move to state
  char input;
  Log("Continue...? (y)");
  
  do {
    cin >> input;
  } while(input != 'y');
}

void enterToContinue(){ // TODO: move to state
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

void progressBar(int width){
  float progress = 0.0;

  cout << "\e[?25l"; //Hide the cursor;
  
  while (progress < 1.01) {
    int barWidth = width;

    cout << setBrightGreen("[");
    int pos = barWidth * progress;
    
    for (int i = 0; i < barWidth; ++i) {
      if (i < pos){
        cout << setGreen("=");
      } else if (i == pos) {
        cout << setGreen(">");
      } else { 
        cout << " ";
      }
    }
    cout << setBrightGreen("] ") << int(progress * 100.0) << " %\r";
    usleep(120000);//wait in microseconds
    cout.flush();

    progress += 0.09;
  }
  cout << endl;
  cout << "\e[?25h"; // show the cursor
}

string cleanString(string text){
  string _local = text;
  cout << endl << "BEFORE: [" <<_local << "]\n";//TODO: remove this DEBUG

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

  cout << "AFTER : [" <<_local << "]"; // TODO: Remove this DEBUG

  return _local;
};

void printBoardKey(){
  Log("Key:");
  Log(setGreen("■") + " = Friendly battleship");
  Log(setRed("■") + " = Sunk ship");
  Log(setYellow("☼") + " = Missed torpedo attack");
  Log(setRed("×") + " = Torpedo attack hit a battleship");
  Log();
}
 
udtCoord cpuGenerateRandCoords(int xSize, int ySize){
  udtCoord _tempCoord;

  _tempCoord.x = rollDice(xSize); //set x as a random number between 1 & xSize
  _tempCoord.y = rollDice(ySize); //set y as a random number between 1 & ySize

  return _tempCoord;
}

pair<char, bool> cpuSelectHeading(){ //TODO: Remove, Not needed;
  int selected = rollDice(4); //get a random number inclusively between 1 & 4
  pair<char, bool> result;

  Log();
  cout << selected << endl;

  result.first = 'x';
  result.second = false;

  switch(selected){
    case 1 : {
      result.first = 'u';
      result.second = true;
    }
    case 2 : {
      result.first = 'd';
      result.second = true;
    } 
    case 3 : {
      result.first = 'l';
      result.second = true;
    }
    case 4 : {
      result.first = 'r';
      result.second = true;
    }
  }
  cout << result.first << endl;
  return result;
}

int cpuSelectRandomTarget(int activePlayerCount, int userIdIgnore){
  int randId;

  do{
    randId = rollDice(activePlayerCount); // get a random number between 1 and the amount of active players;
  } while(randId == userIdIgnore);

  return randId;
}


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

    void setSunk(){
      for(int i = 0; i < _bulkheads.size(); i++){
        _bulkheads[i].sunk = true;
      }
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

    bool isCPU(){
      return _cpu;
    }

    void setInactive(){
      _active = false;
    }

    bool isActive(){
      return _active;
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
          return true;
        }
      }
      return false;
    }

    bool validateOriginCoord(int xSize, int ySize, int point, char axis){
      if(axis == 'x'){
        if(point > xSize || point < 1){
          return false;
        }
      }

      if(axis == 'y'){
        if(point > ySize || point < 1){
          return false;
        }
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
                if(isSunk(x, y)){ // if the coord contains a ship that has been sunk...
                cout << setRed("■ "); // print an S if this coord is occupied by a sunk ship;
                } else {
                cout << setRed("× "); // print an X if this coord is occupied by a ship and has been hit but isnt sunk;
                }
              } else {
                cout << setYellow("☼ "); // print a yellow x if this coord has been attacked but is a miss;
              }
            } else if (checkCollision(x, y, true)){ 
              if(isSunk(x, y)){
                cout << setRed("■ "); // print an S if this coord is occupied by a sunk ship;
              } else {
                cout << setRed("× "); // print an X if this coord is occupied by a ship and has been hit but isnt sunk;
              }
            } else if (checkCollision(x, y)){ 
              cout << setGreen("■ "); // print an S if this coord is occupied by a ship;
            } 
          }
          else if(hasBeenAttacked(x, y)){
              cout << setYellow("☼ "); // print a yellow ☼ if this coord has been hit but is not occupied by a ship;
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
            printGreen("■ ");
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

        if(!isCPU()){ // if player is not CPU controlled
          ClearConsole();
          printPlacementTitle();

          viewBoard(xSize, ySize);
          Log(getName(), ", place your ", _ships[i].getName() + " (length: " + to_string(_ships[i].getLength()) + ")");
          Log();

          Log("Enter " + setBrightGreen("X") + " coord");
          cin >> x;

          while(!validateOriginCoord(xSize, ySize, x, 'x') || !cin){ //validate x coord
            Log("Invalid " + setBrightGreen("X") + " coordinate entered, please enter a coordinate between 1, " ,to_string(xSize));
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> x;
          }
          
          Log("Enter " + setCyan("Y") + " coord");
          cin >> y;

          while(!validateOriginCoord(xSize, ySize, y, 'y') || !cin){ //validate y coord
            Log("Invalid " + setCyan("Y") + " coordinate entered, please enter a coordinate between 1, " ,to_string(ySize));
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> y;
          }

          while(checkCollision(x, y)){ // checks if origin point collides with another ship
            Log("Placing your " + _ships[i].getName() + " here, will cause it to collide with another ship\n");

            Log("Please enter a new " + setBrightGreen("X") + " coordinate");
            cin >> x;

            while(!validateOriginCoord(xSize, ySize, x, 'x') || !cin){ //validate x coord
              Log("Invalid X coordinate entered, please enter a coordinate between 1, " ,to_string(xSize));
              cin.clear();
              cin.ignore(numeric_limits<streamsize>::max(), '\n');
              cin >> x;
            }
            
            Log("Please enter a new " + setCyan("Y") + " coordinate");
            cin >> y;

            while(!validateOriginCoord(xSize, ySize, y, 'y') || !cin){ //validate y coord
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
              if(!(x - _ships[i].getLength() < 0) && !checkCollision(x, y, heading, _ships[i].getLength())){
                canPlace = true;
                break;
              }
              selected = "left";
            } 

            if(heading == 'u'){ //VERTICAL - HEADING UP
              if(!(y + _ships[i].getLength() - 1 > ySize) && !checkCollision(x, y, heading, _ships[i].getLength())){
                canPlace = true;
                break;
              }
              selected = "up";
            }

            if(heading == 'd'){ //VERTICAL - HEADING DOWN
              if(!(y - _ships[i].getLength() < 0) && !checkCollision(x, y, heading, _ships[i].getLength())){
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

        } else { // if the player is CPU controlled
          udtCoord _cpuCoords;

          do{
            int selected = rollDice(4);

            //select a random heading
            do{
              _cpuCoords = cpuGenerateRandCoords(xSize, ySize); // select random x & y coords -> validate coords 
            } while(
              checkCollision(_cpuCoords.x, _cpuCoords.y) || 
              !validateOriginCoord(xSize, ySize, _cpuCoords.x, 'x') || 
              !validateOriginCoord(xSize, ySize, _cpuCoords.y, 'y')
              );

            
            if(selected == 1){ // HORIZONTAL - HEADING RIGHT
              heading = 'r';
              // check if the ship will go off of the map OR if it will intersect with another ship
              if(!(_cpuCoords.x + _ships[i].getLength() - 1 > xSize) && !checkCollision(_cpuCoords.x, _cpuCoords.y, heading, _ships[i].getLength())){
                canPlace = true;
                break;
              }
            }

            if(selected == 2){ // HORIZONTAL - HEADING LEFT
              heading = 'l';
              if(!(_cpuCoords.x - _ships[i].getLength() < 0) && !checkCollision(_cpuCoords.x, _cpuCoords.y, heading, _ships[i].getLength())){
                canPlace = true;
                break;
              }
            } 

            if(selected == 3){ //VERTICAL - HEADING UP
              heading = 'u';
              if(!(_cpuCoords.y + _ships[i].getLength() - 1 > ySize) && !checkCollision(_cpuCoords.x, _cpuCoords.y, heading, _ships[i].getLength())){
                canPlace = true;
                break;
              }
            }

            if(heading == 4){ //VERTICAL - HEADING DOWN
              heading = 'd';
              if(!(_cpuCoords.y - _ships[i].getLength() < 0) && !checkCollision(_cpuCoords.x, _cpuCoords.y, heading, _ships[i].getLength())){
                canPlace = true;
                break;
              }
            }

          } while(!canPlace);

          _ships[i].updateBulkheads(_cpuCoords.x--, _cpuCoords.y--, xSize, ySize, heading); //update the currently selected ships bulkheads;
          placeShip(_ships[i]); //Add the selected ships coords to the _occupied variable;
        }
      }
      if(!(isCPU())) yToContinue();
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
            _ships[i].decrementHealth(); // decrease the current ships health by 1;
            _ships[i].getBulkheads()[j].hit = true; // set the current bulkheads status hit = true;
          }
        }

        if(_ships[i].getHealth() == 0 && !_ships[i].getAnnounced()){ //if any of the updated ships have been destroyed & they have not been announced;
          _ships[i].setAnnounced();
          _ships[i].setSunk(); // set all of the ships bulheads to sunk status;
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

    int calculateHealth(){ // check each ship that a player owns & check if they have remaining bulkheads;
      int remainingShips = 0;

      for(int i = 0; i < _ships.size(); i++){
        if(_ships[i].getHealth() != 0){
          remainingShips++;
        }
      }

      return remainingShips;
    }

    // udtCoord getLastHitCoord(){
    //   return _lastHitCoord;
    // }

    // void setLastHitCoord(int x, int y){
    //   _lastHitCoord.x = x;
    //   _lastHitCoord.y = y;
    // }

    // void resetLastHitCoord(){
    //   _lastHitCoord = {-1, -1};
    // }

    int getLastTargetId(){
      return _lastTarget;
    }

    void setLastTargetId(int id){
      _lastTarget = id;
    }

    vector <udtCoord> getPotentialAttackCoords(){
      return _cpuPotentialAttack;
    }

    bool checkPotentialsIfExists(udtCoord coord){ // Checks if the passed in coord is already in the potentials vector;
      bool result = false;

      for(int i = 0; i < _cpuPotentialAttack.size(); i++){
        if(_cpuPotentialAttack[i].x == coord.x && _cpuPotentialAttack[i].y == coord.y){
          result = true;
        }
      }
      return result;
    }

    void cpuGeneratePotentialAttackCoords(int xSize, int ySize, int x, int y, clsUser target){
      udtCoord _temp = { x, y };

      if((_temp.x + 1) <= xSize && !target.hasBeenAttacked(_temp.x+1, _temp.y)){ //if adding +1 to Xcoord of last hit fits on board;
        udtCoord _newCoord = { _temp.x + 1, _temp.y }; // build new adjusted coord

        if(!checkPotentialsIfExists(_newCoord)){ //if the newly generated coord is not already a potential target, add it to potentials;
          _cpuPotentialAttack.push_back(_newCoord); // push the new coord to potentials;
        }
      }

      if((_temp.y + 1) <= ySize && !target.hasBeenAttacked(_temp.x, _temp.y + 1)){
        udtCoord _newCoord = { _temp.x, _temp.y + 1 };
        
        if(!checkPotentialsIfExists(_newCoord)){ //if the newly generated coord is not already a potential target, add it to potentials;
          _cpuPotentialAttack.push_back(_newCoord); // push the new coord to potentials;
        }
      }

      if((_temp.x - 1) > 0 && !target.hasBeenAttacked(_temp.x - 1, _temp.y)){
        udtCoord _newCoord = { _temp.x - 1, _temp.y };

        if(!checkPotentialsIfExists(_newCoord)){ //if the newly generated coord is not already a potential target, add it to potentials;
          _cpuPotentialAttack.push_back(_newCoord); // push the new coord to potentials;
        }
      }

      if((_temp.y - 1) > 0 && !target.hasBeenAttacked(_temp.x, _temp.y - 1)){
        udtCoord _newCoord = { _temp.x, _temp.y - 1 };

        if(!checkPotentialsIfExists(_newCoord)){ //if the newly generated coord is not already a potential target, add it to potentials;
          _cpuPotentialAttack.push_back(_newCoord); // push the new coord to potentials;
        }
      }
    }

    udtCoord cpuGenerateSmartCoords(){
      udtCoord _tempCoord;
      int randItem = rollDice(_cpuPotentialAttack.size());
      int index = randItem - 1; //get random number based on amount of potentials;

      _tempCoord.x = _cpuPotentialAttack[index].x;
      _tempCoord.y = _cpuPotentialAttack[index].y;

      _cpuPotentialAttack.erase(_cpuPotentialAttack.begin() + index); // delete the selected coordinate from potentials;
      
      return _tempCoord; //return the randomly selected coord;
    }

    void printPotentials(){ //DEBUG function
    Log("Printing potential next shots for ", getName());//DEBUG
      if(_cpuPotentialAttack.size()){
        for(int i = 0; i < _cpuPotentialAttack.size(); i++){
          Log("x:" + to_string(_cpuPotentialAttack[i].x), " y:" + to_string(_cpuPotentialAttack[i].y));
          Log();
        }
      } else {
        Log("No potentials");//DEBUG
      }
    }

    bool isSunk(int x, int y){
      for(int s = 0; s < _ships.size(); s++){ // for each ship...
        for(int b = 0; b < _ships[s].getBulkheads().size(); b++){ //for each bulkhead...
          if(_ships[s].getBulkheads()[b].x == x && _ships[s].getBulkheads()[b].y) { //if the params match the coords of a bulkhead 
            
            if(_ships[s].getBulkheads()[b].sunk) return true;
          }
        }
      }
      return false;
    }

  private:
    int _id;
    string _name;
    vector < udtCoord > _occupied; // represents a players personal board;
    vector < udtCoord > _attacked; // represents where users have fired at this users board;
    vector < udtCoord > _cpuPotentialAttack;
    vector < clsShip > _ships;
    // udtCoord _lastHitCoord = { -1, -1 }; TODO: remove
    int _lastTarget = -1;
    bool _active = true;
    bool _cpu = false;
};

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
      ClearConsole();
      printPlayersTitle();

      for(int i = 0; i < _playerCount; i++){
        char userChoice;
        bool isCPU = false;
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
        userChoice = ' ';
        isCPU = false;
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
        updateUsers(); // calling when stage == 1; //TODO: change to while loop
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
          Log("Asking user " + _users[i].getName() + " to place their ships");
          _users[i].placeFleet(getBoardSize().x, getBoardSize().y); // get user to place their boats;
        }
        setState(2);

      } else if(_state == 2){ // if stage is 'play' - cycle through users to choose target & attack;
        while(getActivePlayers() > 1){ // while there is more than 1 active player
          for(int i = 0; i < _users.size(); i++){ // for each user
            if(_users[i].calculateHealth() == 0){ // if the user is dead;
                _users[i].setInactive(); //set them as inactive
                roundEvents.push_back(setRed("!!! All of " + _users[i].getName() + "'s ships have been destroyed !!!"));//push the event to the event queue;

              } else if(_users[i].isActive() && !_users[i].isCPU()){ // if the player is active & not a CPU player;
              pair <bool, int> foundUser;
              int targetId = -1;

              ClearConsole();
              printBattleTitle();
              printAllUsers(_users[i].getId());

              // VALIDATE PLAYER ID INPUT
              Log();
              Log(setGreen(_users[i].getName()), ", enter the " + setYellow("ID") + " of the player you want to attack: ");
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

              getUserByIndex(foundUser.second).viewBoard(getBoardSize().x, getBoardSize().y, true); // view targets board;
              _users[i].viewBoard(getBoardSize().x, getBoardSize().y); // View current player board;
              printBoardKey();

              udtCoord validatedAttackCoord = selectTargetCoords(getUserByIndex(foundUser.second), getBoardSize().x, getBoardSize().y); // get user to select attack coords;
              getUserByIndex(foundUser.second).addAttacked(validatedAttackCoord.x, validatedAttackCoord.y); // add the validated attack coordinate to the targets board;
              
              ClearConsole();
              printBattleTitle();

              getUserByIndex(foundUser.second).viewBoard(getBoardSize().x, getBoardSize().y, true); //view the targets board again with hit/miss feedback;
              _users[i].viewBoard(getBoardSize().x, getBoardSize().y); // View current player board;
              printBoardKey();

              yToContinue();

            } else if(_users[i].isActive() && _users[i].isCPU()){ // is active & isCPU
              int targetIndex;
              int lastTargetIndex = checkUserExistsById(_users[i].getLastTargetId()).second;
              bool lastTargetExists = checkUserExistsById(_users[i].getLastTargetId()).first;
              bool hit = false;
              udtCoord attackCoord = { -1, -1 };
              
              //ADAPTIVE CPU - SELECT TARGET: 
              if(!lastTargetExists || !getUserByIndex(lastTargetIndex).isActive()){// If last Target isnt active or doesnt exist -> select new random target;
                int _targetId;

                _targetId = cpuSelectRandomTarget(getActivePlayers(), _users[i].getId()); // generate a random targetId
                _users[i].setLastTargetId(_targetId); // store lastTarget
                targetIndex = checkUserExistsById(_targetId).second; //find the user in _users - return the index of the found user in _users;
              } else { //if last target is active -> set as current target for this round;
                targetIndex = checkUserExistsById(_users[i].getLastTargetId()).second;
              }
              ///////////////////////////

              //PRINT USER INTERFACE
              ClearConsole();
              printBattleTitle();
              getUserByIndex(targetIndex).viewBoard(getBoardSize().x, getBoardSize().y, true); //view the targets board before attacking;
              printBoardKey(); // print board key
              Log(setGreen(_users[i].getName()) + " is targeting " + setRed(getUserByIndex(targetIndex).getName()));
              Log();
              Log("Thinking. . .");
              progressBar((getBoardSize().x) * 2); // show CPU is thinking
              ////////////////

              // ADAPTIVE CPU - SELECT ATTACK COORDS;
              //check if there is at least 1 item in potentials vector for user;
              if(_users[i].getPotentialAttackCoords().size() == 0){
                Log("NO POTENTIAL TARGETS... Selecting Random Target");//DEBUG
                do {
                  attackCoord = cpuGenerateRandCoords(getBoardSize().x, getBoardSize().y); // generate random attack coords;
                } while(
                  !getUserByIndex(targetIndex).validateOriginCoord(getBoardSize().x, getBoardSize().y, attackCoord.x, 'x') || //if x coord..
                  !getUserByIndex(targetIndex).validateOriginCoord(getBoardSize().x, getBoardSize().y, attackCoord.y, 'y') || //or y coord is invalid
                  getUserByIndex(targetIndex).checkCollision(attackCoord.x, attackCoord.y, true)   //or the attack coord has already been attacked 
                  );

                } else { //if there are remaining potential targets -> select one from the vector
                Log("Selecting from potential target list");
                attackCoord = _users[i].cpuGenerateSmartCoords(); // return potential attack location;
              }

              getUserByIndex(targetIndex).addAttacked(attackCoord.x, attackCoord.y); // add the validated attack coordinate to the targets board;

              if(getUserByIndex(targetIndex).getAttackedOrOccupied(attackCoord.x, attackCoord.y)){//if the attacked coord has hit...
                hit = true;
                _users[i].cpuGeneratePotentialAttackCoords(getBoardSize().x, getBoardSize().y, attackCoord.x, attackCoord.y, getUserByIndex(targetIndex)); // build new potential hits based off of last hit
              }
              
              //PRINT USER INTERFACE
              ClearConsole();
              printBattleTitle();
              getUserByIndex(targetIndex).viewBoard(getBoardSize().x, getBoardSize().y, true); //view the targets board again with hit/miss feedback;
              printBoardKey();
              Log(setGreen(_users[i].getName()) + " attacked " + setRed(getUserByIndex(targetIndex).getName()) + " at: " + to_string(attackCoord.x) + ", " + to_string(attackCoord.y) + "\n");
              _users[i].viewBoard(getBoardSize().x, getBoardSize().y); //DEBUG SHOWING CPU BOARD
              ////////////////

              if(hit){ // if the shot hit - print confirmation message;
                Log("The shot " +  setRed("hit!"));
              } else { //if shot missed - reset last hit & potential attack coords;
                Log("The shot " + setYellow("missed!"));
                // _users[i].clearPotentials();//reset potentials //DEBUG MIGHT NOT BE NEEDED
                // _users[i].resetLastHitCoord();//reset last hit //DEBUG MIGHT NOT BE NEEDED
              }
              Log();

              yToContinue();
            }
          }
          //End of round
          ClearConsole();
          printRoundOverTitle();
          printRoundEvents(); // print all events that occured in current round;
          roundEvents.clear(); // reset events for next round;
          yToContinue();
        }
        setState(3); // once there is only 1 active player remaining - change state to 3 (game over)

      } else if(_state == 3){ // if stage is 'winner' - display winner screen to remaining player(s);
        char input;
        
        ClearConsole();
        printGameOverTitle();
        Log(setGreen(_users[0].getName() + " has WON!"));
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
        if(!targetUser.validateOriginCoord(xSize, ySize, attackCoord.x, 'x') || !targetUser.validateOriginCoord(xSize, ySize, attackCoord.y, 'y') || !cin){ // only print on 2rd execution of this loop
          cout << "\e[4F"; //move cursor up 4 lines;
          cout << "\e[0J"; // clear screen from cursor down;

          Log(setRed("!Please enter valid coordinates!"));
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
          
          Log();
          Log("Enter the " + setBrightGreen("X") + " coordinate that you want to attack");
          cin >> attackCoord.x;

          Log("Enter the " + setCyan("Y") + " coordinate that you want to attack");
          cin >> attackCoord.y;
        } else { // run this block first
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n'); //ignore all newlines

          Log("Enter the " + setBrightGreen("X") + " coordinate that you want to attack");
          cin >> attackCoord.x;

          Log("Enter the " + setCyan("Y") + " coordinate that you want to attack");
          cin >> attackCoord.y;
        }
      } while(!targetUser.validateOriginCoord(xSize, ySize, attackCoord.x, 'x') || !targetUser.validateOriginCoord(xSize, ySize, attackCoord.y, 'y') || !cin); // check entered coords exist within the map;

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

    pair<bool, int> checkUserExistsById(int id){
      pair <bool, int> result; // a pair is used here for easier interpretation of result & to avoid reaching end of control in a non-void function;
      result.first = false; //Default to no user found
      result.second = -1; //Default to no user found

      for(int i = 0; i < _users.size(); i++){
        if(_users[i].getId() == id){
          result.first = true; // if user is found
          result.second = i; // record the index the user was found at;
        }
      }
      return result;
    }

    clsUser& getUserByIndex(int index){
      return _users[index];
    }

    void deleteAllUsers(){
      if(_users.size()){
        _users.clear(); // delete all registered users;
      }
    }

    void registerShip(clsShip ship){ // registers a ship class to the gamestate to configure new user fleets;
      _fleetConfig.push_back(ship);
    }

    void setState(int i){
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

clsGamestate* clsGamestate::_inst = NULL; // indicate that no instance of clsGamestate has been instantiated above this line;

int main(){
  srand(time(NULL)); // Seed random values generated with rand() with the current time;
  clsGamestate* game; // set variable 'Gamestate' as a pointer;
  game = clsGamestate::getInstance(); // assign the instance of clsGamestate;

  clsShip carrier("Aircraft Carrier", 5);
  clsShip battleship("Battleship", 4);
  clsShip submarine("Submarine", 3);
  clsShip cruiser("Cruiser", 3);
  clsShip patrolBoat("Patrol Boat", 2);

  vector < clsShip > shipConfig;

  shipConfig.push_back(carrier);
  shipConfig.push_back(battleship);
  shipConfig.push_back(submarine);
  shipConfig.push_back(cruiser);
  shipConfig.push_back(patrolBoat);

  clsUser user1("Alex", 1, false, shipConfig);
  clsUser user2("Sofia", 2, false, shipConfig);
  clsUser user3("Bill", 3, true, shipConfig);
  clsUser user4("Ted", 4, true, shipConfig);

  game -> registerUser(user1);
  game -> registerUser(user2);
  // game -> registerUser(user3);
  // game -> registerUser(user4);

  game -> setPlayerCount(4);
  game -> setBoardSize(7, 7);
  game -> setState(1);

  game -> updateUsers();
  game -> updateUsers();
  game -> updateUsers();

  // game -> registerShip(carrier); 
  // game -> registerShip(battleship); 
  // game -> registerShip(submarine); 
  // game -> registerShip(cruiser); 
  // game -> registerShip(patrolBoat); 

  // game -> startNewGame();
}
