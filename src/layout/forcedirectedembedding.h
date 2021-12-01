//=========================================================================
//  FORCEDIRECTEDEMBEDDING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_LAYOUT_FORCEDIRECTEDEMBEDDING_H
#define __OMNETPP_LAYOUT_FORCEDIRECTEDEMBEDDING_H

#include <cstdint>
#include <algorithm>
#include <ctime>
#include <iostream>
#include "geometry.h"
#include "forcedirectedparametersbase.h"

namespace omnetpp {
namespace layout {

class LAYOUT_API ForceDirectedEmbedding
{
    protected:
        /**
         * True means internal state for the layout has been initialized.
         * Initialization is not done during construction time,
         * but it is done not later than the first call to embed.
         */
        bool initialized = false;

        /**
         * True indicates that the embedding is complete, this state may be reset by calling reinitialize.
         */
        bool finished = false;

        /**
         * Total number of calculation cycles in the main loop since the last reinitialize call.
         */
        int cycle;

        /**
         * Total number of calculation probe cycles in the inner loop since the last reinitialize call.
         */
        int probCycle;

        /**
         * The total number of ticks spent on calculation since the last reinitialize call.
         */
        long elapsedTicks;

        /**
         * Total virtual calculation time since the last reinitialize call.
         */
        double elapsedTime;

        /**
         * Total kinetic energy seen in all cycles since the last reinitialize call.
         */
        double kineticEnergySum;

        /**
         * Total mass of bodies.
         */
        double totalMass;

        /**
         * Last calculated acceleration error.
         */
        double lastAccelerationError;

        /**
         * Last calculated maximum velocity.
         */
        double lastMaxVelocity;

        /**
         * Last calculated maximum acceleration.
         */
        double lastMaxAcceleration;

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
         * Algorithm parameters.
         */
        ForceDirectedParameters parameters;

        /**
         * Valid debug levels are: 0, 1, 2, 3, 4.
         * Higher debug level will print more debug messages to the standard output during embedding.
         * Debug level 0 means embedding will not print anything.
         */
        int debugLevel = 0;

        /**
         * When true embedding stops at every cycle to be able to inspect the state of the embedding.
         * Call embed again to continue.
         */
        bool inspected = false;

        /**
         * A value between 0 and 1, where 0 means initialized state and 1 means finished state.
         * This will be updated according to the current internal state during the solution.
         * The value might decrease during the calculation although it is expected to increase most of the time.
         */
        double relaxFactor;

        /**
         * The total time spent on calculation since the last reinitialize call.
         */
        double elapsedCalculationTime;

        /**
         * Time step is automatically updated during the solution. It will always
         * have the highest value so that the acceleration error is less than the max
         * acceleration error. The time step is either multiplied or divided by the
         * time step multiplier according to the current acceleration error.
         */
        double updatedTimeStep;

    public:
        ForceDirectedEmbedding();
        virtual ~ForceDirectedEmbedding();

        const std::vector<Variable *>& getVariables() const {
            return variables;
        }

        const std::vector<IForceProvider *>& getForceProviders() const {
            return forceProviders;
        }

        void addForceProvider(IForceProvider *forceProvider) {
            forceProviders.push_back(forceProvider);
            forceProvider->setForceDirectedEmbedding(this);
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
            body->setForceDirectedEmbedding(this);

            Variable *variable = body->getVariable();
            if (std::find(variables.begin(), variables.end(), variable) == variables.end()) {
                variable->setForceDirectedEmbedding(this);
                variables.push_back(variable);
            }
        }

        bool getFinished() {
            return finished;
        }

        /**
         * Sets the default parameters.
         */
        ForceDirectedParameters getParameters(int32_t seed = 1);

        /**
         * Clears all results from previous calculations and sets initial values.
         */
        void reinitialize();

        /**
         * Calculate the total current kinetic energy.
         */
        double getKineticEnergy() {
            double sum = 0;
            for (int i = 0; i < (int)variables.size(); i++)
                sum +=  variables[i]->getKineticEnergy();

            return sum;
        }

        /**
         * Calculate the total current potential energy.
         */
        double getPotentialEnergy() {
            double sum = 0;
            for (int i = 0; i < (int)forceProviders.size(); i++)
                sum += forceProviders[i]->getPotentialEnergy();

            return sum;
        }

        double getEnergyBasedFrictionCoefficient(double time) {
            return 3 * totalMass / time * log((getPotentialEnergy() + getKineticEnergy()) / parameters.velocityRelaxLimit);
        }

