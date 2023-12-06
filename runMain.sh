#!/bin/bash
g++ -o main main.cpp -lncurses && ./main
dot -Tpng graph.dot -o graph.png