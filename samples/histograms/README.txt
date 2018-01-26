Histogram demo
==============

A demo for the density estimation/histogram classes.

About the model

The network consists of a single module. It goes into a loop to generate
random numbers. The numbers are inserted into histograms and other
density estimation objects using different algorithms. Some of the objects
use the first n numbers to estimate a range, and set up histogram cells
accordingly.

When you start the model, it will ask a couple of questions; it is OK
to accept the defaults offered. When you run the model, several inspector
windows will appear which display histogram-like diagrams. (The corresponding
objects you can find in the object tree on the left side of the main
window, after opening the "Histograms" and "local-objects" nodes.)

The following classes are demonstrated:

* cHistogram
A generic histogram class. The algorithm for laying out the bins is
encapsulated into a "strategy" class. This demo shows several histogram
stategies.

* cPSquare
Implementation of the P^2 (P-square) algorithm from Jain and Chlamtac.
It is an on-line density estimation method that works by estimating and
keeping track of quantiles. The resulting (calculated) histogram will
thus contain roughly equi-probable cells.

* cKSplit
An adaptive histogram structure which splits those cells that receive
"too many" observations (this is defined by a split criterion), thus
"refining the resolution" at (or "zooming in" on) the interesting parts
of the distribution.

