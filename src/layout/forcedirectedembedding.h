//=========================================================================
//  FORCEDIRECTEDEMBEDDING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FORCEDIRECTEDEMBEDDING_H
#define __FORCEDIRECTEDEMBEDDING_H

#include <algorithm>
#include <time.h>
#include <iostream>
#include "geometry.h"
#include "forcedirectedparameters.h"

#define GLRAND_MAX  0x7ffffffeL  /* = 2**31-2 */

class ForceDirectedEmbedding
{
    protected:
	    bool debug;
	
	    std::vector<Variable *> variables;
    	
	    std::vector<IForceProvider *> forceProviders;
    	
	    std::vector<IBody *> bodies;

    public:
	    /**
	     * Spring force coefficient.
	     */
	    double springCoefficient;

	    /**
	     * Electric repeal force coefficient.
	     */
	    double electricRepealCoefficient;

	    /**
	     * Friction reduces the energy of the system. The friction force points in the opposite direction of the current velocity.
	     * Friction is automatically updated during the solution. The current kinetic energy is compared against the average kinetic
	     * energy reduced by the time spent on the calculation and the friction coefficient is updated so that towards the end of the
	     * calculation the kinetic energy of the system converges to zero.
	     */
	    double frictionCoefficient;
    	
    protected:
	    /**
	     * This gets updated.
	     */
	    double updatedFrictionCoefficient;
    	
    public:
	    /**
	     * Lower limit for friction coefficient.
	     */
	    double minFrictionCoefficient;
    	
	    /**
	     * Higher limit for friction coefficient update.
	     */
	    double maxFrictionCoefficient;
    	
	    /**
	     * Multiplier used to update the friction coefficient.
	     */
	    double frictionCoefficientMultiplier;

	    /**
	     * The default time step when solution starts.
	     * Time step is automatically updated during the solution. It will always have the highest value so that the acceleration error
	     * is less than the acceleration error limit. The time step is either multiplied or divided by the time step multiplier according to
	     * the current acceleration error.
	     */
	    double timeStep;
    	
    protected:
	    /**
	     * This gets updated.
	     */
	    double updatedTimeStep;

    public:
	    /**
	     * Lower limit for time step update.
	     */
	    double minTimeStep;

	    /**
	     * Lower limit for time step update.
	     */
	    double maxTimeStep;

	    /**
	     * Multiplier used to update the time step.
	     */
	    double timeStepMultiplier;

	    /**
	     * Maximum time spend on the calculation in milliseconds.
	     * The algorithm will always return after this time has elapsed.
	     */
	    double calculationTimeLimit;

	    /**
	     * Limit of acceleration approximation difference (between a1, a2, a3 and a4 in RK-4).
	     * Acceleration error limit is updated automatically during the solution. It decreases towards zero proportional to the
	     * time spent on the calculation.
	     */
	    double accelerationErrorLimit;
    	
    protected:
	    /**
	     * This gets updated.
	     */
	    double updatedAccelerationErrorLimit;

    public:
	    /**
	     * Acceleration limit during the solution.
	     * When all vertices has lower acceleration than this limit then the algorithm may be stopped.
	     */
	    double accelerationRelaxLimit;

	    /**
	     * Velocity limit during the solution.
	     * When all vertices has lower velocity than this limit then the algorithm may be stopped.
	     */
	    double velocityRelaxLimit;

	    /**
	     * Maximim velocity;
	     */
	    double maxVelocity;

    public:
	    ForceDirectedEmbedding() {
            debug = false;
	        springCoefficient = 0.1;
	        electricRepealCoefficient = 100000;
	        frictionCoefficient = 1;
	        minFrictionCoefficient = 0.1;
	        maxFrictionCoefficient = 10;
	        frictionCoefficientMultiplier = 2;
	        timeStep = 1;
	        minTimeStep = 0.001;
	        maxTimeStep = 1000;
	        timeStepMultiplier = 2;
	        calculationTimeLimit = 100000;
	        accelerationErrorLimit = 1;
	        accelerationRelaxLimit = 1;
	        velocityRelaxLimit = 0.1;
	        maxVelocity = 100;
	    }

