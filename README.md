# AgOpenSoftAP

This component, running on ESP32 hardware, implements the WiFi soft access point for a mobile platform (truck, tractor, etc.) and is part of a
precision vehicle guidance system using the AgOpenGps solution.

[AgOpenGPS GitHub](https://github.com/farmerbriantee/AgOpenGPS)

[AgOpenGPS Web](https://discourse.agopengps.com/)

The AgOpenGPS solution utilizes a simple UDP network topology for intercommunication between subsystems within, or in close proximity to, the vehicle cab.  These subsystems include:

1. The Operator console (ordinarily a tablet PC)
2. RTK enabled real-time navigation subsystem
3. Steer solution for automatic or semi-automatic vehicle guidance
4. Section controls for implements and other peripheral devices


## CHANGE HISTORY
* 6/10/2022     Initial commit of the WiFi soft access point feature and a serial-to-UDP bridge for providing navigation info to the network. 
