##
## replicate.m
##
## Run multiple simulations and display confidence intevals.
##


## initialise constants
clear;
prefix = "/usr/home/rlyon01/omnetpp"; # omnetpp directory
model = "./model";                    # simulation command
A = 95;                               # confidence


##
## get user input to construct omnetpp configration file
##

## get title of this simulation run
simtitle = input("Simulation title: ", "s");

## get the number of subscribers
subscribers = fix(input("Number of subscribers: "));
if (subscribers<1)
  error("Error - the number of subscribers must be greater than 0");
endif

## get the message length pdf type
##
## 0 - fixed length
## 1 - exponential
## 2 - Erlang-2
## n - Erlang-n
##
pdftype = fix(input("Message length pdf type: "));
if (pdftype<0)
  error("The pdf type must be 0 or greater");
endif

## get the average message length
length = input("Average message length (sec): ");
if (length<=0.0)
  error("The average message length must be greater than 0.0");
endif

## get the number of message transmission attempts
attempts = fix(input("Number of transmission attempts per message: "));
if (attempts<1)
  error("The number of attempts must be greater than 0");
endif

## get the message transmission retry interval
retry = input("Message retry interval (sec): ");
if (retry<=0.0)
  error("The retry interval must be greater than 0.0");
endif

## get the message rate
rate = input("Message subsmission rate (msg/sec): ");
if (rate<=0.0)
  error("The message rate must have a value greater than 0.0");
endif

## get the simulation duration
T = input("Duration (sec): ");
if (T<=0.0)
  error("The duration must have a value greater than 0.0");
endif

## get the number of replications
R = fix(input("Number of replications: "));
if (R<=1)
  error("The replications must have a value greater than zero");
endif

## get the starting seed
startseed = fix(input("Starting seed: "));
if (startseed<1)
  error("The starting seed must be greater than 0");
endif


##
## generate random seeds
##

## random seeds for intervals between submissions
interval_seeds = create_seeds(prefix, R, 2*rate*T, startseed);
## random seeds for retry intervals
retry_seeds = create_seeds(prefix, R, T/length, interval_seeds(R));
## random seeds for message lengths
if (pdftype>0)
  length_seeds = create_seeds(prefix, R, 2*rate*T*pdftype, retry_seeds(R));
endif


##
## create ini file
##

ininame = sprintf("%s.ini", simtitle);
scalarname = sprintf("%s.sca", simtitle);

fidout = fopen(ininame, "w", "native");
fprintf(fidout, "[General]\n");
fprintf(fidout, "ini-warnings = yes\n");
fprintf(fidout, "network = aloha\n");
fprintf(fidout, "distributed = no\n");
fprintf(fidout, "number-of-runs = %d\n", R);
fprintf(fidout, "snapshot-file = \"aloha.sna\"\n");
fprintf(fidout, "output-vector-file = \"aloha.vec\"\n");
fprintf(fidout, "output-scalar-file = \"%s\"\n", scalarname);
fprintf(fidout, "pause-in-sendmsg = no\n");
fprintf(fidout, "warnings = yes\n");
fprintf(fidout, "log-parchanges = no\n");
fprintf(fidout, "parchange-file = \"aloha.pch\"\n");
fprintf(fidout, "cpu-time-limit = 0\n");
fprintf(fidout, "total-stack-kb = 8192\n");
fprintf(fidout, "netif-check-freq= 1\n");
fprintf(fidout, "sim-time-limit = %ds\n\n", T);

fprintf(fidout, "[Cmdenv]\n");
fprintf(fidout, "runs-to-execute = 1-%d\n", R);
fprintf(fidout, "module-messages = no\n");
fprintf(fidout, "verbose-simulation = no\n");
fprintf(fidout, "display-update = 100000000\n\n");

fprintf(fidout, "[Parameters]\n");
fprintf(fidout, "aloha.N = %d\n", subscribers);
fprintf(fidout, "aloha.DISTORDER = %d\n", pdftype);
fprintf(fidout, "aloha.AVGMSGLEN = %fs\n", length);
fprintf(fidout, "aloha.TXATTEMPTS = %d\n", attempts);
fprintf(fidout, "aloha.RETRYINTERVAL = %fs\n", retry);
fprintf(fidout, "aloha.MSGRATE = %f\n\n", rate);

for loop = 1:R
  fprintf(fidout, "[Run %d]\n", loop);
  fprintf(fidout, "gen1-seed = %d\n", interval_seeds(loop));
  if(pdftype>0)
    fprintf(fidout, "gen2-seed = %d\n", length_seeds(loop));
  endif
  fprintf(fidout, "gen3-seed = %d\n\n", retry_seeds(loop));
endfor

fclose(fidout);


##
## run the simulations
##

## delete existing scalar file
command = sprintf("rm -f %s", scalarname);
system(command);
## run the simulation
run_sim(model, ininame);


##
## retrieve the results from the scalar file (note: some of the format
## strings have been truncated for this mail)
##

fidin = fopen(scalarname, "r", "native");
if (fidin==-1)
  error("Unable to open the scalar file");
endif
while (isstr(s = fgets(fidin)))
  ## get the run number
  run = fix(sscanf(s, "run %d"));
  ## get the submitted traffic
  s = fgets(fidin);
  Submitted(run) = sscanf(s, "scalar \"aloha.mBase\"  \"Submitted Traffic\"  %f", "C");
  ## get the transmitted traffic
  s = fgets(fidin);
  Transmitted(run) = sscanf(s, "scalar \"aloha.mBase\"  \"Transmitted Traffic\"  %f", "C");
  ## get the received traffic
  s = fgets(fidin);
  Received(run) = sscanf(s, "scalar \"aloha.mBase\"  \"Received Traffic\"  %f", "C");
endwhile
fclose(fidin);


##
## analyse the results
##

[mean_s, hl_s] = estimate_mean(Submitted, A);
[mean_t, hl_t] = estimate_mean(Transmitted, A);
[mean_r, hl_r] = estimate_mean(Received, A);


##
## save the analysis in the results file
##

resultname = sprintf("%s.rst", simtitle);
fidout = fopen(resultname, "w", "native");
fprintf(fidout, "Mean submitted traffic   = %f", mean_s);
fprintf(fidout, ", \twith h.l. interval = %f\n", hl_s);
fprintf(fidout, "Mean transmitted traffic = %f", mean_t);
fprintf(fidout, ", \twith h.l. interval = %f\n", hl_t);
fprintf(fidout, "Mean received traffic    = %f", mean_r);
fprintf(fidout, ", \twith h.l. interval = %f\n", hl_r);
fclose(fidout);

##
## display the analysis on console
##

printf("\nMean submitted traffic   = %f", mean_s);
printf(", \twith h.l. interval = %f\n", hl_s);
printf("Mean transmitted traffic = %f", mean_t);
printf(", \twith h.l. interval = %f\n", hl_t);
printf("Mean received traffic    = %f", mean_r);
printf(", \twith h.l. interval = %f\n", hl_r);
