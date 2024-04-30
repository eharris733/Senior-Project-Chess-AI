# Senior Project: Applications of Genetic Algorithms to Chess Engines

## Introduction
This repository contains the relevant code that was instrumental in my senior thesis (titled above). It is really a UCI compliant chess engine written in C++, but it also includes all code that was used for data processing and genetic algorithm running. To read the thesis, please see this public link: (Insert Link here)

## Project Structure
- `/chess` - Contains the source code for the main chess engine.
    - `/engine` - Includes all code that is needed for the chess engine to run
    - `/engine_test` - Includes all code that was used for testing the various components of the chess engine
- `/ga` - Genetic algorithm implementations. Ga1 is for tuning the evaluation function. Ga2 is for the search function. 
- `/dbs` - All code and files used for data cleaning, pre-processing, and visualizing. Super messy rn -- read at your own 
risk
- `/openingbook` - Opening books that were used or tried. I do not take any credit for their creation.
- `/literature` - Contains the most relevant literature that guided this senior thesis. More is found in the actual paper, which is also in this directory. 
- `/engines` - Contains all the relevant executables. v1-6 are for a mac, any final version has a 1., and is compiled for linux

## Setup and Installation
Most likely, you will have to compile and build stuff on your own. If you want just the chess engine, you can compile base_engine.cpp found in chess/engine. An example compilation command:

/usr/bin/g++ -fdiagnostics-color=always -g ~/senior_project/chess/engine/base_engine.cpp -o ~/senior_project/chess/engine/base_engine -std=c++17 -O3 -march=native

## License
MIT License, see LICENSE file

## Contact Information
Email: elliotmharris@gmail.com
LinkedIn: [linked-in](https://www.linkedin.com/in/elliot-harris-/)

## TODO
a lot... and nothing at all. Probably should make this Readme a little better... 
If you want to try and imrpove any aspect of this project, please sumbit a pull-request! Feel free to fork or steal as well. 
