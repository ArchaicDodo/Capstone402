#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>

const std::string CMD_IPC = "/home/pi/.shared/control/cmdipc";
const std::string RSP_IPC = "/home/pi/.shared/control/cmdipc";

void wrtieRefreshCommand() {
    std::ofstream cmdFile(CMD_IPC, std::ios::trunc);
    if (!cmdFile){
        std::cerr <<"Failed to open command file.\n";;
        exit(1);
    }
    time_t now = std::time(nullptr);
    cmdFile << now<< "1\n";
    cmdFile.close();
}

void readResponse() {
    std::ifstream rspFile(RSP_IPC);
    if (!rspFile) {
        std::cerr << "Failed to open response file.\n";
        exit(1);
    }

    std::string line;
    if (std::getline(rspFile, line)) {
        std::istringstream iss(line);
        int timestamp, status;
        if (iss >> timestamp >> status) {
            if (status == 0) {
                std::cout << "Refresh successful.\n";
            } else {
                std::cout << "Refresh failed.\n";
            }
        } else {
            std::cerr << "Malformed response.\n";
        }
    } else {
        std::cerr << "No response received.\n";
    }

    rspFile.close();
}

int main (int argc, char*argv[]){
    if (argc != 1) {
        std::cerr << "usage: plcrefresh\n";
        return 1;
    }

    wrtieRefreshCommand();
    readResponse();
}