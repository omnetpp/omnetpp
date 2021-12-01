//=========================================================================
//  FORCEDIRECTEDPARAMETERSBASE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_LAYOUT_FORCEDIRECTEDPARAMETERSBASE_H
#define __OMNETPP_LAYOUT_FORCEDIRECTEDPARAMETERSBASE_H

#include <cmath>
#include "geometry.h"

namespace omnetpp {
namespace layout {

class ForceDirectedEmbedding;

/**
 * Various parameters driving the algorithm.
 */
struct LAYOUT_API ForceDirectedParameters
{
    /**
     * For bodies not initialized with a particular size.
     */
    Rs defaultBodySize;

    /**
     * For bodies not initialized with a particular mass.
     */
    double defaultBodyMass;

    /**
     * For bodies not initialized with a particular charge.
     */
    double defaultBodyCharge;

    /**
     * For springs not initialized with a particular coefficient.
     */
    double defaultSpringCoefficient;

    /**
     * Default spring repose length.
     */
    double defaultSpringReposeLength;

    /**
     * Electric repulsion force coefficient.
     */
    double electricRepulsionCoefficient;

    /**
     * For electric repulsions not initialized with a linearity distance.
     */
    double defaultElectricRepulsionLinearityDistance;

    /**
     * For electric repulsions not initialized with a particular max distance.
     */
    double defaultElectricRepulsionMaxDistance;

    /**
     * Friction reduces the energy of the system. The friction force points in the opposite direction of the current velocity.
     */
    double frictionCoefficient;

    /**
     * For force providers not initialized with a particular slippery flag.
     */
    bool defaultSlippery;

    /**
     * For force providers not initialized with a particular point like distance flag.
     */
    bool defaultPointLikeDistance;

    /**
     * The default time step used when solution starts.
     */
    double timeStep;

    /**
     * Lower limit for the updated time step.
     */
    double minTimeStep;

    /**
     * Lower limit for the updated time step.
     */
    double maxTimeStep;

    /**
     * Multiplier used to update the time step.
     */
    double timeStepMultiplier;

    /**
     * Relative lwer limit of acceleration approximation difference (between a1, a2, a3 and a4 in RK-4).
     * During updating the time step this is the lower limit to accept the current time step.
     */
    double minAccelerationError;

    /**
     * Relative upper limit of acceleration approximation difference (between a1, a2, a3 and a4 in RK-4).
     */
    double maxAccelerationError;

    /**
     * Velocity limit during the solution.
     * When all bodies has lower velocity than this limit then the algorithm may be stopped.
     */
    double velocityRelaxLimit;

    /**
     * Acceleration limit during the solution.
     * When all bodies has lower acceleration than this limit then the algorithm may be stopped.
     */
    double accelerationRelaxLimit;

    /**
     * For force providers not initialized with a particular maximum force.
     */
    double defaultMaxForce;

    /**
     * Maximim velocity that a body may have.
     */
    double maxVelocity;

    /**
     * Maximum number of calculation cycles to run.
     */
    int maxCycle;

    /**
     * Maximum time to be spent on the calculation in milliseconds.
     * The algorithm will return after this time has been elapsed.
     */
    double maxCalculationTime;
};

/**
 * A variable used in the differential equation.
 * The actual value of the variable is the position, the first derivative is the velocity
 * and the second derivative is the acceleration.
 */
class LAYOUT_API Variable {
    protected:
        /**
         * Value of the variable.
         */
        Pt position;

        /**
         * First derivative.
         */
        Pt velocity;

        /**
         * Second derivative.
         */
        Pt acceleration;

        /**
         * Total applied force.
         */
        Pt force;

        /**
         * The list of all applied forces for debug purposes.
         */
        std::vector<Pt> forces;

        /**
         * Total mass of bodies appling forces to this variable.
         */
        double mass;

        ForceDirectedEmbedding *embedding;

