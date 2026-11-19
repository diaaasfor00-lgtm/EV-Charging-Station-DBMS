# EV Charging Station Management System (Advanced C)

A modular, multi-file database management system written in **C** to simulate and manage electric vehicle (EV) charging stations, charging ports, vehicle queues, and billing operations. *(Ranked 1st in the cohort).*

## System Architecture & Data Structures
* **Binary Search Trees (BST):** Implemented for fast $O(\log n)$ lookup, insertion, and management of charging stations (`Station`) and registered vehicles (`tCar`).
* **Linked Lists:** Dynamically manages individual charging ports (`portsList`) per station across three charging rates (`SLOW`, `MID`, `FAST`) and tracks port statuses (`Free`, `Occupied`, `Out-of-Order`).
* **FIFO Queues:** Dedicated waiting queues (`carQueue`) per station that automatically advance the first compatible vehicle when a charging port becomes available.
* **Persistent File I/O:** Reads and updates relational database records across `Stations.txt`, `Cars.txt`, `Ports.txt`, and `LineOfCars.txt`.

## Key Features
* **Automated Scheduling:** Matches vehicles from the station queue to newly freed ports based on charging rate compatibility.
* **Real-Time Duration & Load Analytics:** Tracks charging duration in minutes, releases ports exceeding time limits, and calculates station utilization and fault percentages.
* **Dynamic Memory Management:** Clean allocation and deallocation of nodes upon port removal or station closure.

## How to Build & Run
1. Open `ChargingStationDBMS.sln` in **Visual Studio** (or compile all `.c` files using `gcc *.c -o ChargingStationDBMS -lm`).
2. Ensure `Stations.txt`, `Cars.txt`, `Ports.txt`, and `LineOfCars.txt` are in the working directory.
3. Run the executable to interact with the station management CLI menu.
