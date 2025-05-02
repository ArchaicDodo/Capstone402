#include <iostream>

int main() {
    std::cout << "PLC Control Human-Machine Interface Commands:" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "plcdisplay [sensor]" << std::endl;
    std::cout << "  Displays current levels as indicated by the inputs." << std::endl;
    std::cout << "  Arguments:" << std::endl;
    std::cout << "    co2  - CO2 levels (ppm)" << std::endl;
    std::cout << "    ec   - Electrical conductivity (S/m)" << std::endl;
    std::cout << "    ph   - pH level" << std::endl;
    std::cout << "    wfr  - Water flow rate (L/s)" << std::endl;
    std::cout << "    lvl  - Water level (submerged/not submerged)" << std::endl;
    std::cout << "  Without arguments, displays all levels." << std::endl;
    std::cout << std::endl;
    
    std::cout << "plcrefresh" << std::endl;
    std::cout << std::endl;
    
    std::cout << "plcoverride <output> <state>" << std::endl;
    std::cout << "  Overrides an output to a specific state." << std::endl;
    std::cout << "  Arguments:" << std::endl;
    std::cout << "    output: co2" << std::endl;
    std::cout << "    state: on, off" << std::endl;
    std::cout << std::endl;
    
    std::cout << "plcrestart" << std::endl;
    std::cout << "  Restarts the control loop program." << std::endl;
    std::cout << std::endl;
    
    std::cout << "plchelp" << std::endl;
    std::cout << "  Displays this help message." << std::endl;
    
    return 0;
}