	    std::vector<Variable *>& getVariables() {
		    return variables;
	    }

	    std::vector<IForceProvider *>& getForceProviders() {
		    return forceProviders;
	    }

	    void addForceProvider(IForceProvider *forceProvider) {
		    forceProviders.push_back(forceProvider);
	    }

	    void removeForceProvider(IForceProvider *forceProvider) {
            std::vector<IForceProvider *>::iterator it = std::find(forceProviders.begin(), forceProviders.end(), forceProvider);

            if (it != forceProviders.end())
                forceProviders.erase(it);
	    }

	    std::vector<IBody *>& getBodies() {
		    return bodies;
	    }
    	
	    void addBody(IBody *body) {
		    bodies.push_back(body);
    		
		    if (std::find(variables.begin(), variables.end(), body->getVariable()) == variables.end())
			    variables.push_back(body->getVariable());
	    }

    protected:
	    /**
	     * Find the solution for the differential equation using a
         * modified Runge-Kutta-4 algorithm.
         *
         * a1 = a[pn, vn]
         * a2 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a1]
         * a3 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a2]
         * a4 = a[pn + h * vn + h * h / 2 * a3, vn + h * a3]
         *
         * pn+1 = pn + h * vn + h * h / 6 * [a1 + a2 + a3]
         * vn+1 = vn + h / 6 * (a1 + 2 * a2 + 2 * a3 + a4)
         * 
         * This algorithm adaptively modifies timeStep and friction.
         */
	    void embed() {
		    long begin = clock();
		    int cycle = 0;
		    int probCycle = 0;
		    bool relaxed = false;
		    double timeStepSum = 0;
		    double energySum = 0;
		    double energy;
		    // vn : positions
		    std::vector<Pt> pn = createPtArray();
		    // accelerations
		    std::vector<Pt> an = createPtArray();
		    // vn : velocities
		    std::vector<Pt> vn = createPtArray();
		    // as : accelerations
		    std::vector<Pt> a1 = createPtArray();
		    std::vector<Pt> a2 = createPtArray();
		    std::vector<Pt> a3 = createPtArray();
		    std::vector<Pt> a4 = createPtArray();
		    // velocity deltas
		    std::vector<Pt> dvn = createPtArray();
		    std::vector<Pt> tvn = createPtArray();
		    // position deltas
		    std::vector<Pt> dpn = createPtArray();
		    std::vector<Pt> tpn = createPtArray();

		    // set positions and velocities
		    for (int i = 0; i < variables.size(); i++) {
			    Variable *variable = variables[i];
			    pn[i].assign(variable->getPosition());
			    vn[i].assign(variable->getVelocity());
		    }

		    // initial values
		    updatedTimeStep = timeStep;
		    updatedAccelerationErrorLimit = accelerationErrorLimit;
		    updatedFrictionCoefficient = frictionCoefficient;

		    // Runge Kutta 4th order
		    while (!relaxed) {
			    long time;
			    double hMultiplier = 0;
			    double accelerationError = 0;
			    cycle++;

			    while (true) {
				    probCycle++;

				    // update acceleration error limit
				    time = clockToTime(clock() - begin);
				    if (time > calculationTimeLimit)
					    break;
				    else
					    updatedAccelerationErrorLimit = (calculationTimeLimit - time) / calculationTimeLimit * this->accelerationErrorLimit;
				    if (debug)
                        std::cout << "Prob cycle at real time: " << time << " time: " << timeStepSum << " h: " << updatedTimeStep << " friction: " << updatedFrictionCoefficient << " acceleration error limit: " << updatedAccelerationErrorLimit << " acceleration error: " << accelerationError << " cycle: " << cycle << " prob cycle: " << probCycle << "\n";

				    // a1 = a[pn, vn]
				    a(a1, pn, vn);

				    // a2 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a1]
				    addMultiplied(tpn, pn, updatedTimeStep / 2, vn);
				    incrementWithMultiplied(tpn, updatedTimeStep * updatedTimeStep / 8, a1);
				    addMultiplied(tvn, vn, updatedTimeStep / 2, a1);
				    a(a2, tpn, tvn);

				    // a3 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a2]
				    addMultiplied(tpn, pn, updatedTimeStep / 2, vn);
				    incrementWithMultiplied(tpn, updatedTimeStep * updatedTimeStep / 8, a1);
				    addMultiplied(tvn, vn, updatedTimeStep / 2, a2);
				    a(a3, tpn, tvn);

				    // a4 = a[pn + h * vn + h * h / 2 * a3, vn + h * a3]
				    addMultiplied(tpn, pn, updatedTimeStep, vn);
				    incrementWithMultiplied(tpn, updatedTimeStep * updatedTimeStep / 2, a3);
				    addMultiplied(tvn, vn, updatedTimeStep, a3);
				    a(a4, tpn, tvn);

				    // Adjust time step (h)
				    accelerationError = maximumDifference(a1, a2, a3, a4);
    				
				    if (accelerationError < updatedAccelerationErrorLimit) {
					    if (hMultiplier == 0)
						    hMultiplier = timeStepMultiplier;
					    else
						    break;
				    }
				    else {
					    if (hMultiplier == 0)
						    hMultiplier = timeStepMultiplier;
					    else if (hMultiplier == timeStepMultiplier)
						    hMultiplier = 1.0 / timeStepMultiplier;
				    }
    				
				    double hNext = updatedTimeStep * hMultiplier;

				    if (hNext < minTimeStep || hNext > maxTimeStep)
					    break;
    				
				    updatedTimeStep *= hMultiplier;
			    }

			    // pn+1 = pn + h * vn + h * h / 6 * [a1 + a2 + a3]
			    add(dpn, a1, a2);
			    increment(dpn, a3);
			    multiply(dpn, updatedTimeStep * updatedTimeStep / 6);
			    incrementWithMultiplied(dpn, updatedTimeStep, vn);
			    increment(pn, dpn);

			    // vn+1 = vn + h / 6 * (a1 + 2 * a2 + 2 * a3 + a4)
			    addMultiplied(dvn, a1, 2, a2);
			    incrementWithMultiplied(dvn, 2, a3);
			    increment(dvn, a4);
			    multiply(dvn, updatedTimeStep / 6);
			    increment(vn, dvn);
			    // Maximize velocity
			    for (int i = 0; i < vn.size(); i++) {
				    if (vn[i].getLength() > maxVelocity) {
					    vn[i].normalize();
					    vn[i].multiply(maxVelocity);
				    }
			    }

			    // check time limit
			    time = clockToTime(clock() - begin);
			    if (time > calculationTimeLimit)
				    break;
			    if (debug)
				    std::cout << "Cycle at real time: " << time << " time: " << timeStepSum << " h: " << updatedTimeStep << " friction: " << updatedFrictionCoefficient << " acceleration error limit: " << updatedAccelerationErrorLimit << " acceleration error: " << accelerationError << " cycle: " << cycle << " prob cycle: " << probCycle << "\n";

			    // update friction
			    timeStepSum += updatedTimeStep;
			    energy = 0;
    		    for (int i = 0; i < variables.size(); i++) {
	    		    Variable *variable = variables[i];
				    double velocity = variable->getVelocity().getLength();
				    energy +=  1.0 / 2.0 * variable->getMass() * velocity * velocity;
			    }
			    energySum += energy * updatedTimeStep;
			    double energyAvg = (energySum / timeStepSum);
			    double energyExpected = energyAvg * (calculationTimeLimit - time) / calculationTimeLimit;
			    if (debug)
				    std::cout << "Energy: " << energy << " energy expected: " << energyExpected << "\n";
			    if (energy <  energyExpected && updatedFrictionCoefficient / frictionCoefficientMultiplier > minFrictionCoefficient)
				    updatedFrictionCoefficient /= frictionCoefficientMultiplier;
			    else if (energy > energyExpected && updatedFrictionCoefficient * frictionCoefficientMultiplier < maxFrictionCoefficient)
				    updatedFrictionCoefficient *= frictionCoefficientMultiplier;
    			
			    // Check if relaxed
			    relaxed = true;
			    for (int i = 0; i < vn.size(); i++) {
				    an[i].assign(a1[i]).add(a2[i]).add(a3[i]).add(a4[i]).divide(4);

				    if (vn[i].getLength() > velocityRelaxLimit) {
					    if (debug)
						    std::cout << "Not relax due to velocity: " << vn[i].getLength() << "\n";
					    relaxed = false;
					    break;
				    }

				    if (an[i].getLength() > accelerationRelaxLimit) {
					    if (debug)
						    std::cout << "Not relax due to acceleration: " << an[i].getLength() << "\n";
					    relaxed = false;
					    break;
				    }
			    }
		    }
    		
		    long end = clock();
		    std::cout << "Runge-Kutta-4 ended, at real time: " << clockToTime(end - begin) << " cycle: " << cycle << " prob cycle: " << probCycle << "\n";
	    }

