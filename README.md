# Senior Project: Applications of Genetic Algorithms to Chess Engines

## Introduction
This repository contains the relevant code that was instrumental in my senior thesis (titled above). It is really a UCI compliant chess engine written in C++, but it also includes all code that was used for data processing and genetic algorithm running. To read the thesis, please see this public link: (Insert Link here)

## Play GERALD
If you are interested in playing against GERALD on lichess, use this link: https://lichess.org/@/GERALD_EVOLVED
Since I have to be running a process for one to play him, feel free to email (given below) if you would like to play and Gerald is not online. 

## Project Structure
- `/chess` - Contains the source code for the main chess engine.
    - `/engine` - Includes all code that is needed for the chess engine to run
    - `/engine_test` - Includes all code that was used for testing the various components of the chess engine
- `/ga` - Genetic algorithm implementations. Ga1 is for tuning the evaluation function. Ga2 is for the search function. 
- `/dbs` - All code and files used for data cleaning, pre-processing, and visualizing. Super messy rn -- read at your own 
risk
- `/literature` - Contains the most relevant literature that guided this senior thesis. More is found in the actual paper, which is also in this directory. 
- `/engines` - Contains all the relevant executables. v1-6 are for a mac, any final version has a 1., and is compiled for linux

## Setup and Installation
Most likely, you will have to compile and build stuff on your own. If you want just the chess engine, you can compile base_engine.cpp found in chess/engine with the `./chess/build.sh` script:

    % ./chess/build.sh
    Building...
    
    Success!
    
    Run './chess/engine/base_engine' and type (w/ ENTER after each line):
    uci
    ucinewgame
    isready
    position startpos moves e2e4
    go
    quit
    % ./chess/engine/base_engine
    uci
    id name Gerald Current
    id author Elliot Harris
    uciok
    ucinewgame
    isready
    readyok
    position startpos moves e2e3
    go
    info depth 1 score cp 10 nodes 20 nps 10000 time 1 pv e7e5
    info depth 2 score cp -132 nodes 337 nps 168000 time 1 pv d7d5
    info depth 3 score cp 22 nodes 886 nps 295000 time 2 pv d7d5
    info depth 4 score cp -22 nodes 2157 nps 539000 time 3 pv d7d5
    info depth 5 score cp -25 nodes 11309 nps 706000 time 15 pv d7d5
    info depth 6 score cp -43 nodes 24234 nps 781000 time 30 pv d7d5
    bestmove d7d5
    quit

## License
MIT License, see LICENSE file

## Contact Information
Email: elliotmharris@gmail.com
LinkedIn: [linked-in](https://www.linkedin.com/in/elliot-harris-/)

## TODO
a lot... and nothing at all. Probably should make this Readme a little better... 
If you want to try and imrpove any aspect of this project, please sumbit a pull-request! Feel free to fork or steal as well. 
