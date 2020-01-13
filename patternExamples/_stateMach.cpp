#include <iostream>
#include <ctime>

using namespace std;

const int NO_QUARTER = 0;
const int HAS_QUARTER = 1;

const int INSERT_QUARTER = 1;
const int EJECT_QUARTER = 2;
const int TURN_CRANK = 3;
const int CHECK_STOCK = 4;
const int EXIT = 5;


class gumballMachine {
	public:
    gumballMachine(int initialState, int stock, int userOption){
      setState(initialState);
      setStock(stock);
      setUserOption(userOption);
    }

		int getState() {
			return state;
		}

    void getStock() {
      status = "Stock level: " + to_string(stock) + "\n";
    }

		void insertQuarter() {
			if (state == NO_QUARTER) {
				status = "Quarter inserted\n";
				setState(HAS_QUARTER);
			} else {
				status = "Quarter could not be inserted\n";
			}
		}

		void ejectQuarter() {
			if(state == HAS_QUARTER ) {
				status = "Quarter ejected\n";
				setState(NO_QUARTER);
			} else {
				status = "no Quarter to eject\n";
			}
		}

    int rollDice(){
      srand(time(NULL));
      int random_number = rand() % 10;
      return random_number;
    }

    void turnCrank() {
      if(state == HAS_QUARTER && stock > 0){
        if(rollDice() == 0){
          cout << "You have recieved a BONUS GUMBALL!";
          stock --;
        }

        status = "Gumball Sold!\n";
        setState(NO_QUARTER);
        stock--;
      } else if(state == HAS_QUARTER && stock == 0){
        status =  "No stock left!\n";
      } else {
        status = "No quarter in machine\n";
      }
    }

    void startMachine() {
      while(state != EXIT){
        cout << "\033[2J\033[0;0H";
        cout << "Welcome to my Gumball State Machine!\n";
        cout << "Please select an option\n";
        cout << "1. Insert Quarter\n";
        cout << "2. Eject Quarter\n";
        cout << "3. Turn Crank\n";
        cout << "4. Check Stock\n";
        cout << "5. Exit\n";
        cout << status;
        cin >> userOption;

        switch(userOption){
          case(INSERT_QUARTER): insertQuarter(); break;
          case(EJECT_QUARTER): ejectQuarter(); break;
          case(TURN_CRANK): turnCrank(); break;
          case(CHECK_STOCK): getStock(); break;
          case(EXIT): setState(EXIT); break;
        }
      }
      cout << "\nProgramme exited successfully";
    }

	protected:

	private:
		int state;
    int stock;
    int userOption;
    string status;

		void setState(int newState) {
			state=newState;
		}

		void setStock(int newStock) {
			stock=newStock;
		}

		void setUserOption(int newOption) {
			userOption=newOption;
		}
};



// int main() {

// 	gumballMachine gumball(NO_QUARTER, 2, 0);

//   gumball.startMachine();
 
// }