Smart Parking System Design
1. Zone and Slot Representation

Zones:

The system has MAX_ZONES = 4 zones, each represented by a Zone class.

A zone has:

zoneID – unique identifier.

name – e.g., "Downtown", "Commercial".

areas – array of ParkingArea objects.

adjacentZones – list of adjacent zones for cross-zone allocation.

totalSlots & availableSlots – track usage and utilization.

Parking Areas and Slots:

Each Zone has one or more ParkingArea objects.

Each ParkingArea contains multiple ParkingSlot objects.

ParkingSlot stores:

slotID – unique in the area.

zoneID and areaID.

isAvailable – whether it’s free.

vehicleID – vehicle currently occupying the slot.

2. Allocation Strategy

The system uses a hierarchical allocation algorithm with penalties:

Preferred Zone Allocation

Check the requested zone for available slots.

If available, allocate slot → penalty = $0.

Adjacent Zone Allocation

If preferred zone full, check adjacent zones in order.

If allocated in an adjacent zone → penalty = $15.

Non-Adjacent Zone Allocation

If adjacent zones full, search all zones.

Allocation in a non-adjacent zone → penalty = $25.

Key Methods:

Zone::findAvailableSlot() – finds first free slot in the zone.

AllocationEngine::allocateParking() – manages allocation & calculates penalties.

3. Request Lifecycle State Machine

Each parking request is represented by a RequestNode, which tracks the lifecycle using RequestState:

Current State	Allowed Next States	Description
REQUESTED	ALLOCATED, CANCELLED	Request submitted; can be allocated or cancelled.
ALLOCATED	OCCUPIED, CANCELLED	Slot allocated; vehicle not yet parked.
OCCUPIED	RELEASED	Vehicle has parked; next step is release.
RELEASED	None	Parking session completed.
CANCELLED	None	Request cancelled.

Transition Implementation:

RequestNode::isValidTransition() – checks if a state change is valid.

RequestNode::changeState() – updates state & timestamps.

4. Rollback Design

RollbackStack implements a LIFO stack to revert recent operations:

Each StackNode stores:

vehicleID, slotID, zoneID, areaID

oldState, newState

Operations pushed after every state change.

rollbackLastK(int k):

Pops last k operations.

Prints reverted state.

Decrements operation counter.

Usage:

Allows administrators to undo incorrect allocations or state changes.

Works independently of allocation engine and waiting queue.

5. Time and Space Complexity
Time Complexity
Operation	Worst-Case Complexity
Vehicle Registration	O(1) (array insert)
Find Vehicle	O(V) (linear search, V = #vehicles)
Allocate Slot in Zone	O(A × S) (check all areas and slots; A = areas, S = slots per area)
Allocate Slot in System	O(Z × A × S) (Z = zones; worst-case cross-zone search)
Release Slot	O(1) per area (index lookup)
Request Lookup	O(R) (linked list of R requests)
Rollback	O(K) (pop K operations)
Display Waiting Queue	O(Q) (linear traversal)
Space Complexity
Component	Space Complexity
Vehicles	O(V)
Zones + Areas + Slots	O(Z × A × S)
RequestHistory	O(R)
RollbackStack	O(R) (max one entry per request)
WaitingQueue	O(Q)

Total space is dominated by Zones × Areas × Slots and RequestHistory.

Summary:

Zones → hierarchical allocation, adjacency awareness.

Slots → track availability and vehicle allocation.

Requests → state machine with valid transitions.

Rollback → stack-based undo for recent state changes.

Analytics and waiting queue maintain performance metrics and overflow handling.