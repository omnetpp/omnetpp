##
## simple test of calling create_seeds function
##


clear;

## setup parameters
prefix = "/usr/home/rlyon01/omnetpp";  # omnetpp path- change to your setup
num = 5;                               # the number of seeds to generate
d = 1000000;                           # the minimum distance between seeds
init = 137;                            # the initial seed for the tool

## generate the seed vector
seed_vec = create_seeds(prefix, num, d, init);

## display the results
disp("- Seed Tool Test -");
for loop = 1:num
  printf("  seed(%d) = %d\n", loop, seed_vec(loop));
endfor
disp("End of test.");
