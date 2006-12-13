//=========================================================================
//  FORCEDIRECTEDPARAMETERSBASE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FORCEDIRECTEDPARAMETERSBASE_H_
#define __FORCEDIRECTEDPARAMETERSBASE_H_

#include <math.h>
#include "geometry.h"

class Variable;
class ForceDirectedEmbedding;

class IPositioned {
    public:
        virtual Pt getPosition() = 0;
};

class Variable : public IPositioned {
    protected:
	    Pt position;

	    Pt velocity;

        Pt acceleration;

        Pt force;

	    std::vector<Pt> forces;
	
	    double mass;

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

	    virtual Pt getPosition() {
		    return position;
	    }

	    void assignPosition(const Pt& position) {
		    this->position.assign(position);
	    }

	    Pt& getVelocity() {
		    return velocity;
	    }

	    void assignVelocity(const Pt& velocity) {
		    this->velocity.assign(velocity);
	    }

	    Pt& getAcceleration() {
		    return acceleration.assign(force).divide(mass);
	    }

	    void resetForce() {
            force = Pt::getZero();
	    }

	    double getMass() {
		    return mass;
	    }
    	
	    void addMass(double mass) {
		    this->mass += mass;
	    }

	    void addForce(const Pt& vector, double power, bool inspected = false) {
		    Pt f(vector);
            f.normalize().multiply(power);

		    ASSERT(f.isFullySpecified());

		    force.add(f);

            if (inspected)
    		    forces.push_back(f);
	    }

	    void resetForces() {
		    forces.clear();
	    }
    	
	    const std::vector<Pt>& getForces() {
		    return forces;
	    }
};

class IForceProvider {
    protected:
        double maxForce;

    public:
        IForceProvider() {
            maxForce = 1000;
        }

	    double getMaxForce() {
		    return maxForce;
	    }
    	
	    double getValidForce(double force) {
		    ASSERT(force >= 0);
            return std::min(maxForce, force);
	    }
    	
	    double getValidSignedForce(double force) {
            if (force < 0)
                return -getValidForce(fabs(force));
            else
                return getValidForce(fabs(force));
	    }

        virtual const char *getClassName() = 0;

        virtual void applyForces(const ForceDirectedEmbedding& embedding) = 0;
};

class IBody : public IPositioned {
    public:
        virtual const char *getClassName() = 0;

        virtual Rs& getSize() = 0;

        virtual double getMass() = 0;

        virtual double getCharge() = 0;

        virtual Variable *getVariable() = 0;
};

#endif
