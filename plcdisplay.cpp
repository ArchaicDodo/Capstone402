#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <ctime>
#include <sstream>

const std::string CMD_IPC = "/home/pi/.shared/control/cmdipc";
const std::string RSP_IPC = "/home/pi/.shared/control/rspipc";

const std::unordered_map<std::string, int> sensorArgs = {
    {"co2", 10},
    {"wfr", 11},
    {"lvl", 12},
    {"ec", 13},
    {"ph", 14}
};

const std::unordered_map<int, std::string> units = {
    {10, "ppm"},
    {11, "L/s"},
    {12, "(0: Not submerged, 1: Submerged)"},
    {13, "S/m"},
    {14, ""}
};

int getSensorArgCode(const std::string& sensorName) {
    if (sensorArgs.count(sensorName)) return sensorArgs.at(sensorName);
    return 100; // default to all
}

void writeCommand(int argCode) {
    std::ofstream cmdFile(CMD_IPC, std::ios::trunc);
    if (!cmdFile) {
        std::cerr << "Failed to open command file.\n";
        exit(1);
    }
    time_t now = std::time(nullptr);
    cmdFile << now << " 0 " << argCode << "\n";
    cmdFile.close();
}

void displaySensorData(int requestedArg) {
    std::ifstream rspFile(RSP_IPC);
    if (!rspFile) {
        std::cerr << "Failed to open response file.\n";
        exit(1);
    }

    std::string line;
    while (std::getline(rspFile, line)) {
        std::istringstream iss(line);
        int timestamp, argCode;
        float value;
        if (!(iss >> timestamp >> argCode >> value)) continue;

        if (requestedArg == 100 || argCode == requestedArg) {
            std::string name;
            for (const auto& pair : sensorArgs) {
                if (pair.second == argCode) {
                    name = pair.first;
                    break;
                }
            }
            std::cout << name << ": " << value << " " << units.at(argCode) << "\n";
        }
    }

    rspFile.close();
}

int main(int argc, char* argv[]) {
    int argCode = 100; 

    if (argc == 2) {
        std::string sensor = argv[1];
        if (sensorArgs.count(sensor)) {
            argCode = sensorArgs.at(sensor);
        } else {
            std::cerr << "Invalid sensor name. Valid options: co2, wfr, lvl, ec, ph\n";
            return 1;
        }
    } else if (argc > 2) {
        std::cerr << "Usage: plcdisplay [sensor]\n";
        return 1;
    }

    writeCommand(argCode);
    displaySensorData(argCode);

    return 0;
}
