Testing the distribution functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MS Excel has several functions that calculate the probability distribution
functions. They can be used for testing the distribution functions.

Continuous:
- EXPONDIST(x,lambda,cumulative)
- NORMDIST(x,mean,standard_dev,cumulative)
- GAMMADIST(x,alpha,beta,cumulative)
- BETADIST(x,alpha,beta,0,0)
- Erlang: GAMMADIST with integer alpha
- CHIDIST(x,degrees_freedom)
- TDIST(x,degrees_freedom,tails)
   Student-t. Tails specifies the number of distribution tails to return. If tails = 1, TDIST returns the one-tailed distribution. If tails = 2, TDIST returns the two-tailed distribution.
- no Cauchy distr in Excel
- no triangular distr in Excel
- no pdf for lognormal in Excel, LOGNORMDIST(x,mean,standard_dev) is the cummulative one
- WEIBULL(x,alpha,beta,cumulative)
- no Pareto in Excel

Discrete:
- BINOMDIST(number_s,trials,probability_s,cumulative
- geometric is negbinomial's special case
- NEGBINOMDIST(number_f,number_s,probability_s)
- HYPGEOMDIST(sample_s,number_sample,population_s,number_population)
- POISSON(x,mean,cumulative)

All tests are described in omnetpp.ini. Each config tests a specific
distribution. The `dist' program is just a generic "test shell".

To perform the tests:
1. build the test executable: opp_makemake; make
2. run all runs in all configurations, using the provided "run" script: ./run. 
   You'll get a bunch of csv files as output.
3. run the show.R script in Gnu R; on each plot, the lines and dots should visually match 

Or you can use Excel:
3. open a csv file in Excel (doubleclick works on the file for me)
4. select columns 1-2-3
5. select Insert|Chart... --> XY Scatter --> Finish
6. the two plots should visually match

If you want to run specific tests only, type "dist -c gamma" (or the name of the config).
When using Tkenv, do only one step after setting up a run, then find and open
the inspector for the histogram.

See omnetpp.ini for comments on specific tests.

Gnumeric might also be usable instead of Excel, I saw many of its functions
are Excel-compatible. Nevertheless, I've never tried it. The preferred way of 
testing is now the show.R script.


--Andras

