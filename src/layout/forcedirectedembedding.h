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
#include "forcedirectedparametersbase.h"

class ForceDirectedEmbedding
{
    protected:
        /**
         * True means internal state for the layout has been initialized.
         * Initialization is not done during construction time, but it is done not later than
         * the first call to embed.
         */
        bool initialized;

        /**
         * True indicates that the embedding is complete, this state may be reset by calling reinitialize.
         */
        bool finished;

        /**
         * The total number of ticks spent on calculation since the last reinitialize call.
         */
        long ticks;

        /**
         * Total number of cycles since the last reinitialize call.
         */
        int cycle;

        /**
         * Total number of probe cycles since the last reinitialize call.
         */
        int probCycle;

        /**
         * Total virtual calculation time since the last reinitialize call.
         */
        double timeStepSum;

        /**
         * Total kinetic energy seen in all cycles since the last reinitialize call.
         */
        double kineticEnergySum;

        // positions
        std::vector<Pt> pn;
        // accelerations
        std::vector<Pt> an;
        // velocities
        std::vector<Pt> vn;
        // acceleration approximations
        std::vector<Pt> a1;
        std::vector<Pt> a2;
        std::vector<Pt> a3;
        std::vector<Pt> a4;
        // velocity deltas
        std::vector<Pt> dvn;
        std::vector<Pt> tvn;
        // position deltas
        std::vector<Pt> dpn;
        std::vector<Pt> tpn;

        /**
         * The calculation will find a solution for these variables.
         * Members are destructed.
         */
        std::vector<Variable *> variables;
        
        /**
         * Used to generate forces in each cycle of the calculation.
         * Members are destructed.
         */
        std::vector<IForceProvider *> forceProviders;
        
        /**
         * The various bodies which are part of the calculation.
         * Members are destructed.
         */
        std::vector<IBody *> bodies;

    public:
        /**
         * Prints debug messages to standard output during embedding.
         */
        bool debug;

        /**
         * Stops at every cycle to be able to inspect the state of the embedding. Call embed again to continue.
         */
        bool inspected;

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
        
        /**
         * This gets updated.
         */
        double updatedFrictionCoefficient;
        
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
        
        /**
         * This gets updated.
         */
        double updatedTimeStep;

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
        double maxCalculationTime;

        /**
         * Upper limit of acceleration approximation difference (between a1, a2, a3 and a4 in RK-4).
         * Acceleration error limit is updated automatically during the solution. It decreases towards zero proportional to the
         * time spent on the calculation.
         */
        double maxAccelerationError;

        /**
         *
         */
        double minAccelerationError;
        
        /**
         * This gets updated.
         */
        double updatedMaxAccelerationError;

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
         * Maximim velocity.
         */
        double maxVelocity;

        /**
         * Minimum number of cycles to run.
         */
        int minCycle;

        /**
         * Maximum number of cycles to run.
         */
        int maxCycle;

    public:
        ForceDirectedEmbedding() {
            debug = false;
            inspected = false;
            initialized = false;
            finished = false;

            // parameters
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
            maxCalculationTime = 1000;
            maxAccelerationError = 1;
            accelerationRelaxLimit = 1;
            velocityRelaxLimit = 0.1;
            maxVelocity = 100;
            minCycle = 0;
            maxCycle = INT_MAX;
        }

        ~ForceDirectedEmbedding() {
            for(std::vector<Variable *>::iterator it = variables.begin(); it != variables.end(); it++)
                delete *it;

            for(std::vector<IForceProvider *>::iterator it = forceProviders.begin(); it != forceProviders.end(); it++)
                delete *it;

            for(std::vector<IBody *>::iterator it = bodies.begin(); it != bodies.end(); it++)
                delete *it;
        }

        const std::vector<Variable *>& getVariables() const {
            return variables;
        }

