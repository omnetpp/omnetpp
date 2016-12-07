##
## Name:
##
##   estimate_mean
##
## Description:
##
##   Given a vector of samples, this function estimates the mean
##   and confidence interval. It is assumed that the samples
##   are taken from a approximately bell-shaped population.
##   The function is optimised for sample set sizes of less
##   than 30.
##
##   t_inv is the inverse cummulative t distribution function.
##   It is supplied as part of the octave contributed functions
##   package.
##
## Parameters:
##
##   samples
##   A vector containing the sample values.
##
##   confidence
##   The confidence as a percentage.
##
## Return:
##
##   x
##   The estimated mean
##
##   hl
##   The half length confidence interval for the estimate
##   (The actual confidence interval is x +- hl)
##


function [x, hl] = estimate_mean(samples, confidence)

  ## parameter usage checks
  if (nargin!=2)
    usage("estimate_mean: requires two arguments");
  elseif (!is_vector(samples))
    usage("estimate_mean: first argument must be a vector");
  elseif (!is_scalar(confidence))
    usage("estimate_mean: second argument must be a scalar");
  endif

  ## retrieve sample size
  row = rows(samples);
  col = columns(samples);
  if (row==1)
    n = col;
  elseif (col==1)
    n = row;
  else
    error("estimate_mean: invalid sample size");
  endif

  ## calculate the mean of the samples
  x = mean(samples);
  ## calculate the half length confidence interval
  hl = t_inv((0.5+(confidence/200)), n-1)*std(samples)/sqrt(n);

endfunction
