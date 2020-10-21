#include <iostream>

enum class RoadType {
    HIGHWAY = "A", EXPRESSWAY = "S"
};

using LicensePlate = string;
using RoadNumber = int;
using RoadDistancePost = int;

using Memory = map <RoadType, map<RoadNumber, map < LicensePlate, vector < int>>>>; // may change in future but doesn't really matter rn


void log(LicensePlate &licensePlate,
         RoadType roadType,
         RoadNumber roadNumber,
         RoadDistancePost roadDistancePost,
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
