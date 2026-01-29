#include <iostream>
#include <string>
#include <ctime>
using namespace std;

enum RequestState { REQUESTED, ALLOCATED, OCCUPIED, RELEASED, CANCELLED };
const int MAX_ZONES = 4;
const int MAX_VEHICLES = 50;

class TimeStamp {
private:
    int hours;
    int minutes;
    int seconds;
    
public:
    TimeStamp() {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        hours = ltm->tm_hour;
        minutes = ltm->tm_min;
        seconds = ltm->tm_sec;
    }
    TimeStamp(int h, int m, int s) : hours(h), minutes(m), seconds(s) {}
    int getTotalSeconds() const {
        return hours * 3600 + minutes * 60 + seconds;
    }
    float getHoursDifference(const TimeStamp& other) const {
        int diff = abs(getTotalSeconds() - other.getTotalSeconds());
        return diff / 3600.0f;
    }
    void display() const {
        if (hours < 10) cout << "0";
        cout << hours << ":";
        if (minutes < 10) cout << "0";
        cout << minutes << ":";
        if (seconds < 10) cout << "0";
        cout << seconds;
    }
};

class ParkingSlot {
private:
    int slotID;
    int zoneID;
    int areaID;
    bool isAvailable;
    string vehicleID;
    
public:
    ParkingSlot() : slotID(-1), zoneID(-1), areaID(-1), isAvailable(true), vehicleID("") {}
    ParkingSlot(int sID, int zID, int aID) : 
        slotID(sID), zoneID(zID), areaID(aID), isAvailable(true), vehicleID("") {}
    int getSlotID() const { return slotID; }
    int getZoneID() const { return zoneID; }
    int getAreaID() const { return areaID; }
    bool getAvailability() const { return isAvailable; }
    string getVehicleID() const { return vehicleID; }
    void allocate(string vID) {
        if (isAvailable) {
            isAvailable = false;
            vehicleID = vID;
        }
    }
    void release() {
        isAvailable = true;
        vehicleID = "";
    }
    void display() const {
        cout << "Slot " << slotID << " [Zone " << zoneID << ", Area " << areaID << "] - ";
        if (isAvailable) cout << "Available";
        else cout << "Occupied by " << vehicleID;
        cout << endl;
    }
};

class ParkingArea {
private:
    int areaID;
    int zoneID;
    ParkingSlot* slots;
    int totalSlots;
    int availableSlots;
    
public:
    ParkingArea() : areaID(-1), zoneID(-1), slots(nullptr), totalSlots(0), availableSlots(0) {}
    ParkingArea(int aID, int zID, int slotCount) : 
        areaID(aID), zoneID(zID), totalSlots(slotCount), availableSlots(slotCount) {
        slots = new ParkingSlot[totalSlots];
        for (int i = 0; i < totalSlots; i++) {
            slots[i] = ParkingSlot(i + 1, zoneID, areaID);
        }
    }
    ~ParkingArea() { delete[] slots; }
    int getAreaID() const { return areaID; }
    int getZoneID() const { return zoneID; }
    int getAvailableSlots() const { return availableSlots; }
    int getTotalSlots() const { return totalSlots; }
    bool allocateSlot(string vehicleID) {
        for (int i = 0; i < totalSlots; i++) {
            if (slots[i].getAvailability()) {
                slots[i].allocate(vehicleID);
                availableSlots--;
                return true;
            }
        }
        return false;
    }
    bool releaseSlot(int slotID) {
        if (slotID >= 1 && slotID <= totalSlots) {
            if (!slots[slotID - 1].getAvailability()) {
                slots[slotID - 1].release();
                availableSlots++;
                return true;
            }
        }
        return false;
    }
    ParkingSlot* findAvailableSlot() {
        for (int i = 0; i < totalSlots; i++) {
            if (slots[i].getAvailability()) {
                return &slots[i];
            }
        }
        return nullptr;
    }
    void display() const {
        cout << "  Area " << areaID << ": " << availableSlots << "/" << totalSlots << " slots available" << endl;
    }
};

class Vehicle {
private:
    string vehicleID;
    int preferredZone;
    
public:
    Vehicle() : preferredZone(-1) {}
    Vehicle(string id, int zone) : vehicleID(id), preferredZone(zone) {}
    string getVehicleID() const { return vehicleID; }
    int getPreferredZone() const { return preferredZone; }
    void display() const {
        cout << "Vehicle " << vehicleID << " (Preferred Zone: " << preferredZone << ")" << endl;
    }
};

