#!/bin/bash

main(){

TIME=$(date +"%I:%M:%S")
DELIMITER=":"

IFS="$DELIMITER" read -r -a arr <<< "$TIME"

HOURS="${arr[0]}"
MINUTES="${arr[1]}" 
SECONDS="${arr[2]}"


MINUTE_TIME_IN_SEC=$(echo "$MINUTES*60+$SECONDS"|bc)
HOUR_TIME_IN_SEC=$(echo "$MINUTE_TIME_IN_SEC+3600*$HOURS"|bc)

MINUTE_ANGLE=$(echo "scale=5;$MINUTE_TIME_IN_SEC/10*-1-180"|bc)
HOUR_ANGLE=$(echo "scale=5;$HOUR_TIME_IN_SEC/-120-180"|bc)

echo "handAngle=$MINUTE_ANGLE" >> "/tmp/WayVes/clock_minute"
echo "handAngle=$HOUR_ANGLE" >> "/tmp/WayVes/clock_hour"
}

counter=0

while true; do
    sleep 0.01
    counter=$((counter+1))
    main $counter
done