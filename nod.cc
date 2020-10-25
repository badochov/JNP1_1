#include <iostream>
#include <regex>
#include <cmath>

namespace {
enum class LineType {
  INFO,
  QUERY,
  ERROR,
};

enum class RoadType : char {
  HIGHWAY = 'A',
  EXPRESSWAY = 'S'
};

using LineCounter = size_t;
using InputLine = std::pair<std::string, LineCounter>;

using Distance = unsigned long; // Distance travelled by car in hm (100m)

using LicensePlate = std::string;
using CarData = std::map<RoadType, Distance>;
using CarMemory = std::map<LicensePlate, CarData>;

using RoadNumber = int;
using RoadDistancePost = unsigned long; // Distance on road distance post stored in hm (100m)
using Road = std::pair<RoadNumber, RoadType>;
using RoadInfo = std::pair<Road, RoadDistancePost>;
using RoadMemory = std::map<Road, Distance>;

using EntranceLog = std::pair<RoadInfo, InputLine>;
using EntranceMemory = std::map<LicensePlate, EntranceLog>;

using Memory = std::tuple<CarMemory, RoadMemory, EntranceMemory>;

std::ostream &operator<<(std::ostream &os, const RoadType &road_type) {
  os << static_cast<std::underlying_type<RoadType>::type>(road_type);
  return os;
}

// Max distance written on distance post
constexpr RoadDistancePost MAX_ROAD_DISTANCE_POST = 1e8;
namespace nod_regex {
inline const std::string &get_license_plate_expression() {
  static std::string value = R"([a-zA-Z\d]{3,11})";
  return value;
}

inline const std::string &get_road_name_expression() {
  static std::string value = R"([AS][1-9]\d{0,2})";
  return value;
}

inline RoadDistancePost number_length(RoadDistancePost road_distance_post){
  int len =0;
  while(road_distance_post > 0){
    len ++;
    road_distance_post/=10;
  }

  return len;
}

inline const std::string &get_distance_expression() {
  static std::string value = R"((0|[1-9]\d{0,)" + std::to_string(number_length(MAX_ROAD_DISTANCE_POST)) + R"(}),\d)";
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

inline const std::regex &get_road_name_with_spaces_regex() {
  static std::regex value(R"(\s*)" + get_road_name_expression() + R"(\s*)");
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
  static std::regex value(R"(\s*\?\s*()" +
      get_license_plate_expression() +
      R"(|)" +
      get_road_name_expression() +
      R"()?\s*)");
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

inline void print_error(const InputLine &line) {
  std::cerr << "Error in line " << line.second << ": " << line.first << std::endl;
}

inline bool is_match_perfect(const std::smatch &match) {
  return match.prefix().str().empty() && match.suffix().str().empty();
}

inline bool check_match(const std::string &text, const std::regex &regex) {
  std::smatch match;
  return std::regex_search(text, match, regex) && is_match_perfect(match);
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
  std::regex_search(text, match, nod_regex::get_number_regex());

  RoadDistancePost distance = 10 * std::stoul(match.str());

  std::string decimal_part = match.suffix();
  std::regex_search(decimal_part, match, nod_regex::get_number_regex());

  distance += std::stoi(match.str());
  return distance;
}

inline Road parse_road_name(const std::string &road_name, std::smatch &match) {
  RoadType type = char_to_road_type(road_name[0]);
  std::string next_info = match.suffix();
  RoadNumber number = parse_road_number(road_name, match);
  return Road(number, type);
}

inline RoadInfo parse_road_info(const std::string &text, std::smatch &match) {
  std::regex_search(text, match, nod_regex::get_road_name_regex());
  std::string road_name = match.str();
  std::string next_info = match.suffix();

  Road road = parse_road_name(road_name, match);

  std::regex_search(next_info, match, nod_regex::get_distance_regex());
  std::string road_distance = match.str();
  RoadDistancePost distance_post = parse_distance_post(road_distance, match);

  return RoadInfo(road, distance_post);
}

inline bool are_roads_same(const RoadInfo &road_info1, const RoadInfo &road_info2) {
  return road_info1.first == road_info2.first;
}

inline CarMemory &get_car_memory(Memory &memory) {
  return std::get<0>(memory);
}

inline CarMemory const &get_car_memory(const Memory &memory) {
  return std::get<0>(memory);
}

inline RoadMemory &get_road_memory(Memory &memory) {
  return std::get<1>(memory);
}

inline RoadMemory const &get_road_memory(const Memory &memory) {
  return std::get<1>(memory);
}

inline const EntranceMemory &get_entrance_memory(const Memory &memory) {
  return std::get<2>(memory);
}

inline EntranceMemory &get_entrance_memory(Memory &memory) {
  return std::get<2>(memory);
}

inline bool is_on_other_road(const LicensePlate &license_plate,
                             const RoadInfo &road,
                             const Memory &memory) {
  const RoadInfo &prev_road = get_entrance_memory(memory).at(license_plate).first;
  return !are_roads_same(prev_road, road);
}

inline void handle_wrong_road(const LicensePlate &license_plate,
                              const RoadInfo &road_info,
                              const InputLine &input_line,
                              Memory &memory) {
  EntranceLog &entrance_log = get_entrance_memory(memory)[license_plate];
  print_error(entrance_log.second);
  entrance_log = EntranceLog(road_info, input_line);
}

template<class T, class S>
inline bool has_key(std::map<T, S> map, T key) {
  return map.count(key) == 1;
}

inline bool is_on_road(const LicensePlate &license_plate, const Memory &memory) {
  const EntranceMemory &entrance_memory = get_entrance_memory(memory);
  return has_key(entrance_memory, license_plate);
}

inline Distance calc_distance(const RoadInfo &road_entrance_info,
                              const RoadInfo &road_exit_info) {
  return (std::max(road_entrance_info.second, road_exit_info.second)
      - std::min(road_entrance_info.second, road_exit_info.second));
}

void add_data_to_car(
    const LicensePlate &license_plate,
    const Road &road,
    Distance distance,
    Memory &memory) {
  const RoadType &road_type = road.second;
  CarData &car_data = get_car_memory(memory)[license_plate];

  car_data[road_type] += distance;
}

void add_data_to_road(const Road &road,
                      Distance distance,
                      Memory &memory) {
  get_road_memory(memory)[road] += distance;
}

// Adds data about car's finished movement to memory
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
  EntranceMemory &entrance_memory = get_entrance_memory(memory);
  RoadInfo &road_entrance_info = entrance_memory[license_plate].first;
  add_data(road_entrance_info, road, license_plate, memory);
  entrance_memory.erase(license_plate);
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
  get_entrance_memory(memory)[license_plate] = {road, input_line};
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

void print_car_data(const LicensePlate &license_plate, const CarMemory &car_memory) {

  std::cout << license_plate;
  for (const auto &[road_type, distance] : car_memory.at(license_plate)) {
    std::cout << " " << road_type << " ";
    print_distance(distance);
  }
  std::cout << std::endl;
}

void query_car(const LicensePlate &license_plate, const Memory &memory) {
  const CarMemory &car_memory = get_car_memory(memory);
  if (!has_key(car_memory, license_plate))
    return;

  print_car_data(license_plate, car_memory);
}

inline void print_road(const Road &road) {
  std::cout << road.second << road.first;
}

void print_road_data(const Road &road, const RoadMemory &road_memory) {
  Distance distance = road_memory.at(road);
  print_road(road);
  std::cout << " ";
  print_distance(distance);
  std::cout << std::endl;
}

inline void query_road(const Road &road, const Memory &memory) {
  const RoadMemory &road_memory = get_road_memory(memory);
  if (has_key(road_memory, road)) {
    print_road_data(road, road_memory);
  }
}

void query_all_cars(const Memory &memory) {
  for (const auto &[license_plate, _] : get_car_memory(memory)) {
    query_car(license_plate, memory);
  }
}

void query_all_roads(const Memory &memory) {
  for (const auto &[road_id, _] : get_road_memory(memory)) {
    query_road(road_id, memory);
  }
}

//Performs query with no parameters
inline void general_query(const Memory &memory) {
  query_all_cars(memory);
  query_all_roads(memory);
}

//Assumes that line contains matching string.
void parse_info(const InputLine &line, Memory &memory) {
  std::smatch match;

  LicensePlate license_plate = parse_license_plate(line.first, match);
  RoadInfo road_info = parse_road_info(match.suffix(), match);

  log(license_plate, road_info, line, memory);
}

//Assumes that line contains string matching query.
void try_querying_car(const InputLine &line, Memory &memory) {
  std::smatch match;

  //We already checked correctness of the line,
  //so the only case in which match can't be found
  //is when the query argument is road name consisting of two characters (eg. S2)
  LicensePlate license_plate = parse_license_plate(line.first, match);
  if (!license_plate.empty()) {
    query_car(license_plate, memory);
  }
}

bool check_if_road_name_is_not_substring(std::smatch &match) {
  return (match.suffix().str().empty() &&
      check_match(match.prefix(), nod_regex::get_general_query_regex()));
}

void parse_road_name_and_query_road(const std::string &road_name_with_spaces,
                                    Memory &memory) {
  std::smatch match;
  std::regex_search(road_name_with_spaces, match, nod_regex::get_road_name_regex());
  Road road = parse_road_name(match.str(), match);
  query_road(road, memory);
}

//Assumes that line contains string matching query.
void try_querying_road(const InputLine &line, Memory &memory) {
  std::smatch match;
  //Correct road name can be a substring of a license plate,
  //eg. A1 is substring of QA1TYH12
  //Therefore, we need to confirm that
  //query's argument is not something like that.
  std::regex_search(line.first, match, nod_regex::get_road_name_with_spaces_regex());
  if (!match.str().empty() && check_if_road_name_is_not_substring(match)) {
    parse_road_name_and_query_road(match.str(), memory);
  }
}

//Assumes that line contains matching string.
void parse_query(const InputLine &line, Memory &memory) {
  if (check_match(line.first, nod_regex::get_general_query_regex())) {
    general_query(memory);
  } else {
    try_querying_car(line, memory);
    try_querying_road(line, memory);
  }
}

LineType get_line_type(const InputLine &line) {
  const std::string &text = line.first;

  if (check_match(text, nod_regex::get_car_movement_regex())) {
    return LineType::INFO;
  } else if (check_match(text, nod_regex::get_query_regex())) {
    return LineType::QUERY;
  } else {
    return LineType::ERROR;
  }
}

void parse_line(const InputLine &line, Memory &memory) {
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