class Zone {
private:
    int zoneID;
    string name;
    ParkingArea* areas;
    int areaCount;
    int* adjacentZones;
    int adjacentCount;
    int totalSlots;
    int availableSlots;
    
public:
    Zone() : areas(nullptr), adjacentZones(nullptr), areaCount(0), adjacentCount(0), totalSlots(0), availableSlots(0) {}
    Zone(int zID, string zName, int numAreas, int* areaCapacities) : 
        zoneID(zID), name(zName), areaCount(numAreas), adjacentCount(0), totalSlots(0), availableSlots(0) {
        areas = new ParkingArea[areaCount];
        for (int i = 0; i < areaCount; i++) {
            areas[i] = ParkingArea(i + 1, zoneID, areaCapacities[i]);
            totalSlots += areaCapacities[i];
            availableSlots += areaCapacities[i];
        }
        adjacentZones = new int[MAX_ZONES - 1];
    }
    ~Zone() {
        delete[] areas;
        delete[] adjacentZones;
    }
    int getZoneID() const { return zoneID; }
    string getName() const { return name; }
    int getTotalSlots() const { return totalSlots; }
    int getAvailableSlots() const { return availableSlots; }
    float getUtilization() const { 
        if (totalSlots == 0) return 0.0f;
        return ((totalSlots - availableSlots) * 100.0f) / totalSlots; 
    }
    void addAdjacentZone(int zoneID) {
        if (adjacentCount < MAX_ZONES - 1) {
            adjacentZones[adjacentCount++] = zoneID;
        }
    }
    int* getAdjacentZones() { return adjacentZones; }
    int getAdjacentCount() { return adjacentCount; }
    ParkingSlot* findAvailableSlot() {
        for (int i = 0; i < areaCount; i++) {
            ParkingSlot* slot = areas[i].findAvailableSlot();
            if (slot != nullptr) {
                return slot;
            }
        }
        return nullptr;
    }
    bool allocateSlot(string vehicleID) {
        ParkingSlot* slot = findAvailableSlot();
        if (slot != nullptr) {
            slot->allocate(vehicleID);
            availableSlots--;
            return true;
        }
        return false;
    }
    bool releaseSlot(int slotID, int areaID) {
        if (areaID >= 1 && areaID <= areaCount) {
            if (areas[areaID - 1].releaseSlot(slotID)) {
                availableSlots++;
                return true;
            }
        }
        return false;
    }
    bool hasAvailableSlot() const {
        return availableSlots > 0;
    }
    void display() const {
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
};

class RequestNode {
public:
    string vehicleID;
    int requestedZone;
    int allocatedSlot;
    int allocatedZone;
    int allocatedArea;
    RequestState state;
    float penalty;
    TimeStamp requestTime;
    TimeStamp allocationTime;
    TimeStamp completionTime;
    RequestNode* next;
    RequestNode(string vID, int zone) : 
        vehicleID(vID), requestedZone(zone), allocatedSlot(-1), 
        allocatedZone(-1), allocatedArea(-1), state(REQUESTED), 
        penalty(0), next(nullptr), requestTime(TimeStamp()) {}
    bool isValidTransition(RequestState newState) {
        switch(state) {
            case REQUESTED:
                return (newState == ALLOCATED || newState == CANCELLED);
            case ALLOCATED:
                return (newState == OCCUPIED || newState == CANCELLED);
            case OCCUPIED:
                return (newState == RELEASED);
            case RELEASED:
            case CANCELLED:
                return false;
        }
        return false;
    }
    bool changeState(RequestState newState) {
        if (isValidTransition(newState)) {
            if (newState == ALLOCATED) {
                allocationTime = TimeStamp();
            } else if (newState == RELEASED || newState == CANCELLED) {
                completionTime = TimeStamp();
            }
            state = newState;
            return true;
        }
        return false;
    }
    float getParkingDuration() const {
        if (state == RELEASED) {
            return requestTime.getHoursDifference(completionTime);
        }
        return 0.0f;
    }
    void display() const {
        cout << "Request - Vehicle: " << vehicleID << endl;
        cout << "  Requested Zone: " << requestedZone << endl;
        cout << "  State: ";
        switch(state) {
            case REQUESTED: cout << "REQUESTED"; break;
            case ALLOCATED: cout << "ALLOCATED"; break;
            case OCCUPIED: cout << "OCCUPIED"; break;
            case RELEASED: cout << "RELEASED"; break;
            case CANCELLED: cout << "CANCELLED"; break;
        }
        cout << " | Request Time: ";
        requestTime.display();
        if (allocatedSlot != -1) {
            cout << "\n  Allocated: Slot " << allocatedSlot << " in Zone " << allocatedZone 
                 << ", Area " << allocatedArea;
            if (penalty > 0) cout << " (Penalty: $" << penalty << ")";
            cout << "\n  Allocation Time: ";
            allocationTime.display();
        }
        if (state == RELEASED || state == CANCELLED) {
            cout << "\n  Completion Time: ";
            completionTime.display();
            if (state == RELEASED) {
                cout << " | Duration: " << getParkingDuration() << " hours";
            }
        }
        cout << endl;
    }
};

class RequestHistory {
private:
    RequestNode* head;
    int count;
public:
    RequestHistory() : head(nullptr), count(0) {}
    ~RequestHistory() {
        RequestNode* current = head;
        while (current != nullptr) {
            RequestNode* next = current->next;
            delete current;
            current = next;
        }
    }
    void addRequest(RequestNode* newNode) {
        newNode->next = head;
        head = newNode;
        count++;
    }
    RequestNode* findRequest(string vehicleID) {
        RequestNode* current = head;
        while (current != nullptr) {
            if (current->vehicleID == vehicleID) return current;
            current = current->next;
        }
        return nullptr;
    }
    void displayAll() {
        cout << "\n=== REQUEST HISTORY (" << count << " requests) ===" << endl;
        if (count == 0) {
            cout << "No requests yet." << endl;
            return;
        }
        RequestNode* current = head;
        while (current != nullptr) {
            current->display();
            cout << "--------------------------------" << endl;
            current = current->next;
        }
    }
    RequestNode* getHead() { return head; }
    int getCount() { return count; }
};  // <--- Fixed: MISSING SEMICOLON HERE

class RollbackStack {
private:
    struct StackNode {
        string vehicleID;
        int slotID;
        int zoneID;
        int areaID;
        RequestState oldState;
        RequestState newState;
        StackNode* next;
    };
    StackNode* top;
    int operationCount;
public:
    RollbackStack() : top(nullptr), operationCount(0) {}
    ~RollbackStack() {
        while (top != nullptr) {
            StackNode* temp = top;
            top = top->next;
            delete temp;
        }
    }
    void push(string vID, int sID, int zID, int aID, RequestState oldS, RequestState newS) {
        StackNode* newNode = new StackNode();
        newNode->vehicleID = vID;
        newNode->slotID = sID;
        newNode->zoneID = zID;
        newNode->areaID = aID;
        newNode->oldState = oldS;
        newNode->newState = newS;
        newNode->next = top;
        top = newNode;
        operationCount++;
    }
    bool pop() {
        if (top == nullptr) return false;
        StackNode* temp = top;
        top = top->next;
        cout << "Rollback Operation: Vehicle " << temp->vehicleID << endl;
        cout << "  Slot " << temp->slotID << " in Zone " << temp->zoneID << ", Area " << temp->areaID << endl;
        cout << "  State reverted: ";
        displayState(temp->newState);
        cout << " -> ";
        displayState(temp->oldState);
        cout << endl;
        delete temp;
        operationCount--;
        return true;
    }
    void rollbackLastK(int k) {
        cout << "\n=== ROLLING BACK LAST " << k << " OPERATIONS ===" << endl;
        int rolledBack = 0;
        for (int i = 0; i < k && top != nullptr; i++) {
            if (pop()) {
                rolledBack++;
            }
        }
        cout << "Successfully rolled back " << rolledBack << " operations." << endl;
    }
    void displayState(RequestState state) {
        switch(state) {
            case REQUESTED: cout << "REQUESTED"; break;
            case ALLOCATED: cout << "ALLOCATED"; break;
            case OCCUPIED: cout << "OCCUPIED"; break;
            case RELEASED: cout << "RELEASED"; break;
            case CANCELLED: cout << "CANCELLED"; break;
        }
    }
    bool isEmpty() { return top == nullptr; }
    int getOperationCount() { return operationCount; }
};

class WaitingQueue {
private:
    struct QueueNode {
        string vehicleID;
        int requestedZone;
        TimeStamp queueTime;
        QueueNode* next;
        QueueNode(string vID, int zone) : vehicleID(vID), requestedZone(zone), 
                                         queueTime(TimeStamp()), next(nullptr) {}
    };
    QueueNode* front;
    QueueNode* rear;
    int queueSize;
public:
    WaitingQueue() : front(nullptr), rear(nullptr), queueSize(0) {}
    ~WaitingQueue() {
        while (front != nullptr) {
            QueueNode* temp = front;
            front = front->next;
            delete temp;
        }
    }
    void enqueue(string vehicleID, int zone) {
        QueueNode* newNode = new QueueNode(vehicleID, zone);
        if (rear == nullptr) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        queueSize++;
    }
    QueueNode* dequeue() {
        if (front == nullptr) return nullptr;
        QueueNode* temp = front;
        front = front->next;
        if (front == nullptr) {
            rear = nullptr;
        }
        queueSize--;
        return temp;
    }
    void display() const {
        cout << "\n=== WAITING QUEUE (" << queueSize << " vehicles) ===" << endl;
        if (queueSize == 0) {
            cout << "Queue is empty." << endl;
            return;
        }
        QueueNode* current = front;
        int position = 1;
        while (current != nullptr) {
            cout << position++ << ". Vehicle: " << current->vehicleID 
                 << " (Requested Zone: " << current->requestedZone << ")" << endl;
            cout << "   Queue Time: ";
            current->queueTime.display();
            cout << endl;
            current = current->next;
        }
    }
    bool isEmpty() const { return queueSize == 0; }
    int getSize() const { return queueSize; }
};

class AllocationEngine {
private:
    Zone* zones;
    int zoneCount;
    float crossZonePenalty;
    float nonAdjacentPenalty;
public:
    AllocationEngine(Zone* zArray, int count) : 
        zones(zArray), zoneCount(count), crossZonePenalty(15.0), nonAdjacentPenalty(25.0) {}
    bool allocateParking(string vehicleID, int requestedZone, 
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
    bool releaseParking(int zoneID, int slotID, int areaID) {
        if (zoneID >= 1 && zoneID <= zoneCount) {
            return zones[zoneID - 1].releaseSlot(slotID, areaID);
        }
        return false;
    }
    void displayZoneStatus() {
        cout << "\n=== ZONE STATUS ===" << endl;
        for (int i = 0; i < zoneCount; i++) {
            zones[i].display();
        }
    }
};

class ParkingSystem {
private:
    Zone* zones;
    int zoneCount;
    Vehicle* vehicles;
    int vehicleCount;
    RequestHistory requestHistory;
    RollbackStack rollbackStack;
    WaitingQueue waitingQueue;
    AllocationEngine* allocator;
    int completedRequests;
    int cancelledRequests;
    int* zoneUsageCount;
public:
    ParkingSystem() : zoneCount(MAX_ZONES), vehicleCount(0), 
                     completedRequests(0), cancelledRequests(0) {
        zones = new Zone[zoneCount];
        int downtownAreas[] = {3, 2};
        int commercialAreas[] = {4, 3, 2};
        int residentialAreas[] = {5, 4, 3, 2};
        int industrialAreas[] = {3, 3};
        zones[0] = Zone(1, "Downtown", 2, downtownAreas);
        zones[1] = Zone(2, "Commercial", 3, commercialAreas);
        zones[2] = Zone(3, "Residential", 4, residentialAreas);
        zones[3] = Zone(4, "Industrial", 2, industrialAreas);
        zones[0].addAdjacentZone(2);
        zones[0].addAdjacentZone(3);
        zones[1].addAdjacentZone(1);
        zones[1].addAdjacentZone(3);
        zones[2].addAdjacentZone(1);
        zones[2].addAdjacentZone(2);
        zones[2].addAdjacentZone(4);
        zones[3].addAdjacentZone(3);
        vehicles = new Vehicle[MAX_VEHICLES];
        zoneUsageCount = new int[zoneCount];
        for (int i = 0; i < zoneCount; i++) { zoneUsageCount[i] = 0; }
        allocator = new AllocationEngine(zones, zoneCount);
    }
    ~ParkingSystem() {
        delete[] zones;
        delete[] vehicles;
        delete[] zoneUsageCount;
        delete allocator;
    }
    void registerVehicle(string vehicleID, int preferredZone) {
        if (vehicleCount < MAX_VEHICLES) {
            vehicles[vehicleCount++] = Vehicle(vehicleID, preferredZone);
            cout << "Vehicle " << vehicleID << " registered successfully (Preferred Zone: " 
                 << preferredZone << ")" << endl;
        } else {
            cout << "Maximum vehicle limit reached!" << endl;
        }
    }
    Vehicle* findVehicle(string id) {
        for (int i = 0; i < vehicleCount; i++) {
            if (vehicles[i].getVehicleID() == id) {
                return &vehicles[i];
            }
        }
        return nullptr;
    }
    bool requestParking(string vehicleID, int requestedZone) {
        Vehicle* vehicle = findVehicle(vehicleID);
        if (vehicle == nullptr) {
            cout << "Vehicle not registered! Please register first." << endl;
            return false;
        }
        RequestNode* existingRequest = requestHistory.findRequest(vehicleID);
        if (existingRequest != nullptr && 
            (existingRequest->state == ALLOCATED || existingRequest->state == OCCUPIED)) {
            cout << "Vehicle already has active parking!" << endl;
            existingRequest->display();
            return false;
        }
        int allocatedSlot = -1, allocatedZone = -1, allocatedArea = -1;
        float penalty = 0.0f;
        if (allocator->allocateParking(vehicleID, requestedZone, 
                                       allocatedSlot, allocatedZone, 
                                       allocatedArea, penalty)) {
            RequestNode* newRequest = new RequestNode(vehicleID, requestedZone);
            newRequest->allocatedZone = allocatedZone;
            newRequest->allocatedSlot = allocatedSlot;
            newRequest->allocatedArea = allocatedArea;
            newRequest->penalty = penalty;
            if (newRequest->changeState(ALLOCATED)) {
                requestHistory.addRequest(newRequest);
                rollbackStack.push(vehicleID, allocatedSlot, allocatedZone, 
                                  allocatedArea, REQUESTED, ALLOCATED);
                zoneUsageCount[allocatedZone - 1]++;
                cout << "\n=== PARKING ALLOCATED SUCCESSFULLY ===" << endl;
                cout << "Vehicle: " << vehicleID << endl;
                cout << "Location: Slot " << allocatedSlot << " in Zone " << allocatedZone 
                     << ", Area " << allocatedArea << endl;
                cout << "Allocation Time: ";
                newRequest->allocationTime.display();
                if (penalty > 0) {
                    cout << "\nCross-zone penalty applied: $" << penalty;
                    if (allocatedZone != requestedZone && 
                        allocatedZone != requestedZone + 1 && 
                        allocatedZone != requestedZone - 1) {
                        cout << " (non-adjacent zone)";
                    }
                }
                cout << "\n======================================" << endl;
                return true;
            }
        } else {
            waitingQueue.enqueue(vehicleID, requestedZone);
            cout << "\nNo parking slots available! Vehicle added to waiting queue." << endl;
            waitingQueue.display();
            return false;
        }
        return false;
    }
    bool changeRequestState(string vehicleID, RequestState newState) {
        RequestNode* request = requestHistory.findRequest(vehicleID);
        if (request == nullptr) {
            cout << "No active request found for vehicle " << vehicleID << endl;
            return false;
        }
        RequestState oldState = request->state;
        if (request->isValidTransition(newState)) {
            if (request->changeState(newState)) {
                rollbackStack.push(vehicleID, request->allocatedSlot, request->allocatedZone, 
                                 request->allocatedArea, oldState, newState);
                if (newState == CANCELLED || newState == RELEASED) {
                    if (request->allocatedZone >= 1 && request->allocatedZone <= zoneCount) {
                        allocator->releaseParking(request->allocatedZone, 
                                                 request->allocatedSlot, 
                                                 request->allocatedArea);
                    }
                    if (newState == RELEASED) {
                        completedRequests++;
                    } else if (newState == CANCELLED) {
                        cancelledRequests++;
                    }
                }
                cout << "State change successful for vehicle " << vehicleID << endl;
                cout << "  State changed from ";
                displayState(oldState);
                cout << " to ";
                displayState(newState);
                cout << endl;
                return true;
            }
        } else {
            cout << "Invalid state transition for vehicle " << vehicleID << endl;
            cout << "  Current state: ";
            displayState(oldState);
            cout << " | Requested state: ";
            displayState(newState);
            cout << endl;
            cout << "  Valid transitions from " << getStateName(oldState) << " are: ";
            displayValidTransitions(oldState);
        }
        return false;
    }
    void displayState(RequestState state) {
        switch(state) {
            case REQUESTED: cout << "REQUESTED"; break;
            case ALLOCATED: cout << "ALLOCATED"; break;
            case OCCUPIED: cout << "OCCUPIED"; break;
            case RELEASED: cout << "RELEASED"; break;
            case CANCELLED: cout << "CANCELLED"; break;
        }
    }
    string getStateName(RequestState state) {
        switch(state) {
            case REQUESTED: return "REQUESTED";
            case ALLOCATED: return "ALLOCATED";
            case OCCUPIED: return "OCCUPIED";
            case RELEASED: return "RELEASED";
            case CANCELLED: return "CANCELLED";
        }
        return "UNKNOWN";
    }
    void displayValidTransitions(RequestState state) {
        switch(state) {
            case REQUESTED: cout << "ALLOCATED or CANCELLED"; break;
            case ALLOCATED: cout << "OCCUPIED or CANCELLED"; break;
            case OCCUPIED: cout << "RELEASED"; break;
            case RELEASED: cout << "No valid transitions"; break;
            case CANCELLED: cout << "No valid transitions"; break;
        }
        cout << endl;
    }
    void cancelRequest(string vehicleID) {
        if (changeRequestState(vehicleID, CANCELLED)) {
            cout << "Request cancelled successfully!" << endl;
        }
    }
    void releaseParking(string vehicleID) {
        if (changeRequestState(vehicleID, RELEASED)) {
            cout << "Parking released successfully!" << endl;
        }
    }
    void rollbackOperations(int k) {
        rollbackStack.rollbackLastK(k);
    }
    void displayAnalytics() {
        cout << "\n========== COMPREHENSIVE ANALYTICS ==========" << endl;
        cout << "\n1. ZONE UTILIZATION:" << endl;
        allocator->displayZoneStatus();
        int totalRequests = requestHistory.getCount();
        float avgDuration = 0.0f;
        int completedCount = 0;
        RequestNode* current = requestHistory.getHead();
        while (current != nullptr) {
            if (current->state == RELEASED) {
                avgDuration += current->getParkingDuration();
                completedCount++;
            }
            current = current->next;
        }
        if (completedCount > 0) {
            avgDuration /= completedCount;
        }
        cout << "\n2. REQUEST STATISTICS:" << endl;
        cout << "   Total Requests: " << totalRequests << endl;
        cout << "   Completed Requests: " << completedRequests << endl;
        cout << "   Cancelled Requests: " << cancelledRequests << endl;
        cout << "   Average Parking Duration: " << avgDuration << " hours" << endl;
        cout << "\n3. ZONE USAGE ANALYSIS:" << endl;
        int maxUsage = 0;
        int peakZone = -1;
        for (int i = 0; i < zoneCount; i++) {
            cout << "   Zone " << (i + 1) << " (" << zones[i].getName() << "): " 
                 << zoneUsageCount[i] << " allocations" << endl;
            if (zoneUsageCount[i] > maxUsage) {
                maxUsage = zoneUsageCount[i];
                peakZone = i + 1;
            }
        }
        if (peakZone != -1) {
            cout << "   Peak Usage Zone: Zone " << peakZone << " (" 
                 << zones[peakZone-1].getName() << ") with " << maxUsage << " allocations" << endl;
        }
        cout << "\n4. CURRENT SYSTEM STATUS:" << endl;
        cout << "   Registered Vehicles: " << vehicleCount << endl;
        cout << "   Vehicles in Waiting Queue: " << waitingQueue.getSize() << endl;
        cout << "   Pending Rollback Operations: " << rollbackStack.getOperationCount() << endl;
        waitingQueue.display();
    }
    void displaySystemStatus() {
        cout << "\n========== SYSTEM OVERVIEW ==========" << endl;
        cout << "Total Zones: " << zoneCount << endl;
        cout << "Registered Vehicles: " << vehicleCount << endl;
        cout << "Total Parking Requests: " << requestHistory.getCount() << endl;
        cout << "Completed Requests: " << completedRequests << endl;
        cout << "Cancelled Requests: " << cancelledRequests << endl;
        allocator->displayZoneStatus();
    }
    void displayAllRequests() {
        requestHistory.displayAll();
    }
    void runDemo() {
        cout << "\n=== DEMONSTRATING SMART PARKING SYSTEM ===" << endl;
        cout << "\n1. Registering vehicles..." << endl;
        registerVehicle("CAR001", 1);
        registerVehicle("CAR002", 2);
        registerVehicle("CAR003", 1);
        registerVehicle("CAR004", 3);
        registerVehicle("CAR005", 4);
        cout << "\n2. Requesting parking..." << endl;
        requestParking("CAR001", 1);
        requestParking("CAR002", 2);
        requestParking("CAR003", 1);
        requestParking("CAR004", 1);
        cout << "\n3. State transitions..." << endl;
        changeRequestState("CAR001", OCCUPIED);
        changeRequestState("CAR002", OCCUPIED);
        cout << "\n4. Testing invalid transition..." << endl;
        changeRequestState("CAR003", RELEASED);
        cout << "\n5. Cancelling a request..." << endl;
        cancelRequest("CAR003");
        cout << "\n6. Releasing parking..." << endl;
        releaseParking("CAR001");
        cout << "\n7. Testing rollback..." << endl;
        rollbackOperations(1);
        cout << "\n8. Testing waiting queue..." << endl;
        for (int i = 6; i <= 10; i++) {
            string carID = "CAR" + string("00") + to_string(i);
            registerVehicle(carID, 1);
            requestParking(carID, 1);
        }
        cout << "\n9. Displaying analytics..." << endl;
        displayAnalytics();
        cout << "\n=== DEMONSTRATION COMPLETE ===" << endl;
    }
};

int main() {
    ParkingSystem system;
    system.runDemo();
    int choice;
    string vehicleID;
    int zone;
    do {
        cout << "\n========== MAIN MENU ==========" << endl;
        cout << "1. Display System Status" << endl;
        cout << "2. Register New Vehicle" << endl;
        cout << "3. Request Parking" << endl;
        cout << "4. Mark as Occupied" << endl;
        cout << "5. Cancel Parking Request" << endl;
        cout << "6. Release Parking" << endl;
        cout << "7. Rollback Last K Operations" << endl;
        cout << "8. Display Comprehensive Analytics" << endl;
        cout << "9. Display All Requests" << endl;
        cout << "10. Run Demonstration" << endl;
        cout << "0. Exit System" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        switch(choice) {
            case 1:
                system.displaySystemStatus();
                break;
            case 2:
                cout << "Enter vehicle ID: ";
                cin >> vehicleID;
                cout << "Enter preferred zone (1-4): ";
                cin >> zone;
                if (zone < 1 || zone > 4) {
                    cout << "Invalid zone! Please enter 1-4." << endl;
                } else {
                    system.registerVehicle(vehicleID, zone);
                }
                break;
            case 3:
                cout << "Enter vehicle ID: ";
                cin >> vehicleID;
                cout << "Enter requested zone (1-4): ";
                cin >> zone;
                if (zone < 1 || zone > 4) {
                    cout << "Invalid zone! Please enter 1-4." << endl;
                } else {
                    system.requestParking(vehicleID, zone);
                }
                break;
            case 4:
                cout << "Enter vehicle ID to mark as occupied: ";
                cin >> vehicleID;
                system.changeRequestState(vehicleID, OCCUPIED);
                break;
            case 5:
                cout << "Enter vehicle ID to cancel: ";
                cin >> vehicleID;
                system.cancelRequest(vehicleID);
                break;
            case 6:
                cout << "Enter vehicle ID to release: ";
                cin >> vehicleID;
                system.releaseParking(vehicleID);
                break;
            case 7:
                int k;
                cout << "Enter number of operations to rollback: ";
                cin >> k;
                if (k > 0) {
                    system.rollbackOperations(k);
                } else {
                    cout << "Please enter a positive number." << endl;
                }
                break;
            case 8:
                system.displayAnalytics();
                break;
            case 9:
                system.displayAllRequests();
                break;
            case 10:
                system.runDemo();
                break;
            case 0:
                cout << "\nThank you for using Smart Parking System!" << endl;
                cout << "System shutting down..." << endl;
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 0);
    return 0;
}