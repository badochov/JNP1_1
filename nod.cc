#include <iostream>
#include <regex>

namespace nod_regex { //TODO: consider whether the name is good, maybe move to function
static inline const std::string& get_license_plate_expression()
{
  static std::string value = R"([a-zA-Z\d]{3,11})";
  return value;
}

static inline const std::string& get_road_name_expression() {
  static std::string value = R"([AS][1-9]\d{0,2})";
  return value;
}

static inline const std::string& get_distance_expression() {
  static std::string value = R"((0|[1-9]\d*),\d)";
  return value;
}

static inline const std::regex& get_license_plate_regex() {
  static std::regex value(get_license_plate_expression());
  return value;
}

static inline const std::regex& get_road_name_regex() {
  static std::regex value(get_road_name_expression());
  return value;
}

static inline const std::regex& get_distance_regex() {
  static std::regex value(get_distance_expression());
  return value;
}

static inline const std::regex& get_car_movement_regex() {
  static std::regex value(R"(\s*()"
                          + get_license_plate_expression() + R"()\s+()"
                          + get_road_name_expression() + R"()\s+()"
                          + get_distance_expression() + R"()\s*)");
  return value;
}

static inline const std::regex& get_query_regex() {
  static std::regex value(R"(\s*\?()" + get_license_plate_expression() + R"()?\s*)");
  return value;
}

static inline const std::regex& get_general_query_regex() {
  static std::regex value(R"(\s*\?\s*)");
  return value;
}

static inline const std::regex& get_number_regex() {
  static std::regex value(R"(\d+)");
  return value;
}
}

enum class LineType {
  INFO,
  QUERY,
  ERROR
};

enum class RoadType : char {
  HIGHWAY = 'A',
  EXPRESSWAY = 'S'
};

using LicensePlate = std::string;
using RoadNumber = int;
using RoadDistancePost = int; //TODO: may be convenient to change it to unsigned long long
using RoadInfo = std::tuple<RoadType, RoadNumber, RoadDistancePost>;

using LineCounter = size_t;
using InputLine = std::pair<std::string, LineCounter>;

using Memory = std::map <RoadType, std::map<RoadNumber, std::map <LicensePlate, std::vector <int>>>>; //TODO: may change in future but doesn't really matter rn


void log(LicensePlate &licensePlate,
         RoadInfo road,
         const InputLine &input_line,
         Memory &memory) {

}

void general_query(Memory &memory) {

}

void query_car(LicensePlate &licensePlate, Memory &memory) {

}

void query_road(RoadType roadType, RoadNumber roadNumber, Memory &memory) {

}

static inline bool is_match_perfect(const std::smatch &match) {
  return match.prefix().str().empty() && match.suffix().str().empty();
}

static inline bool check_match(const std::string &text, const std::regex &regex) {
  std::smatch match;
  return std::regex_search(text, match, regex) && is_match_perfect(match);
}

static LineType get_line_type(const InputLine &line) {
  const std::string &text = line.first;
  std::smatch match;
  if(check_match(text, nod_regex::get_car_movement_regex())) {
    return LineType::INFO;
  }
  else if(check_match(text, nod_regex::get_query_regex())) {
    return LineType::QUERY;
  }
  else {
    return LineType::ERROR;
  }
}

static RoadType char_to_road_type(char ch) {
  if(ch == 'A') {
    return RoadType::HIGHWAY;
  }
  else {
    return RoadType::EXPRESSWAY;
  }
}

static inline LicensePlate parse_license_plate(const std::string &text, std::smatch &match) {
  std::regex_search(text, match, nod_regex::get_license_plate_regex());
  return match.str();
}

static inline RoadNumber parse_road_number(const std::string &text, std::smatch &match) {
  std::regex_search(text, match, nod_regex::get_number_regex());
  return std::stoi(match.str());
}

static inline RoadDistancePost parse_distance_post(const std::string &text, std::smatch &match) {
  // TODO: change stoi to stol if we decide to change RoadDistancePost type
  std::regex_search(text, match, nod_regex::get_number_regex());
  RoadDistancePost distance = 10 * std::stoi(match.str());
  std::string decimal_part = match.suffix();
  std::regex_search(decimal_part, match, nod_regex::get_number_regex());
  distance += std::stoi(match.str());
  return distance;
}

static inline RoadInfo parse_road_info(const std::string &text, std::smatch &match) {
  std::regex_search(text, match, nod_regex::get_road_name_regex());
  std::string road_name = match.str();

  RoadType type = char_to_road_type(road_name[0]);

  RoadNumber number = parse_road_number(road_name, match);
  std::string next_info = match.suffix();

  std::regex_search(next_info, match, nod_regex::get_distance_regex());
  std::string road_distance = match.str();
  RoadDistancePost distance_post = parse_distance_post(road_distance, match);

  return RoadInfo(type, number, distance_post);
}

//Assumes that line contains matching string.
static void parse_info(const InputLine &line, Memory &memory) {
  std::smatch match;

  LicensePlate license_plate = parse_license_plate(line.first, match);
  RoadInfo road_info = parse_road_info(match.suffix(), match);

  //TODO: log should return the line with error (if needed) or print the error by itself
  log(license_plate, road_info, line, memory);
}

static void try_querying_car(const InputLine &line, Memory &memory) {
  std::smatch match;

  LicensePlate license_plate = parse_license_plate(line.first, match);
  if(!license_plate.empty()) {
    query_car(license_plate, memory);
  }
}

static void try_querying_road(const InputLine &line, Memory &memory) {
  std::smatch match;

  std::regex_search(line.first, match, nod_regex::get_road_name_regex());
  std::string road_name = match.str();
  if(!road_name.empty()) {
    RoadType type = char_to_road_type(road_name[0]);
    RoadNumber number = parse_road_number(road_name, match);
    query_road(type, number, memory);
  }
}

//Assumes that line contains matching string.
static void parse_query(const InputLine &line, Memory &memory) {
  // May be inefficient, I'm not sure how regex work.
  if(check_match(line.first, nod_regex::get_general_query_regex())) {
    general_query(memory);
  }
  else {
    try_querying_car(line, memory);
    try_querying_road(line, memory);
  }
}

static inline void print_error(const InputLine &line) {
  std::cerr << "Error in line " << line.second << ": " << line.first << std::endl;
}

static void parse_line(const InputLine &line, Memory &memory) { // TODO: finish all the branches
  switch (get_line_type(line)) {
    case LineType::INFO:
      parse_info(line, memory);
      break;
    case LineType::QUERY:
      parse_query(line, memory);
      break;
    case LineType::ERROR:
      print_error(line);
      break;
  }
}

void process_input(Memory &memory) {
  std::string current_line;
  LineCounter line_counter = 0;

  while(std::getline(std::cin, current_line)) {
    line_counter++;
    if(!current_line.empty()) {
      parse_line(InputLine(current_line, line_counter), memory);
    }
  }
}

int main() {
  Memory memory;
  process_input(memory);
  return 0;
}
