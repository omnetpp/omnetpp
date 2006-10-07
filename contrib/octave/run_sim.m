##
## Name:
##
##   run_sim
##
## Description:
##
##   This function calls a simulation and displays the output immediately
##   on the console. The function only returns when the simulation
##   is completed. Apart from the simulation file, the configuration file
##   may also be optionally specified.
##
## Parameters:
##
##   sim_file
##   A string containing the simulation filename. This must be identical
##   to what you normally enter from the command line. Some typical
##   examples are:
##                    "./MySimulation"
##                    "/usr/home/user1/MySimulation"
##
##   This string should not contain any command-line arguments apart
##   from the simulation filename.
##
##   ini_file
##   A string containing the simulation configuration filename. This must be
##   identical to what you normally enter from the command line. Some
##   typical examples are:
##                    "demo.ini"
##                    "/usr/home/user1/omnetpp.ini
##
## Return:
##
##   None. If there is an error the function will abort and display an
##   error message.
##


function run_sim(sim_file, ini_file)

  ## check the number of arguments and create the command line
  if (nargin==2)
    com_line = sprintf("%s -f %s", sim_file, ini_file);
  elseif (nargin==1)
    com_line = sprintf("%s", sim_file);
  else
    error("run_sim called with an incorrect parameter list");
  endif

  ## run the simulation, immediately displaying the output
  fflush(stdout);
  system(com_line);
  fflush(stdout);

endfunction
