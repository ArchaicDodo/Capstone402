#include "rpiplc.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <modbus.h>
#include <time.h>

// Function declarations
unsigned long pulseIn(unsigned int pin, short value, float timeout = 5e9, 
                      int multiplier = 1);
bool waterLevel(unsigned int pin);
float waterFlow(unsigned int pin);
float co2Level(unsigned int pin);
short modbusLevel(int address, int reg);

	
int main(int argc, char* argv[]) {
  // Begin setup code, run once
  // Set pins
  pinMode(A0_5, OUTPUT);
  pinMode(A0_6, OUTPUT);
  pinMode(I0_0, INPUT);
  pinMode(I0_1, INPUT);
  pinMode(I0_2, INPUT);
  // Set modbus power
  analogWrite(A0_6, 4095);
  // Timestamp for checking
  unsigned int timestamp = 0;
  // Read config files
  FILE *config;
  config = fopen("config.txt", "r");
  float co2val, wfval, ecval, phval;
  for (int i = 0; i < 4; i++) {
    fscanf(file, "%*[^0-9.]%f%*[^0-9.]%f%*[^0-9.]%f%*[^0-9.]%f", 
           &co2val, &wfval, &ecval, &phval);
  }
  fclose(config);

  while (1) {
    // Begin control loop code, running indefinitely
    // Read values
    bool wlvl = waterLevel(I0_1);
    float co2lvl = co2Level(I0_0);
    float flwlvl = waterFlow(I0_2);
    float eclvl = modbusLevel(1, 40103)/1000);
    float phlvl = modbusLevel(1, 40102)/1000);
    
    // Adjust
    if (!wlvl) {
      // Run pumps to water tank
    }
    if (flwlvl > 1.05*wfval) {
      // Slow pumps to trays
      // No pumps presently implemented
    } else if (flwlvl < 0.95*wfval) {
      // Speed up pumps to trays
      // No pumps presently implemented
    }
    if (co2lvl < 0.95*co2val) {
      // Release CO2
      analogWrite(A0_5, 2047);
      delay(4000);
      analogWrite(A0_5, 0);
    }
    if (eclvl > 1.05*ecval && phlvl < 0.95*phval) {
      // Adjust nutrient levels
      // I don't know what combinations would be entailed
    }
    
    
    // HMI things
    FILE *cmdipc; 
    cmdipc = fopen("/home/pi/.shared/control/cmdipc","r");
    FILE *rspipc; 
    rspipc = fopen("/home/pi/.shared/control/rspipc","w");
    int instr[4];
    fscanf(cmdipc, "%d", &instr[0]); // Get UNIX timestamp
    if (instr[0] > timestamp) {
      timestamp = instr[0];
      fscanf(cmdipc, "%d", &instr[1]); // Get command
      switch (instr[1]) {
      case 0:
        // plcdisplay
        fscanf(cmdipc, "%d", &instr[2]); // Get argument
        switch (instr[2]) {
        case 10: // co2
          fprintf(rspipc, "%ld %d %.2f", time(NULL), 10, co2Level(I0_0));
          break;
        case 11: // wfr
          fprintf(rspipc, "%ld %d %.2f", time(NULL), 10, waterFlow(I0_2));
          break;
        case 12: // lvl
          fprintf(rspipc, "%ld %d %d", time(NULL), 10, waterLevel(I0_1));
          break;
        case 13: // ec
          fprintf(rspipc, "%ld %d %.2f", time(NULL), 10, modbusLevel(1, 40103)/1000);
          break;
        case 14: // ph
          fprintf(rspipc, "%ld %d %.2f", time(NULL), 10, modbusLevel(1, 40102)/1000);
          break;
        case 100: // all
          fprintf(rspipc, "%ld %d %.2f\n", time(NULL), 10, co2Level(I0_0));
          fprintf(rspipc, "%ld %d %.2f\n", time(NULL), 10, waterFlow(I0_2));
          fprintf(rspipc, "%ld %d %d\n", time(NULL), 10, waterLevel(I0_1));
          fprintf(rspipc, "%ld %d %.2f\n", time(NULL), 10, modbusLevel(1, 40103)/1000);
          fprintf(rspipc, "%ld %d %.2f\n", time(NULL), 10, modbusLevel(1, 40102)/1000);
          break;
        default:
          fprintf(rspipc, "%ld 0 0", time(NULL));
          break;
        }
        break;
      case 1:
        // plcrefresh
        for (int i = 0; i < 4; i++) {
          fscanf(file, "%*[^0-9.]%f%*[^0-9.]%f%*[^0-9.]%f%*[^0-9.]%f", 
                 &co2val, &wfval, &ecval, &phval);
        }
        fclose(config);
        fprintf(rspipc, "%ld 0", time(NULL));
        break;
      case 2:
        // plcoverride
        fscanf(cmdipc, "%d", &instr[2]); // Get argument
        fscanf(cmdipc, "%d", &instr[3]); // Get state
        switch (instr[2]) {
        case 10:
          analogWrite(A0_5, instr[3]);
          fprintf(rspipc, "%ld 0", time(NULL));
          break;
        default:
          fprintf(rspipc, "%ld 1", time(NULL));
          break;
        }
        break;
      default:
        break;
      }
    }
    fclose(cmdipc);
    fclose(rspipc);
    delay(5); // For stability
  }
  return 0;
}


