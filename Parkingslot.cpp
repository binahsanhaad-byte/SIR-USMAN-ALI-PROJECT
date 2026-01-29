#include "ParkingSlot.h"
#include <iostream>
using namespace std;

ParkingSlot::ParkingSlot() : slotID(-1), zoneID(-1), areaID(-1), isAvailable(true), vehicleID("") {}

ParkingSlot::ParkingSlot(int sID, int zID, int aID) : 
    slotID(sID), zoneID(zID), areaID(aID), isAvailable(true), vehicleID("") {}

int ParkingSlot::getSlotID() const { return slotID; }
int ParkingSlot::getZoneID() const { return zoneID; }
int ParkingSlot::getAreaID() const { return areaID; }
bool ParkingSlot::getAvailability() const { return isAvailable; }
string ParkingSlot::getVehicleID() const { return vehicleID; }

void ParkingSlot::allocate(string vID) {
    if (isAvailable) {
        isAvailable = false;
        vehicleID = vID;
    }
}

void ParkingSlot::release() {
    isAvailable = true;
    vehicleID = "";
}

void ParkingSlot::display() const {
    cout << "Slot " << slotID << " [Zone " << zoneID << ", Area " << areaID << "] - ";
    if (isAvailable) cout << "Available";
    else cout << "Occupied by " << vehicleID;
    cout << endl;
}
