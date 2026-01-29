#include "Zone.h"
#include <iostream>
using namespace std;

Zone::Zone() : areas(nullptr), adjacentZones(nullptr), areaCount(0), adjacentCount(0), totalSlots(0), availableSlots(0) {}

Zone::Zone(int zID, string zName, int numAreas, int* areaCapacities) : 
    zoneID(zID), name(zName), areaCount(numAreas), adjacentCount(0), totalSlots(0), availableSlots(0) {
    areas = new ParkingArea[areaCount];
    for (int i = 0; i < areaCount; i++) {
        areas[i] = ParkingArea(i + 1, zoneID, areaCapacities[i]);
        totalSlots += areaCapacities[i];
        availableSlots += areaCapacities[i];
    }
    adjacentZones = new int[MAX_ZONES - 1];
}

Zone::~Zone() {
    delete[] areas;
    delete[] adjacentZones;
}

int Zone::getZoneID() const { return zoneID; }
string Zone::getName() const { return name; }
int Zone::getTotalSlots() const { return totalSlots; }
int Zone::getAvailableSlots() const { return availableSlots; }

float Zone::getUtilization() const { 
    if (totalSlots == 0) return 0.0f;
    return ((totalSlots - availableSlots) * 100.0f) / totalSlots; 
}

void Zone::addAdjacentZone(int zID) {
    if (adjacentCount < MAX_ZONES - 1) {
        adjacentZones[adjacentCount++] = zID;
    }
}

int* Zone::getAdjacentZones() { return adjacentZones; }
int Zone::getAdjacentCount() { return adjacentCount; }

ParkingSlot* Zone::findAvailableSlot() {
    for (int i = 0; i < areaCount; i++) {
        ParkingSlot* slot = areas[i].findAvailableSlot();
        if (slot != nullptr) return slot;
    }
    return nullptr;
}

bool Zone::allocateSlot(string vehicleID) {
    ParkingSlot* slot = findAvailableSlot();
    if (slot != nullptr) {
        slot->allocate(vehicleID);
        availableSlots--;
        return true;
    }
    return false;
}

bool Zone::releaseSlot(int slotID, int areaID) {
    if (areaID >= 1 && areaID <= areaCount) {
        if (areas[areaID - 1].releaseSlot(slotID)) {
            availableSlots++;
            return true;
        }
    }
    return false;
}

bool Zone::hasAvailableSlot() const {
    return availableSlots > 0;
}

void Zone::display() const {
    cout << "\nZone " << zoneID << ": " << name << endl;
    cout << "  Total Slots: " << totalSlots << " | Available: " << availableSlots 
         << " | Utilization: " << getUtilization() << "%" << endl;
    if (adjacentCount > 0) {
        cout << "  Adjacent Zones: ";
        for (int i = 0; i < adjacentCount; i++) {
            cout << adjacentZones[i];
            if (i < adjacentCount - 1) cout << ", ";
        }
        cout << endl;
    }
}
