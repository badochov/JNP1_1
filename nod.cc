#include <iostream>
#include <regex>

namespace nod_regex { //TODO: consider whether the name is good, maybe move to function
std::string expression_license_plate = R"([a-zA-Z\d]{3,11})";
std::string expression_road_name = R"([AS][1-9]\d{0,2})";
std::string expression_distance = R"((0|1-9\d*),\d)";

std::regex license_plate(expression_license_plate);
std::regex road_name(expression_road_name);
std::regex distance(expression_distance);
std::regex car_movement_info(R"(\s*()" + expression_license_plate + R"()\s+()" + expression_road_name + R"()\s+()" + expression_distance + R"()\s*)");
std::regex query_car(R"(\s*\?\s+)" +  expression_license_plate + R"(\s*)");
std::regex query_road(R"(\s*\?\s+)" +  expression_road_name + R"(\s*)");
std::regex query_all(R"(\s*\?\s*)");
}

enum class LineType {
  INFO,
  QUERY,
  ERROR
};

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

void parse_line(const InputLine &line, Memory &memory) {

}

int main() {
  Memory memory;
  std::string current_line;
  unsigned long long line_counter = 0;

  while(std::getline(std::cin, current_line)) {
    line_counter++;
    if(!current_line.empty()) {
      parse_line(InputLine(current_line, line_counter), memory);
    }
  }
  return 0;
}
