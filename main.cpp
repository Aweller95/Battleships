/* 
Welcome to Alex Wellers...
   ___         __   __   __           __    _          
  / _ ) ___ _ / /_ / /_ / /___  ___  / /   (_)___   ___
 / _  |/ _ `// __// __// // -_)(_-< / _ \ / // _ \ (_-<
/____/ \_,_/ \__/ \__//_/ \__//___//_//_//_// .__//___/
                                           /_/         

Name: Alexander Weller
email: alexander.weller@ada.ac.uk
AI Design Document: https://docs.google.com/spreadsheets/d/1qIGcY03PQ1ut_RpV3RzkFJiKKVMFKJbYgjXU_Tgb5kQ/edit?usp=sharing 
                    (Requires an Ada gmail account to access)


Overall Approach
----------------
I approached the brief for this project by first thinking about how to architect the implementation. I initially thought about how I could merge multiple design patterns into one (Singleton, Observer Patter & Finite State Machine). My first thought was to have 3 main classes; User class, State class & Ship class, with a dependency on each other. This first design fell short due to the fact that it introduced cyclic dependency which is a paradigm broadly considered to be bad practice in program design. 

Moving away from cyclic dependency, I decided to have my State class deal with all aspects related to the state of the game, as well as control the flow of the gameplay. I chose to also set up the state class as a subject to which Users/players would subscribe to and be updated based on the state. The ship class would also stored within the state and handed to new users in order to configure fleet composition.


Design/Development Iterations
-----------------------------
  --> Class design
  The initial class design mainly remained unchanged throughout the development of Battleships. I found that sticking to 3 main classes eased complexity nearer the end of the project.

  --> state
  I wanted the state to be used to update all registered users depending on 3 main stages of the game which I identified by decomposing the game on paper. The 3 main states were ‘Placement’ where each user would need to place their fleet on the board, ‘Play’ where each user would select a target and where to attack the target by entering coordinates and finally ‘End / win’, where the last remaining play wins the game. These states remained unchanged throughout the development process.

  --> extensible from the beginning
  I went into the development of Battleships with the intention of making the game fully customizable and extensible. From the beginning I set no hard limits to board size, ship sizes, ship amounts or player count. Due to allowing for the flexibility in these areas, some parts of the gameplay loop especially were complex as there were many permutations to consider.

  --> gameplay loop
  The gameplay loop was one of the most complex areas to work on nearer the end of the project. As mentioned above, the sheer mount of permutations to consider for both human player and CPU players caused the code to become very long.

  --> Player vs. player
  I first attempted to tackle player vs. player as I could test this fairly quickly and easily, as well as test each stage of the game thouroughly, such as placement, ensuring no boats were colliding, ships could not be placed outside of the map etc. Once a working journey with 2 human players was implemented, I could then start thinking about the steps that a CPU player would need to take, which turned out to be extremely similar to that of a human player – making the required tasks for the CPU clear.

  --> AI Design (1st iteration (random))
  The first iteration for CPU players was to have their placement of ships and selection of attack coordinates to be completely random. Once a reliable randomization function was written (rollDice()), I could then start using this to generate random selections by the CPU. Having a working CPU player, I was also able to put two CPU players against each other, which increased the speed of my testing considerably.

  --> smarter AI Design (AI design Document)
  Once I had played a full game against a working ‘easy’ CPU player, I decided to make the AI smarter and more adaptive to situations. The smarter AI design still randomly placed their own ships, however during the ‘play’ phase, the CPU initially selects completely random coordinates, which I call “Seek Mode”. Once the CPU hits a ship, the CPU then changes to “Hunt Mode”. 

  In Hunt Mode, the CPU generates all of the potential next hits based on the initial hit and stores them under the user class. Each subsequent turn, the CPU will randomly choose one of the potential target locations, deleting the selected one until it has no more viable potential targets. Once there are no more potential target locations left, the CPU switches back to Seek Mode.

  --> Future considerations for AI design
  Due to time limitations, I could not implement further improvements to the AI. If I had more time, I would think about allowing individual CPU opponents interpret hits by other users. I would also have the CPU determine vertical or horizontal direction of the target & eliminate potential targets from the stack. I was also considering assigning probability values to each potential target within the stack to influence the choice of next attack, by selecting the highest probability coordinate first.


Main Challenges
---------------
The biggest challenges I found was writing the AI for CPU opponents. I have never attempted to write an AI or CPU before so I was approaching this with no prior knowledge and came across many problems while implementing my solution. I found that the most difficult part was working out what needed to happen at which point in time during gameplay rounds.


Most interesting / enjoyable/ rewarding successes
-------------------------------------------------
Although I struggled with it, I found writing the CPU opponents logic to be the most interesting, enjoyable AND rewarding. When I finally was happy with the logic, I gave me great delight to watch two CPU opponents fight each other in a ‘smart’ fashion!


Reflection on continued development
-----------------------------------
I enjoyed this project immensely, I think that I have written a solid base on which to continue my exploration into game design, program design and AI design. I plan on continuing to play with what I have below to further improve my development knowledge. 

I also found that, while very challenging at first, learning a new language forced me to really think about certain problems that I had while developing my solution, as opposed to writing my solution in JavaScript, where I wouldn’t have really explored HOW I could have implemented my solution in different ways.

I intend to continue learning more about C++ and its application in the game development industry, I am also looking into game development using C# within the Unity engine. I also intend to discuss my experiences using C++ with my team at work, I feel that now having used two languages, I appreciate what some provide you and what other do not. I also feel that I have added additional context to my engineering decisions that I will make in the future.
*/

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <regex>
#include <typeinfo>
#include <limits>
#include <unistd.h>
#include <fstream>