// Function definitions
unsigned long pulseIn(unsigned int pin, short value, float timeout, 
                      int multiplier) {
  // A crude recreation of Arduino's pulseIn() function, with extensions
  unsigned long waitTime = 0; // Time the function has been running
  unsigned long pulseTime = 0; // Time of the pulse
  bool pulse = false; // Whether the pulse is in progress
  while (waitTime < timeout) {
    delayMicroseconds(1*multiplier);
    short read = digitalRead(pin);
    if (read == value && pulseTime == 0) { // If we're in the state before pulse
      pulse = true;
    } else if (read == value) { // If the pulse has ended
      pulseTime++;
      break;
    } else if (read != value && pulse) { // If we're in the pulse
      pulseTime++;
    }
    waitTime++;
  }
  return pulseTime;
}

bool waterLevel(unsigned int pin) {
  // A small function, effectively only here as a placeholder for if a water
  // level sensor with more complex behavior is used
  short read = digitalRead(pin);
  if (read == 0) {
    return false;
  } else {
    return true;
  }
}

float waterFlow(unsigned int pin) {
  // Returns water flow rate in L/s
  long high = pulseIn(pin, 1);
  float pulse;
  if (high == 0) {
    pulse = 0;
  } else {
    pulse = 1000000.0 / (2 * high);
  }
  float flowLs = pulse / 7.5; // Rough calibration
  return flowLs;
}

float co2Level(unsigned int pin) {
  // Returns CO2 level in ppm
  int timeHigh = pulseIn(pin, 0, 5e9, 1000);
  int timeLow = 1004 - timeHigh;
  float co2PPM = 2000 * (timeHigh-2.0) / (timeHigh+timeLow-4.0);
  return co2PPM;
}

short modbusLevel(int address, int reg) {
  // Get a value using the AgrowLink system, returns -1 for any errors
  // Set LX2 parameters
  modbus_t *lx2 = modbus_new_rtu("/dev/ttySC0", 19200, 'N', 8, 1);
  if (lx2 == NULL)
    return -1;
  int slv = modbus_set_slave(lx2, address); // Set device
  if (slv == -1) { // If invalid slave ID
    modbus_free(lx2);
    return -1;
  }
  // Connect to LX2, exit if fails
  if (modbus_connect(lx2) == -1) {
    modbus_free(lx2);
    return -1;
  }
  // Read value from register
  unsigned short val;
  int rc = modbus_read_registers(lx2, reg-40001, 1, &val);
  if (rc == -1) { // If fails to read register
    modbus_close(lx2);
    modbus_free(lx2);
    return -1;
  }
  // Close everything up and return the value from the register
  modbus_close(lx2);
  modbus_free(lx2);
  return val;
}