	    std::vector<Pt> createPtArray() {
		    std::vector<Pt> pts;

		    for (int i = 0; i < variables.size(); i++)
                pts.push_back(Pt::getZero());

		    return pts;
	    }

	    double maximumDifference(const std::vector<Pt>& a1, const std::vector<Pt>& a2, const std::vector<Pt>& a3, const std::vector<Pt>& a4)
	    {
		    double max = 0;
		    ASSERT(a1.size() == a2.size() && a2.size() == a3.size() && a3.size() == a4.size());

		    for (int i = 0; i < a1.size(); i++) {
                max = std::max(max, a1[i].getDistance(a2[i]));
			    max = std::max(max, a2[i].getDistance(a3[i]));
			    max = std::max(max, a3[i].getDistance(a4[i]));
		    }

		    return max;
	    }

	    /**
	     * pts = a + b * c
	     */
	    void addMultiplied(std::vector<Pt>& pts, const std::vector<Pt>& a, double b, const std::vector<Pt>& c)
	    {
		    ASSERT(a.size() == c.size());
		    ASSERT(pts.size() == c.size());

		    for (int i = 0; i < pts.size(); i++)
			    pts[i].assign(c[i]).multiply(b).add(a[i]);
	    }

	    /**
	     * pts += a * b
	     */
	    void incrementWithMultiplied(std::vector<Pt>& pts, double a, const std::vector<Pt>& b)
	    {
		    ASSERT(pts.size() == b.size());

		    for (int i = 0; i < pts.size(); i++) {
			    Pt& pt = pts[i];
			    double x = pt.x;
			    double y = pt.y;

			    pts[i].assign(b[i]).multiply(a).add(x, y);
		    }
	    }

