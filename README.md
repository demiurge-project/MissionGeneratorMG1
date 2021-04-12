# README
ARGoS MissionGenerator
======================

## Package content

- 'bin' Empty folder that will contain the executable mission_generator
after compilation
- 'generated_missions' Empty folder that will contain the argos configuration files and a text file containing all string representation of the missions after the execution of mission_generator
- 'params/configurationParametersMG1.txt' The parameters of MG1  
- 'src' The source files of the package
    - 'argos3' Symbolic link to ARGoS3.
    - 'cmake' The .cmake files.
    - 'demiurge/generic-loop-functions/Arenas' The source files of the arenas (walls, floor patches, lights, obstacles, etc.)
    - 'demiurge/generic-loop-functions/Missions' The source files of the missions (constraints on the arena, objective function, etc.)
    - 'demiurge/generic-loop-functions/Generator' The source files of the generator (sampling of the variables, parsing of the missions configurations, etc.)


## Installation
### Dependencies:
- [ARGoS3](https://github.com/ilpincy/argos3) (3.0.0-beta48)
- [argos3-epuck](https://github.com/demiurge-project/argos3-epuck) (v48)
- [demiurge-epuck-dao](https://github.com/demiurge-project/demiurge-epuck-dao) (master)

### Compilation:
    $ mkdir build
    $ cmake ../src/
    $ make

 Once compiled, the 'bin/' folder should contain the 'mission_generator' executable.

 ## How to use

 The executable 'mission_generator' takes 3 parameters: 1 mandatory and 2 optionals.

 - '--conf ' 	 The configuration file [MANDATORY];
 - '--seed ' 	 The seed for the generator [OPTIONAL];
 - '--nb '	     The numbers of missions to generate [OPTIONAL].
     
### Example

./bin/mission_generator --seed 12345 --nb 30 --conf params/configurationParametersMG1.txt
