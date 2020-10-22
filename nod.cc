#include <iostream>
#include <regex>


enum class RoadType {
  HIGHWAY = 'A',
  EXPRESSWAY = 'S'
};

using LicensePlate = std::string;
using RoadNumber = int;
using RoadDistancePost = int;
using RoadInfo = std::tuple<RoadType, RoadNumber, RoadDistancePost>;

using InputLine = std::tuple<std::string, long long>;

using Memory = std::map <RoadType, std::map<RoadNumber, std::map < LicensePlate, std::vector <int>>>>; //TODO: may change in future but doesn't really matter rn


void log(LicensePlate &licensePlate,
         RoadInfo road,
         const InputLine &input_line,
         Memory &memory) {

}

void general_query(Memory &memory) {

}

void query_car(LicensePlate *licensePlate, Memory &memory) {

}

void query_road(RoadType roadType, RoadNumber roadNumber, Memory &memory) {

}

int main() {
  Memory memory;
  return 0;
}