        const std::vector<IForceProvider *>& getForceProviders() const {
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

        const std::vector<IBody *>& getBodies() const {
            return bodies;
        }
        
        void addBody(IBody *body) {
            bodies.push_back(body);
            
            if (std::find(variables.begin(), variables.end(), body->getVariable()) == variables.end())
                variables.push_back(body->getVariable());
        }

        bool getFinished() {
            return finished;
        }

        void reinitialize() {
            initialized = true;
            finished = false;

            cycle = 0;
            probCycle = 0;
            timeStepSum = 0;
            kineticEnergySum = 0;
            ticks = 0;

            pn = createPtArray();
            an = createPtArray();
            vn = createPtArray();
            a1 = createPtArray();
            a2 = createPtArray();
            a3 = createPtArray();
            a4 = createPtArray();
            dvn = createPtArray();
            tvn = createPtArray();
            dpn = createPtArray();
            tpn = createPtArray();

            // set positions and velocities
            for (int i = 0; i < variables.size(); i++) {
                Variable *variable = variables[i];
                pn[i].assign(variable->getPosition());
                vn[i].assign(variable->getVelocity());
            }

            // initial values
            updatedTimeStep = timeStep;
            updatedMaxAccelerationError = maxAccelerationError;
            updatedFrictionCoefficient = frictionCoefficient;
        }

        /**
         * Find the solution for the differential equation using a
         * modified Runge-Kutta 4th order algorithm.
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
            if (finished)
                return;

            if (!initialized)
                reinitialize();

            long begin = clock();

            // modified Runge Kutta 4th order
            do {
                long time;
                double hMultiplier = 0;
                double accelerationError = 0;
                cycle++;

                while (true) {
                    probCycle++;

                    // update acceleration error limit
                    time = ticksToMilliseconds(ticks + clock() - begin);
                    if (time > maxCalculationTime && cycle > minCycle)
                        break;
                    else
                        updatedMaxAccelerationError = (maxCalculationTime - time) / maxCalculationTime * this->maxAccelerationError;

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

                    if (debug)
                        std::cout << "Prob cycle at real time: " << time << " time: " << timeStepSum << " h: " << updatedTimeStep << " friction: " << updatedFrictionCoefficient << " acceleration error limit: " << updatedMaxAccelerationError << " acceleration error: " << accelerationError << " cycle: " << cycle << " prob cycle: " << probCycle << "\n";

                    if (accelerationError < updatedMaxAccelerationError) {
                        if (hMultiplier == 0)
                            hMultiplier = timeStepMultiplier;
                        else
                            break;
                    }
                    else {
                        if (hMultiplier == 0 || hMultiplier == timeStepMultiplier)
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

                time = ticksToMilliseconds(ticks + clock() - begin);
                if (debug)
                    std::cout << "Cycle at real time: " << time << " time: " << timeStepSum << " h: " << updatedTimeStep << " friction: " << updatedFrictionCoefficient << " acceleration error limit: " << updatedMaxAccelerationError << " acceleration error: " << accelerationError << " cycle: " << cycle << " prob cycle: " << probCycle << "\n";

                // update friction
                timeStepSum += updatedTimeStep;
                double kineticEnergy = 0;
                for (int i = 0; i < variables.size(); i++) {
                    Variable *variable = variables[i];
                    double velocity = variable->getVelocity().getLength();
                    kineticEnergy +=  1.0 / 2.0 * variable->getMass() * velocity * velocity;
                }
                kineticEnergySum += kineticEnergy * updatedTimeStep;
                double kineticEnergyAvg = (kineticEnergySum / timeStepSum);
                double kineticEnergyExpected = kineticEnergyAvg * (maxCalculationTime - time) / maxCalculationTime;
                if (debug)
                    std::cout << "Current kinetic energy: " << kineticEnergy << " expected: " << kineticEnergyExpected << "\n";
                if (kineticEnergy <  kineticEnergyExpected && updatedFrictionCoefficient / frictionCoefficientMultiplier > minFrictionCoefficient)
                    updatedFrictionCoefficient /= frictionCoefficientMultiplier;
                else if (kineticEnergy > kineticEnergyExpected && updatedFrictionCoefficient * frictionCoefficientMultiplier < maxFrictionCoefficient)
                    updatedFrictionCoefficient *= frictionCoefficientMultiplier;
                
                // Check if relaxed
                finished = true;
                for (int i = 0; i < vn.size(); i++) {
                    an[i].assign(a1[i]).add(a2[i]).add(a3[i]).add(a4[i]).divide(4);

                    if (vn[i].getLength() > velocityRelaxLimit) {
                        if (debug)
                            std::cout << "Not relax due to velocity: " << vn[i].getLength() << "\n";
                        finished = false;
                        break;
                    }

                    if (an[i].getLength() > accelerationRelaxLimit) {
                        if (debug)
                            std::cout << "Not relax due to acceleration: " << an[i].getLength() << "\n";
                        finished = false;
                        break;
                    }
                }

                if ((cycle == maxCycle) || (time > maxCalculationTime && cycle > minCycle))
                    finished = true;
            }
            while (!inspected && !finished);

            ticks += clock() - begin;

            if (debug) {
                if (finished)
                    std::cout << "Runge-Kutta-4 ended, at real time: " << ticksToMilliseconds(ticks) << " time: " << timeStepSum << " cycle: " << cycle << " prob cycle: " << probCycle << "\n";

                std::cout.flush();
            }
        }

    protected:
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
            if (debug) {
                std::cout << "Calling a() with:\n";
                for (int i = 0; i < pn.size(); i++) {
                    std::cout << "p[" << i << "] = " << pn[i].x << ", " << pn[i].y << " ";
                    std::cout << "v[" << i << "] = " << vn[i].x << ", " << vn[i].y << "\n";
                }
            }

            // set positions and velocities and reset forces
            for (int i = 0; i < variables.size(); i++) {
                Variable *variable = variables[i];
                variable->assignPosition(pn[i]);
                variable->assignVelocity(vn[i]);
                variable->resetForce();

                if (inspected)
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

            if (debug) {
                std::cout << "Returning from a() with:\n";
                for (int i = 0; i < pn.size(); i++)
                    std::cout << "a[" << i << "] = " << an[i].x << ", " << an[i].y << "\n";
            }
        }

        double ticksToMilliseconds(long ticks) {
            return (double)ticks / CLOCKS_PER_SEC * 1000;
        }
};

#endif
