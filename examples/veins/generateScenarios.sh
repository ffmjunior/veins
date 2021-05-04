#!/bin/bash
set -x
for i in $(seq 1 10); do
  cp map00.launchd.xml map00$i.launchd.xml
  cp map00.rou.xml map00$i.rou.xml
  cp map00.sumo.cfg map00$i.sumo.cfg
  sed -i "s/map00.rou.xml/map00$i.rou.xml/g" map00$i.launchd.xml
  sed -i "s/map00.sumo.cfg/map00$i.sumo.cfg/g" map00$i.launchd.xml
  sed -i "s/number='0'/number='${i}'/g" map00$i.rou.xml
  sed -i "s/map00.rou.xml/map00$i.rou.xml/g" map00$i.sumo.cfg
done

#sudo sed -i 's/set default="0"/set default="4"/g' /boot/grub/grub.cfg 