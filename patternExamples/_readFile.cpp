#include <iostream>
#include <string>
#include <fstream>

#define cFILE "config.csv"

struct battleship {
  int id;
  int type;
  int size;
  std::string name;
};

std::string cleanString(std::string text){
  std::string _local = text;
  std::string output;
  std::cout << "BEFORE : [" <<_local << "]\n";
  
  for(int i = 0; i < _local.length(); i++){
    // std::string temp;
    char start = _local[0];
    char end = _local[_local.length()];

    if (_local[0] == ' '){
      _local.erase(0, 1);
    }

    if(_local[_local.length()] == ' '){
      _local.erase(_local.length(), 1);
    }
  }

  std::cout << "AFTER : [" <<_local << "]\n";

  std::cout << "\nCleanString done: [" << _local << ']';

  return _local;
};

battleship parser(std::string line){
  std::string words[4];
  int currentWord = 0;
  battleship _battleship;

  for(int i = 0; i < line.length(); i++){
    if(line[i] == ','){
      currentWord++;
    } else {
      words[currentWord] += line[i];
    }
  }

  _battleship.id = std::stoi(words[0]);
  _battleship.type = std::stoi(words[1]);
  _battleship.size = std::stoi(words[2]);
  _battleship.name = cleanString(words[3]);

  return _battleship;
}

void ReadFile(void) {
	std::string currentShip; 
	int len;
  battleship _current;

	std::ifstream dbFile; 
	dbFile.open(cFILE); 
  
	while(!dbFile.eof()) { 
		getline(dbFile, currentShip);

    std::cout << "\nFound: [" << _current.name << ']';
    _current = parser(currentShip);

	}
	
	dbFile.close();

	std::cout << std::endl << "\nconfig file read";
}

// int main() {
// 	ReadFile();
// }