	    /**
	     * pts = a + b
	     */
	    void add(std::vector<Pt>& pts, const std::vector<Pt>& a, const std::vector<Pt>& b)
	    {
		    ASSERT(a.size() == b.size());

		    for (int i = 0; i < pts.size(); i++)
			    pts[i].assign(a[i]).add(b[i]);
	    }

	    /**
	     * pts += a
	     */
	    void increment(std::vector<Pt>& pts, const std::vector<Pt>& a)
	    {
		    ASSERT(pts.size() == a.size());

		    for (int i = 0; i < pts.size(); i++)
			    pts[i].add(a[i]);
	    }

	    /**
	     * pts *= a
	     */
	    void multiply(std::vector<Pt>& pts, double a)
	    {
		    for (int i = 0; i < pts.size(); i++)
			    pts[i].multiply(a);
	    }

	    /**
	     * an = b[pn, vn]
	     */
	    void a(std::vector<Pt>& an, const std::vector<Pt>& pn, const std::vector<Pt>& vn)
	    {
		    // set positions and velocities and reset forces
		    for (int i = 0; i < variables.size(); i++) {
			    Variable *variable = variables[i];
			    variable->assignPosition(pn[i]);
			    variable->assignVelocity(vn[i]);
			    variable->resetForces();
		    }

		    // calculate forces
            for (std::vector<IForceProvider *>::iterator it = forceProviders.begin(); it != forceProviders.end(); it++)
			    (*it)->applyForces(*this);

		    // return accelerations
		    for (int i = 0; i < variables.size(); i++) {
			    Variable *variable = variables[i];
			    an[i].assign(variable->getAcceleration());
		    }
	    }

        double clockToTime(long ticks) {
            return ticks / CLOCKS_PER_SEC;
        }
};

#endif
