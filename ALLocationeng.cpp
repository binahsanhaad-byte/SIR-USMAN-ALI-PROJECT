#include "AllocationEngine.h"
#include <iostream>
using namespace std;

AllocationEngine::AllocationEngine(Zone* zArray, int count) : 
    zones(zArray), zoneCount(count), crossZonePenalty(15.0), nonAdjacentPenalty(25.0) {}

bool AllocationEngine::allocateParking(string vehicleID, int requestedZone, 
                                       int& allocatedSlot, int& allocatedZone, 
                                       int& allocatedArea, float& penalty) {
    if (requestedZone >= 1 && requestedZone <= zoneCount) {
        if (zones[requestedZone - 1].hasAvailableSlot()) {
            ParkingSlot* slot = zones[requestedZone - 1].findAvailableSlot();
            if (slot != nullptr) {
                allocatedZone = requestedZone;
                allocatedSlot = slot->getSlotID();
                allocatedArea = slot->getAreaID();
                zones[requestedZone - 1].allocateSlot(vehicleID);
                penalty = 0.0f;
                return true;
            }
        }
    }
    if (requestedZone >= 1 && requestedZone <= zoneCount) {
        int* adjacentZones = zones[requestedZone - 1].getAdjacentZones();
        int adjCount = zones[requestedZone - 1].getAdjacentCount();
        for (int i = 0; i < adjCount; i++) {
            int adjZoneID = adjacentZones[i];
            if (zones[adjZoneID - 1].hasAvailableSlot()) {
                ParkingSlot* slot = zones[adjZoneID - 1].findAvailableSlot();
                if (slot != nullptr) {
                    allocatedZone = adjZoneID;
                    allocatedSlot = slot->getSlotID();
                    allocatedArea = slot->getAreaID();
                    zones[adjZoneID - 1].allocateSlot(vehicleID);
                    penalty = crossZonePenalty;
                    return true;
                }
            }
        }
    }
    for (int i = 0; i < zoneCount; i++) {
        if (zones[i].hasAvailableSlot()) {
            ParkingSlot* slot = zones[i].findAvailableSlot();
            if (slot != nullptr) {
                allocatedZone = i + 1;
                allocatedSlot = slot->getSlotID();
                allocatedArea = slot->getAreaID();
                zones[i].allocateSlot(vehicleID);
                penalty = nonAdjacentPenalty;
                return true;
            }
        }
    }
    return false;
}

bool AllocationEngine::releaseParking(int zoneID, int slotID, int areaID) {
    if (zoneID >= 1 && zoneID <= zoneCount) {
        return zones[zoneID - 1].releaseSlot(slotID, areaID);
    }
    return false;
}

void AllocationEngine::displayZoneStatus() {
    cout << "\n=== ZONE STATUS ===" << endl;
    for (int i = 0; i < zoneCount; i++) {
        zones[i].display();
    }
}
