#!/bin/bash

lastSwitch=-1

handle() {
  case $1 in
    activewindowv2\>\>*) 
    
    address=$(echo $1 | cut -d ">" -f 3)
    if [[ -z "${address//[[:space:]]/}" && $lastSwitch != 1 ]]; then
    lastSwitch=1
         WayVes -c linear_bars -w 390 -h 250 -r 10 -t 188 -z 1&&echo -e "isLarge=1" > /tmp/WayVes/linear_bars&
  
      WayVes -c linear_waves -u 0 -d 1 -r 79 -w 120 -h 50 &&  echo -e "isSmall=1" > /tmp/WayVes/linear_waves&
  
     WayVes -c linear_holder -u 0 -d 1 -r 20 -w 290 -h 200&&   echo -e "visualiserMode = 2\nparticleRadius = 32" > /tmp/WayVes/linear_holder&
      WayVes -c linear_boxes -t 511 -l 322 -w 290 -h 211 -z 1&&   echo -e "isLarge=1" > /tmp/WayVes/linear_boxes&

     WayVes -c merged_bars -v1 -z 1&
     WayVes -c linear_lights -v1 -z 1&
     WayVes -c linear_chain -v1 -z 1&

 
     
    elif [[ $lastSwitch != 0 && $(hyprctl activewindow) != *"Invalid"* ]]; then
    lastSwitch=0
         WayVes -c linear_bars -w 190 -h 35 -r 220 -t 15 -z 2&&echo -e "isLarge=0" > /tmp/WayVes/linear_bars&
   
     WayVes -c linear_waves -w 900 -r -1 -d 0 -u 1 -t -43 && echo -e "isSmall=0" > /tmp/WayVes/linear_waves&
     WayVes -c linear_holder -w 120 -h 100 -r 1020 -d 0 -u 1 -t -65&&  echo -e "visualiserMode = 0\nparticleRadius = 12" > /tmp/WayVes/linear_holder &
    WayVes -c linear_boxes -t -35 -l 720 -w 90 -h 90 -z 2 &&  echo -e "isLarge=0" > /tmp/WayVes/linear_boxes&

      WayVes -c merged_bars -v0 -z 3&
    WayVes -c linear_chain -v0 -z 3&
     WayVes -c linear_lights -v0 -z 3&
   fi
    ;;
  
  esac
}


(socat -U - UNIX-CONNECT:$XDG_RUNTIME_DIR/hypr/$HYPRLAND_INSTANCE_SIGNATURE/.socket2.sock) | while read -r line; do handle "$line"; done