#define configFile "config.csv"

using namespace std;

// I declare classes here initially to enable interdependence;
class clsShip;
class clsUser;
class clsGamestate;

vector <string> roundEvents; // stores strings that describe various events that occur during a round;

struct bulkhead{
  int x = -1;
  int y = -1;
  bool hit = false;
  bool sunk = false;
};

struct udtCoord{
  int x;
  int y;
};

//UTILITIES CLASS
/* 
- Log() takes 3 optional params that will print to the console. Calling with no params prints a new line - I found that this was a cleaner way instead of using `cout << "\n"`.
- ClearConsole() prints an escape sequence using `cout` which will clear all data currently displayed on the console.
*/

class clsUtilities{
  protected:

  void Log(string message1 = "", string message2 = "", string message3 = ""){
  cout << message1 << message2 << message3 << endl;
  }

  int getIntLength(int i){
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

  void yToContinue(){
    char input;
    
    do {
      Log("Continue...? (y)");
      cin >> input;
    } while(input != 'y');
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

  void printReadyTitle(){
    string line1 = "   ___                 __      ___ ";
    string line2 = "  / _ \\ ___  ___ _ ___/ /__ __/__ \\";
    string line3 = " / , _// -_)/ _ `// _  // // / /__/";
    string line4 = "/_/|_| \\__/ \\_,_/ \\_,_/ \\_, / (_)  ";
    string line5 = "                       /___/       ";
    
    Log(setGreen(line1));
    Log(setGreen(line2));
    Log(setGreen(line3));
    Log(setGreen(line4));
    Log(setGreen(line5));
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

  int cpuSelectRandomTarget(int activePlayerCount, int userIdIgnore){
    int randId;

    do{
      randId = rollDice(activePlayerCount); // get a random number between 1 and the amount of active players;
    } while(randId == userIdIgnore);

    return randId;
  }  
};

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

    // int calculateHealth(){
    //   int _hit;

    //   for(int i = 0; i < _bulkheads.size(); i++){
    //     if(_bulkheads[i].hit) _hit++;
    //   }

    //   return _length - _hit;
    // }

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

    // void setSunk(){
    //   for(int i = 0; i < _bulkheads.size(); i++){
    //     _bulkheads[i].sunk = true;
    //   }
    // }

  private:
    int _length;
    int _health;
    bool _announced = false;
    string _name;
    vector < bulkhead > _bulkheads;
};

//USER CLASS
class clsUser : clsUtilities{ //Observer - inheirits utilities class
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

    // bool isActive(){
    //   return _active;
    // }

    bool getAnnounced(){
      return _announced;
    }

    void setAnnounced(){
      _announced = true;
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
        Log("Targeting ", setRed(getName()), "'s board");
      } else {
        Log("Your (", setGreen(getName()), "'s) board");
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
          Log();
          Log(setGreen(getName()), ", place your ", setYellow(_ships[i].getName()) + " (length: " + to_string(_ships[i].getLength()) + ")");
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
          }
        }

        if(_ships[i].getHealth() == 0 && !_ships[i].getAnnounced()){ //if any of the updated ships have been destroyed & they have not been announced;
          registerSunk(_ships[i]); // set all of the ships bulheads to sunk status if ship has been destroyed;
          _ships[i].setAnnounced();
          roundEvents.push_back(getName() + "'s " + _ships[i].getName() + " has been " + setRed("destroyed!")); // Add this to the message queue
        }
      } 
      
