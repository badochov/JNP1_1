#include <iostream>
#include <regex>
#include <cmath>

namespace {
namespace nod_regex {
inline const std::string &get_license_plate_expression() {
  static std::string value = R"([a-zA-Z\d]{3,11})";
  return value;
}

inline const std::string &get_road_name_expression() {
  static std::string value = R"([AS][1-9]\d{0,2})";
  return value;
}

inline const std::string &get_distance_expression() {
  static std::string value = R"((0|[1-9]\d*),\d)";
  return value;
}

inline const std::regex &get_license_plate_regex() {
  static std::regex value(get_license_plate_expression());
  return value;
}

inline const std::regex &get_road_name_regex() {
  static std::regex value(get_road_name_expression());
  return value;
}

inline const std::regex &get_distance_regex() {
  static std::regex value(get_distance_expression());
  return value;
}

inline const std::regex &get_car_movement_regex() {
  static std::regex value(R"(\s*()"
                              + get_license_plate_expression() + R"()\s+()"
                              + get_road_name_expression() + R"()\s+()"
                              + get_distance_expression() + R"()\s*)");
  return value;
}

inline const std::regex &get_query_regex() {
  static std::regex value(R"(\s*\?()" + get_license_plate_expression() + R"()?\s*)");
  return value;
}

inline const std::regex &get_general_query_regex() {
  static std::regex value(R"(\s*\?\s*)");
  return value;
}

inline const std::regex &get_number_regex() {
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
  EXPRESSWAY = 'S',
  NO_ROAD
};

std::ostream &operator<<(std::ostream &os, const RoadType &road_type) {
  os << static_cast<std::underlying_type<RoadType>::type>(road_type);
  return os;
}
using LicensePlate = std::string;
using RoadNumber = int;
using RoadDistancePost = int; //FIXME: may be convenient to change it to unsigned long long

using Road = std::pair<RoadNumber, RoadType>;

using RoadInfo = std::pair<Road, RoadDistancePost>;

using LineCounter = size_t;
using InputLine = std::pair<std::string, LineCounter>;

using Distance = unsigned long long;


using RoadMemory = std::map<Road, Distance>;
using EntranceLog = std::pair<RoadInfo, InputLine>;
using CarData = std::pair<std::map<RoadType, Distance>, EntranceLog>;
using CarMemory = std::map<LicensePlate, CarData>;
using Memory = std::pair<CarMemory, RoadMemory>; //TODO: may change in future but doesn't really matter rn

constexpr std::pair NOT_ON_ROAD = {0, RoadType::NO_ROAD};

inline bool is_match_perfect(const std::smatch &match) {
  return match.prefix().str().empty() && match.suffix().str().empty();
}

inline bool check_match(const std::string &text, const std::regex &regex) {
  std::smatch match;
  return std::regex_search(text, match, regex) && is_match_perfect(match);
}

LineType get_line_type(const InputLine &line) {
  const std::string &text = line.first;
  std::smatch match;
  if (check_match(text, nod_regex::get_car_movement_regex())) {
    return LineType::INFO;
  } else if (check_match(text, nod_regex::get_query_regex())) {
    return LineType::QUERY;
  } else {
    return LineType::ERROR; //TODO handle empty line as it's not error
  }
}

RoadType char_to_road_type(char ch) {
  if (ch == 'A') {
    return RoadType::HIGHWAY;
  } else {
    return RoadType::EXPRESSWAY;
  }
}

inline LicensePlate parse_license_plate(const std::string &text, std::smatch &match) {
  std::regex_search(text, match, nod_regex::get_license_plate_regex());
  return match.str();
}

inline RoadNumber parse_road_number(const std::string &text, std::smatch &match) {
  std::regex_search(text, match, nod_regex::get_number_regex());
  return std::stoi(match.str());
}

inline RoadDistancePost parse_distance_post(const std::string &text, std::smatch &match) {
  // TODO: change stoi to stol if we decide to change RoadDistancePost type
  std::regex_search(text, match, nod_regex::get_number_regex());
  RoadDistancePost distance = 10 * std::stoi(match.str());
  std::string decimal_part = match.suffix();
  std::regex_search(decimal_part, match, nod_regex::get_number_regex());
  distance += std::stoi(match.str());
  return distance;
}

inline RoadInfo parse_road_info(const std::string &text, std::smatch &match) {
  std::regex_search(text, match, nod_regex::get_road_name_regex());
  std::string road_name = match.str();

  RoadType type = char_to_road_type(road_name[0]);
  std::string next_info = match.suffix();
  RoadNumber number = parse_road_number(road_name, match);

  std::regex_search(next_info, match, nod_regex::get_distance_regex());
  std::string road_distance = match.str();
  RoadDistancePost distance_post = parse_distance_post(road_distance, match);

  Road road(number, type);
  return RoadInfo(road,
                  distance_post); //TODO very quick and dirty fix, pls change it to be less cancerogenous
}

void print_error(const InputLine &line) {
  std::cerr << "Error in line " << line.second << ": " << line.first << std::endl;
}

inline bool are_roads_same(const RoadInfo &road_info1, const RoadInfo &road_info2) {
  return road_info1.first == road_info2.first;
}

inline const CarData &get_const_car_data(const LicensePlate &license_plate, const Memory &memory) {
  return memory.first.at(license_plate);
}

inline CarData &get_car_data(const LicensePlate &license_plate, Memory &memory) {
  return memory.first[license_plate];
}

inline const Distance &get_const_road_data(const Road &road, const Memory &memory) {
  return memory.second.at(road);
}

inline bool is_on_other_road(const LicensePlate &license_plate, const RoadInfo &road, const Memory &memory) {
  const RoadInfo &prev_road = get_const_car_data(license_plate, memory).second.first;
  return !are_roads_same(prev_road, road);
}

inline void handle_wrong_road(const LicensePlate &license_plate,
                              const RoadInfo &road_info,
                              const InputLine &input_line,
                              Memory &memory) {

  print_error(get_car_data(license_plate, memory).second.second);
  get_car_data(license_plate, memory).second = EntranceLog(road_info, input_line);
}

template<class T, class S>
inline bool has_key(std::map<T, S> map, T key) {
  return map.count(key) == 1;
}

inline bool is_on_road(const LicensePlate &license_plate, const Memory &memory) {
  return has_key(memory.first, license_plate)
      && get_const_car_data(license_plate, memory).second.first.first.second != RoadType::NO_ROAD;
}

inline Distance calc_distance(const RoadInfo &road_entrance_info, const RoadInfo &road_exit_info) {
  return static_cast<Distance>(std::max(road_entrance_info.second, road_exit_info.second)
      - std::min(road_entrance_info.second, road_exit_info.second));
}

void add_data_to_car(
    const LicensePlate &license_plate,
    const Road &road,
    Distance distance,
    Memory &memory) {
  const RoadType &road_type = road.second;
  auto &car_data = get_car_data(license_plate, memory).first;

  car_data[road_type] += distance;
}

void add_data_to_road(const Road &road,
                      Distance distance,
                      Memory &memory) {
  memory.second[road] += distance;
}

void add_data(const RoadInfo &road_entrance_info,
              const RoadInfo &road_exit_info,
              const LicensePlate &license_plate,
              Memory &memory) {
  Road road = road_entrance_info.first;
  Distance distance = calc_distance(road_entrance_info, road_exit_info);
  add_data_to_car(license_plate, road, distance, memory);
  add_data_to_road(road, distance, memory);
}

inline void handle_valid_road_exit_log(const LicensePlate &license_plate,
                                       const RoadInfo &road,
                                       Memory &memory) {

  RoadInfo &road_entrance_info = get_car_data(license_plate, memory).second.first;
  add_data(road_entrance_info, road, license_plate, memory);
  get_car_data(license_plate, memory).second.first.first = NOT_ON_ROAD;
}

inline void handle_road_exit_log(const LicensePlate &license_plate,
                                 const RoadInfo &road,
                                 const InputLine &line_input,
                                 Memory &memory) {
  if (is_on_other_road(license_plate, road, memory)) {
    handle_wrong_road(license_plate, road, line_input, memory);
  } else {
    handle_valid_road_exit_log(license_plate, road, memory);
  }
}

inline void handle_road_entrance_log(const LicensePlate &license_plate,
                                     const RoadInfo &road,
                                     const InputLine &input_line,
                                     Memory &memory) {
  get_car_data(license_plate, memory).second = {road, input_line};
}

void log(const LicensePlate &license_plate,
         const RoadInfo &road,
         const InputLine &input_line,
         Memory &memory) {
  if (is_on_road(license_plate, memory)) {
    handle_road_exit_log(license_plate, road, input_line, memory);
  } else {
    handle_road_entrance_log(license_plate, road, input_line, memory);
  }
}

inline void print_distance(Distance distance) {
  std::cout << distance / 10 << "," << distance % 10;
}

void print_car_data(const LicensePlate &license_plate, const std::map<RoadType, Distance> &road_category_map) {

  std::cout << license_plate;
  for (const auto &[road_type, distance] : road_category_map) {
    std::cout << " " << road_type << " ";
    print_distance(distance);
  }
  std::cout << std::endl;
}

void query_car(const LicensePlate &license_plate, const Memory &memory) {
  if (!has_key(memory.first, license_plate))
    return;

  const std::map<RoadType, Distance> &road_category_map = get_const_car_data(license_plate, memory).first;
  if (road_category_map.empty())
    return;
  print_car_data(license_plate, road_category_map);
}

inline void print_road(const Road &road) {
  std::cout << road.second << road.first;
}

void print_road_data(const Road &road, const Memory &memory) {
  Distance distance = get_const_road_data(road, memory);
  print_road(road);
  std::cout << " ";
  print_distance(distance);
  std::cout << std::endl;
}

inline void query_road(const Road &road, const Memory &memory) {

  if (!has_key(memory.second, road))
    return;
  print_road_data(road, memory);
}

void query_cars(const Memory &memory) {
  for (const auto &[license_plate, _] : memory.first) {
    query_car(license_plate, memory);
  }
}

void query_roads(const Memory &memory) {
  for (const auto &[road_id, _] : memory.second) {
    query_road(road_id, memory);
  }
}

inline void general_query(const Memory &memory) {
  query_cars(memory);
  query_roads(memory);
}

//Assumes that line contains matching string.
void parse_info(const InputLine &line, Memory &memory) {
  std::smatch match;

  LicensePlate license_plate = parse_license_plate(line.first, match);
  RoadInfo road_info = parse_road_info(match.suffix(), match);

  //TODO: log should return the line with error (if needed) or print the error by itself
  log(license_plate, road_info, line, memory);
}

void try_querying_car(const InputLine &line, Memory &memory) {
  std::smatch match;

  LicensePlate license_plate = parse_license_plate(line.first, match);
  if (!license_plate.empty()) {
    query_car(license_plate, memory);
  }
}

void try_querying_road(const InputLine &line, Memory &memory) {
  std::smatch match;

  std::regex_search(line.first, match, nod_regex::get_road_name_regex());
  std::string road_name = match.str();
  if (!road_name.empty()) {
    RoadType type = char_to_road_type(road_name[0]);
    RoadNumber number = parse_road_number(road_name, match);
    Road road(number, type);
    query_road(road, memory);
  }
}

//Assumes that line contains matching string.
void parse_query(const InputLine &line, Memory &memory) {
  // May be inefficient, I'm not sure how regex work.
  if (check_match(line.first, nod_regex::get_general_query_regex())) {
    general_query(memory);
  } else {
    try_querying_car(line, memory);
    try_querying_road(line, memory);
  }
}

void parse_line(const InputLine &line, Memory &memory) { // TODO: finish all the branches
  switch (get_line_type(line)) {
    case LineType::INFO:parse_info(line, memory);
      break;
    case LineType::QUERY:parse_query(line, memory);
      break;
    case LineType::ERROR:print_error(line);
      break;
  }
}

void process_input(Memory &memory) {
  std::string current_line;
  LineCounter line_counter = 0;

  while (std::getline(std::cin, current_line)) {
    line_counter++;
    if (!current_line.empty()) {
      parse_line(InputLine(current_line, line_counter), memory);
    }
  }
}
}

int main() {
  Memory memory;
  process_input(memory);
  return 0;
}
