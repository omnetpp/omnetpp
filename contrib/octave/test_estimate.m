##
## A simple script for testing estimate_mean function
##

clear;

## parameters (an execise from text book)
c = 95; ## confidence
a = [9.8, 10.2, 10.4, 9.8, 10.0, 10.2, 9.6]; ## samples

## estimate the mean
[mm, hll] = estimate_mean(a, c);

## display results
fprintf("Mean = %f\n", mm);                  ## given ans: 10.0
fprintf("Half length interval = %f\n", hll); ## given ans: 0.26
