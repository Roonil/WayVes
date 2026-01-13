#!/bin/bash

send_to_pipe() {
    local position=$1
      echo "trackPosition=$position" >> "/tmp/WayVes/clock"
      
   
}


main(){

LENGTH_US=$(playerctl metadata mpris:length 2>/dev/null)
CURRENT_POS=$(playerctl position)
POS_FRACTION=0


if [ $? -eq 0 ] && [ -n "$LENGTH_US" ]; then
    CURRENT_POS=$(echo "$CURRENT_POS*1000000" | bc)
    if [ "$LENGTH_US" -eq 0 ]; then
        POS_FRACTION=0
    else
        POS_FRACTION=$(echo "scale=9;$CURRENT_POS/$LENGTH_US" | bc)
    fi

else
    POS_FRACTION=0
fi
   send_to_pipe $POS_FRACTION
  

}

while true; do 
 (sleep 0.01)
main
 done