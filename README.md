# FOGWISE - VEINS #

This repository contains the code necessary to run simulations of the research paper "FogWise: On the limits of the coexistence of heterogeneous applications on Fog computing and Internet of Vehicles." 

Using it, you can simulate the operation of vehicles as Fog Nodes in IoV (Internet of Vehicles) manipulating a set of parameters:

- map, vehicle density, beacon interval, request type deviations, auction method, client-server ratio, and request sizes.

You can also collect and analyze a set of metrics:

- averageRtt, responseRequestRatio, receivedWSMs, requestsAccepted, requestsDiscarded, requestsFromOthers, requestsGenerated, requestsOverhead, requestsReceived, responsesReceived, responsesSent, tasksAccepted, queueDuration, queueSize, tasksCancelled, tasksFinished,  notificationsReceived, notificationsSent, interactions, generatedWSMs, processingCapacity, RXTXLostPackets, countCars, SNIRLostPackets, SentPackets, SlotsBackoff, busyTime, maxSpeed, minSpeed, ncollisions, totalCO2Emission, totalDistance, totalTime

This work is completely based on VEINS 4.7, initially available at https://veins.car2x.org/. It also uses SUMO 0.30 https://sumo.dlr.de/docs/index.html. Everything runs over Omnetpp 5.6.1 https://omnetpp.org/. Thus, we recommend previous knowledge about these tools to run simulations and analyze the results.

### Requirements

- Omnetpp 5.6.1
- SUMO 0.30

### Setup ###

* Open Omnetpp 5.6.1
* Import this repository into Omnetpp on the menu: File - Import - General - Projects from Folder or Archive -  Next - Directory...

### Running

Use terminal, shell, prompt to reach the directory, and run $ ./sumo-launchd.py -v. If all dependencies are correctly installed, the prompt should display the message "Listening on port 9999"

Click on play and watch the simulation running. 

### Important information and tips

Some important files:

- fogwise-veins\examples\veins\omnetpp.ini - contains main parameters to control evaluation
- fogwise-veins\src\veins\modules\application\traci\FogWiseFull.h & FogWiseFull.cc - main source codes for Fog Nodes and Clients. You can find auxiliary classes and headers within those files
- Notice that there are different ways to run that on windows or Linux. Thus, check the documentation on VEINS, Omnetpp, and SUMO for more information.

### Contribution guidelines ###

You are welcome to fork this project and send contributions. 

If you use this simulation, please cite the paper:

de Mendonça Junior, FF, Lopes   Dias, K,  d’Orey,  P.   M., and Kokkinogenis, Z,  “Fogwise:  On  the  limits  of  the  coexistence  ofheterogeneous applications on fog computing and internet of vehicles,”Transactions on Emerging Telecommunications Technologies,   vol.n/a, no. n/a, p. e4145. [Online]. Available: https://onlinelibrary.wiley.com/doi/abs/10.1002/ett.4145

### Contact info ###

Contact in case of any doubts on ffmj@cin.ufpe.br

To know more about me and my research visit https://cin.ufpe.br/~ffmj/