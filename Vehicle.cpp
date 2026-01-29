#include "Vehicle.h"
#include <iostream>
using namespace std;

Vehicle::Vehicle() : preferredZone(-1) {}

Vehicle::Vehicle(string id, int zone) : vehicleID(id), preferredZone(zone) {}

string Vehicle::getVehicleID() const { return vehicleID; }
int Vehicle::getPreferredZone() const { return preferredZone; }

void Vehicle::display() const {
    cout << "Vehicle " << vehicleID << " (Preferred Zone: " << preferredZone << ")" << endl;
}
