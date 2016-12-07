##
## A simple script for testing the run_sim function
##


clear;

## parameters (you'll have to change these)
simulation = "./model";
configuration = "demo.ini";

## run the simulation
disp("\n---- SIMULATION STARTED ----");
run_sim(simulation, configuration)
disp("\n------- SIMULATION ENDED -------");
