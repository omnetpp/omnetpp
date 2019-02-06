# show.R
#
# Draws plots of the experimental distributions stored in csv files in the current working directory
# together with the theoretical probability distribution.
#
# TODO: pareto_shifted distribution
#
# Author: Tomi
###############################################################################

dundefined <- function(x, ...) { 0.0 }


# redefine some distribution so they accept the same parameters as in the csv file
dbernoulli_ <- function(x, p) { dbinom(x, 1, p) }
derlang_    <- function(x, k, mean) { dgamma(x, k, scale=mean/k) }
dexp_       <- function(x, mean) { dexp(x, 1/mean) }
dgamma_     <- function(x, shape, scale) { dgamma(x, shape, scale=scale) }
dintunif_   <- function(x, min, max) { if (is.na(x) | x<min | x>max) 0.0 else 1/(max-min+1) }
dpareto_    <- function(x, a, b, c) { a/b*(b/(x+c))^(a+1) }
dtruncnorm_ <- function(x, mean, sd) { if (is.na(x) | x<0) 0.0 else dnorm(x, mean, sd)/(1-pnorm(0,mean,sd)) }
dtriang_    <- function (x, a, b, c) {
	if (is.na(x) | x<a | x>c) 0.0
	else if (x<b) 2*(x-a)/(c-a)/(b-a)
	else 2*(c-x)/(c-a)/(c-b)
}
dweibull_   <- function(x,scale,shape) { dweibull(x, shape, scale) }

# names of the theoretical distributions
dists <- c("dbernoulli_", "dbeta", "dbinom", "dcauchy", "dchisq", "derlang_",
		"dexp_", "dgamma_", "dgeom", "dintunif_", "dlnorm", "dnbinom",
		"dnorm", "dpareto_", "dpois", "dt", "dtriang_", "dtruncnorm_",
		"dunif", "dweibull_", "dpareto_")

# names of the distributions in the csv files
names(dists) <- c("bernoulli", "beta", "binomial", "cauchy", "chi_square", "erlang_k",
		"exponential", "gamma_d", "geometric", "intuniform", "lognormal", "negbinomial",
		"normal", "pareto_shifted", "poisson", "student_t", "triang", "truncnormal",
		"uniform", "weibull")



# reads a csv file and returns the name of the distribution and its data
readFile <- function(fileName) {
	header <- scan(fileName, what=list(x="", t="", m="", mm="", md=""), nlines=1, sep=",");
	dist <- header$t;
	dist <- substring(dist, first=nchar("theoretical ")+1, nchar(dist)-nchar(" pdf"));
	frame <- read.csv(fileName);
	list(dist=dist, data=frame)
}

# computes the theoretical distribution values
# dist may have parameters, e.g. "normal(5,3)"
theoreticalValues <- function(dist, x) {
	fields <- strsplit(dist, "\\(|\\)|\\,| ")[[1]];
	fields <- fields[fields!=""];
	name <- fields[1];
	params <- as.numeric(fields[-1]);
	mapply(dists[name], x, MoreArgs=as.list(params))
}

# draws the plot with the experimental and theoretical distribution
display <- function(dist, data) {
	x <- data[[1]];
	experimental <- data[[3]];
	theoretical <- theoreticalValues(dist, x);

	plot(x, experimental, main=dist);
	lines(x, theoretical)
}

#setwd("c:\\work\\trunk\\omnetpp\\test\\dist")
files <- dir(pattern=".*\\.csv$")

for (file in files) {
	content <- readFile(file);
	dist <- content$dist;
	data <- content$data;
	show(dist);
	display(dist, data);
	readline("Press <ENTER> for the next plot.")
}
