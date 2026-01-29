#include "ParkingArea.h"
#include <iostream>
using namespace std;

ParkingArea::ParkingArea() : areaID(-1), zoneID(-1), slots(nullptr), totalSlots(0), availableSlots(0) {}

ParkingArea::ParkingArea(int aID, int zID, int slotCount) : 
    areaID(aID), zoneID(zID), totalSlots(slotCount), availableSlots(slotCount) {
    slots = new ParkingSlot[totalSlots];
    for (int i = 0; i < totalSlots; i++) {
        slots[i] = ParkingSlot(i + 1, zoneID, areaID);
    }
}

ParkingArea::~ParkingArea() {
    delete[] slots;
}

int ParkingArea::getAreaID() const { return areaID; }
int ParkingArea::getZoneID() const { return zoneID; }
int ParkingArea::getAvailableSlots() const { return availableSlots; }
int ParkingArea::getTotalSlots() const { return totalSlots; }

bool ParkingArea::allocateSlot(string vehicleID) {
    for (int i = 0; i < totalSlots; i++) {
        if (slots[i].getAvailability()) {
            slots[i].allocate(vehicleID);
            availableSlots--;
            return true;
        }
    }
    return false;
}

bool ParkingArea::releaseSlot(int slotID) {
    if (slotID >= 1 && slotID <= totalSlots) {
        if (!slots[slotID - 1].getAvailability()) {
            slots[slotID - 1].release();
            availableSlots++;
            return true;
        }
    }
    return false;
}

ParkingSlot* ParkingArea::findAvailableSlot() {
    for (int i = 0; i < totalSlots; i++) {
        if (slots[i].getAvailability()) {
            return &slots[i];
        }
    }
    return nullptr;
}

void ParkingArea::display() const {
    cout << "  Area " << areaID << ": " << availableSlots << "/" << totalSlots << " slots available" << endl;
}