    private:
        void constructor(const Pt& position, const Pt& velocity) {
            this->position = position;
            this->velocity = velocity;

            mass = 0;
            force = Pt::getZero();
        }

    public:
        Variable(const Pt& position) {
            constructor(position, Pt::getZero());
        }

        Variable(const Pt& position, const Pt& velocity) {
            constructor(position, velocity);
        }
        virtual ~Variable() {}

        virtual void reinitialize() {};

        void setForceDirectedEmbedding(ForceDirectedEmbedding *embedding) {
            this->embedding = embedding;
        }

        const Pt& getPosition() {
            return position;
        }

        virtual void assignPosition(const Pt& position) {
            this->position.assign(position);
        }

        const Pt& getVelocity() {
            return velocity;
        }

        virtual void assignVelocity(const Pt& velocity) {
            this->velocity.assign(velocity);
        }

        virtual const Pt& getAcceleration() {
            return acceleration.assign(force).divide(mass);
        }

        double getKineticEnergy() {
            double vlen = velocity.getLength();
            return 0.5 * mass * vlen * vlen;
        }

        void resetForce() {
            force = Pt::getZero();
        }

        double getMass() {
            return mass;
        }

        void setMass(double mass) {
            this->mass = mass;
        }

        Pt getForce() {
            return force;
        }

        void addForce(const Pt& f) {
            if (f.isFullySpecified()) {
                force.add(f);

// TODO:
//                if (embedding->inspected)
//                    forces.push_back(f);
            }
        }

        void subtractForce(const Pt& f) {
            if (f.isFullySpecified()) {
                force.subtract(f);

// TODO:
//                if (embedding->inspected)
//                    forces.push_back(f.multiply(-1));
            }
        }

        void resetForces() {
            forces.clear();
        }

        const std::vector<Pt>& getForces() {
            return forces;
        }
};

/**
 * A variable which has fix x and y coordinates but still has a free z coordinate.
 */
class LAYOUT_API PointConstrainedVariable : public Variable {
    public:
        PointConstrainedVariable(Pt position) : Variable(position) {
        }

        virtual void assignPosition(const Pt& position) override {
            this->position.z = position.z;
        }

        virtual void assignVelocity(const Pt& velocity) override {
            this->velocity.z = velocity.z;
        }

        virtual const Pt& getAcceleration() override {
            return acceleration.assign(0, 0, force.z).divide(mass);
        }
};

/**
 * Interface class for bodies.
 */
class LAYOUT_API IBody {
    protected:
        ForceDirectedEmbedding *embedding;

    public:
        virtual ~IBody() {}

        virtual void reinitialize() {};

        virtual void setForceDirectedEmbedding(ForceDirectedEmbedding *embedding) {
            this->embedding = embedding;
        }

        virtual const Pt& getPosition() = 0;

        virtual const char *getClassName() = 0;

        virtual const Rs& getSize() = 0;

        virtual double getMass() = 0;

        virtual double getCharge() = 0;

        virtual Variable *getVariable() = 0;

        Pt getLeftTop() {
            return Pt(getLeft(), getTop(), getPosition().z);
        }

        double getLeft() {
            return getPosition().x - getSize().width / 2;
        }

        double getRight() {
            return getPosition().x + getSize().width / 2;
        }

        double getTop() {
            return getPosition().y - getSize().height / 2;
        }

        double getBottom() {
            return getPosition().y + getSize().height / 2;
        }
};

/**
 * Interface class used by the force directed embedding to generate forces among bodies.
 */
class LAYOUT_API IForceProvider {
    protected:
        ForceDirectedEmbedding *embedding;

    public:
        virtual ~IForceProvider() {}

        virtual void reinitialize() {};

        virtual void setForceDirectedEmbedding(ForceDirectedEmbedding *embedding) {
            this->embedding = embedding;
        }

        virtual const char *getClassName() = 0;

        virtual void applyForces() = 0;

        virtual double getPotentialEnergy() = 0;
};

}  // namespace layout
}  // namespace omnetpp


#endif
