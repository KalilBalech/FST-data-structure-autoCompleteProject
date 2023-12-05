#!/bin/bash
g++ -o myFST main.cpp
./myFST
dot -Tpng graph.dot -o graph.png