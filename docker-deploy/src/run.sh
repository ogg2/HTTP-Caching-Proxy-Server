#!/bin/bash

make clean

make proxy

./proxy

while true
do
    sleep 1
done