        Rc getBoundingRectangle();

        /**
         * Find the solution for the differential equation using a
         * modified Runge-Kutta 4th order algorithm.
         *
         * a1 = a[pn, vn]
         * a2 = a[pn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a1]
         * a3 = a[pn + h / 2 * vn + h * h / 8 * a2, vn + h / 2 * a2]
         * a4 = a[pn + h * vn + h * h / 2 * a3, vn + h * a3]
         *
         * pn+1 = pn + h * vn + h * h / 6 * [a1 + a2 + a3]
         * vn+1 = vn + h / 6 * (a1 + 2 * a2 + 2 * a3 + a4)
         *
         * This algorithm adaptively modifies timeStep and friction.
         */
        void embed();

        /**
         * Writes internal debug information into the given stream.
         */
        void writeDebugInformation(std::ostream& ostream);

    protected:
        /**
         * Create a Pt array filled with zeros.
         */
        std::vector<Pt> createPtArray() {
            std::vector<Pt> pts;

            for (int i = 0; i < (int)variables.size(); i++)
                pts.push_back(Pt::getZero());

            return pts;
        }

        /**
         * Calculate the average relative error of any corresponding pair between a1, a2, a3 and a4
         * relative to the absolute a values.
         */
        double averageRelativeError(const std::vector<Pt>& a1, const std::vector<Pt>& a2,
                                 const std::vector<Pt>& a3, const std::vector<Pt>& a4)
        {
            double sum1 = 0;
            double sum2 = 0;
            Assert(a1.size() == a2.size() && a2.size() == a3.size() && a3.size() == a4.size());

            for (int i = 0; i < (int)a1.size(); i++) {
                sum1 += a1[i].getDistance(a2[i]);
                sum1 += a2[i].getDistance(a3[i]);
                sum1 += a3[i].getDistance(a4[i]);

                sum2 += a1[i].getLength();
                sum2 += a2[i].getLength();
                sum2 += a3[i].getLength();
                sum2 += a4[i].getLength();
            }

            sum1 /= a1.size() * 3;
            sum2 /= a1.size() * 4;

            return sum2 == 0 ? 0 : sum1 / sum2;
        }

        /**
         * Calculate the maximum difference of any corresponding pair between a1, a2, a3 and a4.
         */
        double maximumDifference(const std::vector<Pt>& a1, const std::vector<Pt>& a2,
                                 const std::vector<Pt>& a3, const std::vector<Pt>& a4)
        {
            double max = 0;
            Assert(a1.size() == a2.size() && a2.size() == a3.size() && a3.size() == a4.size());

            for (int i = 0; i < (int)a1.size(); i++) {
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
            Assert(a.size() == c.size());
            Assert(pts.size() == c.size());

            for (int i = 0; i < (int)pts.size(); i++)
                pts[i].assign(c[i]).multiply(b).add(a[i]);
        }

        /**
         * pts += a * b
         */
        void incrementWithMultiplied(std::vector<Pt>& pts, double a, const std::vector<Pt>& b)
        {
            Pt pt;
            Assert(pts.size() == b.size());

            for (int i = 0; i < (int)pts.size(); i++) {
                pt.assign(b[i]).multiply(a);
                pts[i].add(pt);
            }
        }

        /**
         * pts = a + b
         */
        void add(std::vector<Pt>& pts, const std::vector<Pt>& a, const std::vector<Pt>& b)
        {
            Assert(a.size() == b.size());

            for (int i = 0; i < (int)pts.size(); i++)
                pts[i].assign(a[i]).add(b[i]);
        }

        /**
         * pts += a
         */
        void increment(std::vector<Pt>& pts, const std::vector<Pt>& a)
        {
            Assert(pts.size() == a.size());

            for (int i = 0; i < (int)pts.size(); i++)
                pts[i].add(a[i]);
        }

        /**
         * pts *= a
         */
        void multiply(std::vector<Pt>& pts, double a)
        {
            for (int i = 0; i < (int)pts.size(); i++)
                pts[i].multiply(a);
        }

        /**
         * an = b[pn, vn]
         */
        void a(std::vector<Pt>& an, const std::vector<Pt>& pn, const std::vector<Pt>& vn);

        /**
         * Convert measured ticks to milliseconds.
         */
        double ticksToMilliseconds(long ticks) {
            return (double)ticks / CLOCKS_PER_SEC * 1000;
        }
};

}  // namespace layout
}  // namespace omnetpp

#endif
