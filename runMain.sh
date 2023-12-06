#!/bin/bash
g++ -o main main.cpp -lncurses
dot -Tpng graph.dot -o graph.png
./main