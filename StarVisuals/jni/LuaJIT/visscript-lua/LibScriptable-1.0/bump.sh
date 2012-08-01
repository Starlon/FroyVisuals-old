#!/usr/bin/bash

v1=22+1
v2=24

for file in ./*/*/*.lua 
do
	sed "s/MINOR = $v1/MINOR = $v2/" -i $file
done

for file in ./LibScriptableLocale-1.0/*.lua
do
	sed "s/MINOR = $v1/MINOR = $v2/" -i $file
done