      if(missed){
        roundEvents.push_back(getName() + " was attacked at: " + to_string(x) + ", " + to_string(y) + " but the shot " + setYellow("missed")); // add the attacked coord to the message queue;
      }
    }

    void registerSunk(clsShip &ship){ // push all coordinate for bulheads of a ship once sunk;
      for(int i = 0; i < ship.getBulkheads().size(); i++){
        udtCoord _sunkCoord;

        _sunkCoord.x = ship.getBulkheads()[i].x;
        _sunkCoord.y = ship.getBulkheads()[i].y;

        _sunk.push_back(_sunkCoord);
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

    int getLastTargetId(){
      return _lastTarget;
    }

    void setLastTargetId(int id){
      _lastTarget = id;
    }

    vector <udtCoord> getPotentialAttackCoords(){
      return _cpuPotentialAttack;
    }

    void printPotentialAttackCoords(){ // DEBUG FUNCTION
      Log("Printing potential attack coordinates");
      for(int i = 0; i < _cpuPotentialAttack.size(); i++){
        cout << _cpuPotentialAttack[i].x << ", " << _cpuPotentialAttack[i].y << endl;
      }
    }

    void resetPotentialTargets(){
      _cpuPotentialAttack.clear();
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

    void cpuGeneratePotentialAttackCoords(int xSize, int ySize, int x, int y, clsUser &target){
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

    udtCoord cpuGenerateSmartCoords(clsUser &target, int xSize, int ySize){
      udtCoord _tempCoord;
      int randItem;
      int index; 
      
      do{ 
        if(_cpuPotentialAttack.size() == 0){ // if no more potential targets - generate random coord;
          Log("No more potentials - generating random target");//DEBUg
          yToContinue();//DEBUG
          do{
            _tempCoord = cpuGenerateRandCoords(xSize, ySize);
          } while (
                    !target.validateOriginCoord(xSize, ySize, _tempCoord.x, 'x') || //if x coord..
                    !target.validateOriginCoord(xSize, ySize, _tempCoord.y, 'y') || //or y coord is invalid
                    target.checkCollision(_tempCoord.x, _tempCoord.y, true));   //or the attack coord has already been attacked

          return _tempCoord;
        }

        randItem = rollDice(_cpuPotentialAttack.size()); //get random number based on amount of potentials;
        index = randItem - 1;

        _tempCoord.x = _cpuPotentialAttack[index].x;
        _tempCoord.y = _cpuPotentialAttack[index].y;

        _cpuPotentialAttack.erase(_cpuPotentialAttack.begin() + index); // delete the selected coordinate from potentials;
      } while(target.hasBeenAttacked(_tempCoord.x, _tempCoord.y)); // If the coord has been attacked already;

      return _tempCoord; //return the randomly selected coord;
    }

    void printSunk(int xSize, int ySize){//DEBUG FUNCTION
      Log("Printing sunk coords for " + getName());
      for(int y = ySize; y >= 1; y--){
        for(int x = 1; x <= xSize; x++){
          if(isSunk(x, y)) cout << "sunk: " << x << ", " << y << endl;
        }
      }
    }

    bool isSunk(int x, int y){
     for(int i = 0; i < _sunk.size(); i++){
       if(_sunk[i].x == x && _sunk[i].y == y){
         return true;
       }
     }
     return false;
    }

  private:
    int _id;
    string _name;
    vector < udtCoord > _occupied; // represents a players personal board;
    vector < udtCoord > _attacked; // represents where users have fired at this users board;
    vector < udtCoord > _sunk;
    vector < udtCoord > _cpuPotentialAttack;
    vector < clsShip > _ships;
    int _lastTarget = -1;
    bool _active = true;
    bool _announced = false;
    bool _cpu = false;
};

//GAMESTATE CLASS - Combined Singleton, State Machine & observer Design Patterns
class clsGamestate : clsUtilities{
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

      do{
        ClearConsole();
        printConfigTitle();

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        Log("Enter the number of players: ");
        cin >> playerCount;

      } while (!playerCount || playerCount < 2);
      
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
      for(int i = 0; i < _playerCount; i++){
        char userChoice;
        bool isCPU = false;
        string name;
        
        ClearConsole();
        printPlayersTitle();

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
      bool firstRun = true;

      do {
        ClearConsole();
        printConfigTitle();

        Log("(Recommended board size: " + setBrightGreen(to_string(_maxShipLength * 2)) + " x " + setCyan(to_string(_maxShipLength * 2)) + ")");
        Log();

        if(firstRun){
          Log("Please enter the size of the " + setBrightGreen("X") + " axis for player boards");
          cin >> x;

          Log("Please enter the size of the " + setCyan("Y") + " axis for player boards");
          cin >> y;
        } else {
          Log(setRed("Board cannot be smaller than the number of ships it must contain"));
          Log();

          Log("Please enter a new size of the " + setBrightGreen("X") + " axis for player boards");
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
          cin >> x;

          Log("Please enter a new size of the " + setCyan("Y") + " axis for player boards");
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
          cin >> y;
        }
        firstRun = false;
      } while((x < _maxShipLength && y < _maxShipLength) || ((x * y) < _maxOccupied));

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
        
        setState(2);
        updateUsers(); // calling when stage == 2;

        setState(3);
        updateUsers(); // calling when stage == 3;
      }
    }

    int getActivePlayers(){
      int count = 0;

      for(int i = 0; i < _users.size(); i++){
        if(_users[i].calculateHealth() > 0){
          count++;
        }
      }
      return count;
    }

    void updateUsers(){
      if(_state == 1){ // if stage is 'placement' - get players to place ships;
        for(int i = 0; i < _users.size(); i++){
          _users[i].placeFleet(getBoardSize().x, getBoardSize().y); // get user to place their boats;
        }

      } else if(_state == 2){ // if stage is 'play' - cycle through users to choose target & attack;
        while(getActivePlayers() > 1){ // while there is more than 1 active player
          for(int i = 0; i < _users.size(); i++){ // for each user

            cout << _users[i].getName() << "'s health = " << _users[i].calculateHealth() << endl;
            yToContinue();

            if(_users[i].calculateHealth() > 0) {// if the current user is not dead;

              if(!_users[i].isCPU()){ // if the player is active & not a CPU player;
                pair <bool, int> foundUser;
                int targetId = -1;

                if(getNumOfHumanOpponents(_users[i].getId()) > 0){ // if there is at least 1 human opponent - show this warning;
                  ClearConsole();
                  printReadyTitle();
                  Log(setGreen(_users[i].getName()), " are you ready? (make sure your " + setRed("opponents") + " cannot see the screen!)");
                  Log();
                  yToContinue();
                }

                // PLAYER SELECT TARGET
                if(getActivePlayers() > 2){ //if there are more than 2 players, get user to enter a target;
                  printAllUsers(_users[i].getId());
                  Log();
                  Log(setGreen(_users[i].getName()), ", enter the " + setYellow("ID") + " of the player you want to attack: ");
                  cin >> targetId;
                  foundUser = checkUserExistsById(targetId);

                  while(!cin || _users[i].getId() == targetId || !foundUser.first){ // ask for a target id while the id is invalid / the id is of the current player / the type of input is invalid;
                    cout << "\e[2F"; //move cursor up 4 lines;
                    cout << "\e[0J"; // clear screen from cursor down;
                    Log(_users[i].getName() + ", enter a " + setRed("valid") + " user " + setYellow("ID"));

                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin >> targetId;
                    foundUser = checkUserExistsById(targetId);
                  }
                } else { // if there is only one potential target, select automatically;
                  targetId = cpuSelectRandomTarget(getActivePlayers(), _users[i].getId()); // get the id for the other player automatically;
                  foundUser = checkUserExistsById(targetId);
                }
                ///////////////////////////

                ClearConsole();
                printBattleTitle();

                getUserByIndex(foundUser.second).viewBoard(getBoardSize().x, getBoardSize().y, true); // view targets board;
                _users[i].viewBoard(getBoardSize().x, getBoardSize().y); // View current player board;
                printBoardKey();

                udtCoord validatedAttackCoord = selectTargetCoords(getUserByIndex(foundUser.second), getBoardSize().x, getBoardSize().y); // get user to select attack coords;
                getUserByIndex(foundUser.second).addAttacked(validatedAttackCoord.x, validatedAttackCoord.y); // add the validated attack coordinate to the targets board;

                if(getUserByIndex(foundUser.second).calculateHealth() == 0 && !getUserByIndex(foundUser.second).getAnnounced()) { //if user has been killed and hasnt been announced yet...
                  getUserByIndex(foundUser.second).setAnnounced();
                  roundEvents.push_back(setRed("!!! All of " + getUserByIndex(foundUser.second).getName() + "'s ships have been destroyed !!!"));//push the event to the event queue;
                }

                ClearConsole();
                printBattleTitle();

                getUserByIndex(foundUser.second).viewBoard(getBoardSize().x, getBoardSize().y, true); //view the targets board again with hit/miss feedback;
                _users[i].viewBoard(getBoardSize().x, getBoardSize().y); // View current player board;
                printBoardKey();
              }

              if(_users[i].isCPU()){ // if current player is CPU controlled;
                int targetIndex;
                int lastTargetIndex = checkUserExistsById(_users[i].getLastTargetId()).second;
                bool lastTargetExists = checkUserExistsById(_users[i].getLastTargetId()).first;
                bool hit = false;
                udtCoord attackCoord = { -1, -1 };
                
                // ADAPTIVE CPU - SELECT TARGET: 
                if(!lastTargetExists || getUserByIndex(lastTargetIndex).calculateHealth() == 0){// If last Target has been killed -> select new random target;
                  int _targetId;
                  int _targetIndex;

                  do{
                    _targetId = cpuSelectRandomTarget(getActivePlayers(), _users[i].getId()); // generate a random targetId
                    _targetIndex = checkUserExistsById(_targetId).second; //find the user in _users - return the index of the found user in _users;
                  } while(getUserByIndex(_targetIndex).calculateHealth() == 0); // while the new user is dead -> select new target;

                  _users[i].setLastTargetId(_targetId); // store lastTarget
                  _users[i].resetPotentialTargets(); // reset old potential targets;
                  targetIndex = _targetIndex;

                } else if(getUserByIndex(lastTargetIndex).calculateHealth() > 0){ //if last target is alive -> set as current target for this round;
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
                ///////////////////////

                // ADAPTIVE CPU - SELECT ATTACK COORDS;
                if(_users[i].getPotentialAttackCoords().size() == 0){ //check if there are no more coords in potentials vector for user;
                  do {
                    attackCoord = cpuGenerateRandCoords(getBoardSize().x, getBoardSize().y); // generate new random attack coords;
                  } while(
                    !getUserByIndex(targetIndex).validateOriginCoord(getBoardSize().x, getBoardSize().y, attackCoord.x, 'x') || //if x coord..
                    !getUserByIndex(targetIndex).validateOriginCoord(getBoardSize().x, getBoardSize().y, attackCoord.y, 'y') || //or y coord is invalid
                    getUserByIndex(targetIndex).checkCollision(attackCoord.x, attackCoord.y, true)   //or the attack coord has already been attacked 
                    );
                  } else { //if there are remaining potential targets -> select one from the vector
                    attackCoord = _users[i].cpuGenerateSmartCoords(getUserByIndex(targetIndex), getBoardSize().x, getBoardSize().y); // return potential attack location;
                }
                getUserByIndex(targetIndex).addAttacked(attackCoord.x, attackCoord.y); // add the validated attack coordinate to the targets board;

                if(getUserByIndex(targetIndex).calculateHealth() == 0 && !getUserByIndex(targetIndex).getAnnounced()) {
                  getUserByIndex(targetIndex).setAnnounced();
                  roundEvents.push_back(setRed("!!! All of " + getUserByIndex(targetIndex).getName() + "'s ships have been destroyed !!!"));//push the event to the event queue;
                }
                if(getUserByIndex(targetIndex).getAttackedOrOccupied(attackCoord.x, attackCoord.y)){//if the attacked coord has hit...
                  hit = true;
                  _users[i].cpuGeneratePotentialAttackCoords(getBoardSize().x, getBoardSize().y, attackCoord.x, attackCoord.y, getUserByIndex(targetIndex)); // build new potential hits based off of last hit
                }
                ///////////////////////
                
                //PRINT USER INTERFACE
                ClearConsole();
                printBattleTitle();
                getUserByIndex(targetIndex).viewBoard(getBoardSize().x, getBoardSize().y, true); //view the targets board again with hit/miss feedback;
                printBoardKey();
                Log(setGreen(_users[i].getName()) + " attacked " + setRed(getUserByIndex(targetIndex).getName()) + " at: " + to_string(attackCoord.x) + ", " + to_string(attackCoord.y) + "\n");
                if(hit){ // if the shot hit - print confirmation message;
                  Log("The shot " +  setRed("hit!"));
                } else { //if shot missed - print confirmation message;
                  Log("The shot " + setYellow("missed!"));
                }
                Log();
                ///////////////////////
              }
            }
          }
          //End of round
          ClearConsole();
          printRoundOverTitle();
          printRoundEvents(); // print all events that occured in current round;
          roundEvents.clear(); // reset events for next round;
          // yToContinue(); //ENABLE THIS DEBUG TODO
        }

      } else if(_state == 3){ // if stage is 'winner' - display winner screen to remaining player(s);
        char input;
        
        ClearConsole();
        printGameOverTitle();

        for(int i = 0; i < _users.size(); i++){
          if(_users[i].calculateHealth() > 0) Log(setGreen(_users[i].getName() + " has WON!"));
        }
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

    int getNumOfHumanOpponents(int ignoreId){
      int count = 0;

      for(int i = 0; i < _users.size(); i++){
        if(_users[i].getId() != ignoreId && !_users[i].isCPU()){
          count++;
        }
      }
      return count;
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

    string cleanString(string text){ // clean all leading, double and trailing spaces from a string;
      string _local = text;

      _local = regex_replace(_local, regex("^ +| +$|( ) +"), "$1");
      // (^ +|) match the beginning of the string, if it has one or more whitespaces, OR;
      // ( +$|) match the end of the string, if it has one or more whitespaces, OR;
      // (( ) +) match any double spaces within the string

      return _local;
    };

    pair <string, int> parse(string _line){
      pair < string, int > _result;
      string values[2];
      int _currentVal = 0;

      for(int i = 0; i < _line.length(); i++){
        if(_currentVal > 2) break;

        if(_line[i] == ','){ // if the current char is a separator increase _currentVal + 1;
          _currentVal++;
        } else {
          values[_currentVal] += _line[i]; // add the current char to values;
        }
      }

      _result.first = cleanString(values[0]); // clean the string containing the ship name;
      _result.second = stoi(values[1]); // convert the ship length from string to int;

      return _result;
    }

    void readConfig(){
      ifstream cFile; //define a new filestream;
      string _currentLine;
      int _longestShip = 0;
      int _toBeOccupied = 0;

      cFile.open(configFile); //open the config file with filestream;

      while(!cFile.eof()){ //while there are remaining lines in the file...
        getline(cFile, _currentLine); //assign the current line to '_currentLine'
        pair < string, int > _parsedLine = parse(_currentLine); // parse the current line and store the result as a pair in _parsedLine;

        if(_parsedLine.second > _longestShip) _longestShip = _parsedLine.second;

        _toBeOccupied += _parsedLine.second; // add the length of the current ship to _toBeOccupied variable;

        registerShip(clsShip(_parsedLine.first, _parsedLine.second)); // construct a new ship from the parsed line and register it to the gamestate;
      }

      _maxShipLength = _longestShip; // set _maxShipLength;
      _maxOccupied = _toBeOccupied;

      cFile.close();
    }

  private:
    int _state = 0;
    int _playerCount;
    int _maxShipLength;
    int _maxOccupied;
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

  game -> readConfig(); // read the config.csv file for ships & create the classes for each ship;
  game -> startNewGame();// start a new game;
}
