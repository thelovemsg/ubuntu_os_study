#!/bin/bash

while true ; do
	DATE=$(date | tr -d '\n')
	INFO=$(ps aux | grep demand-paging | grep -v grep)

if [ -z "$INFO" ] ; then
	echo "$DATE: target process seems to be finished"
		break
	fi
	echo "${DATE}: ${INFO}"
	sleep 1
done

