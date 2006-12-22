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

        double getKineticEnergy() {
            double vlen = velocity.getLength();
            return 0.5 * mass * vlen * vlen;
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

class IBody : public IPositioned {
    public:
        virtual const char *getClassName() = 0;

        virtual Rs& getSize() = 0;

        virtual double getMass() = 0;

        virtual double getCharge() = 0;

        virtual Variable *getVariable() = 0;
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

        Pt getStandardDistanceAndVector(IBody *body1, IBody *body2, double &distance) {
            Pt vector = Pt(body1->getPosition()).subtract(body2->getPosition());
            distance = vector.getLength();
            return vector;
        }

        Pt getStandardHorizontalDistanceAndVector(IBody *body1, IBody *body2, double &distance) {
            distance = body1->getPosition().x - body2->getPosition().x;
            Pt vector = Pt(distance, 0);
            distance = std::abs(distance);
            return vector;
        }

        Pt getStandardVerticalDistanceAndVector(IBody *body1, IBody *body2, double &distance) {
            distance = body1->getPosition().y - body2->getPosition().y;
            Pt vector = Pt(0, distance);
            distance = std::abs(distance);
            return vector;
        }

        Pt getSlipperyDistanceAndVector(IBody *body1, IBody *body2, double &distance) {
            Rc rc1 = Rc::getRcFromCenterSize(body1->getPosition(), body1->getSize());
            Rc rc2 = Rc::getRcFromCenterSize(body2->getPosition(), body2->getSize());
            Ln ln = rc1.getDistance(rc2, distance);
            Pt vector = ln.begin;
            vector.subtract(ln.end);
            vector.setNaNToZero();
            return vector;
        }

        virtual const char *getClassName() = 0;

        virtual void applyForces(const ForceDirectedEmbedding& embedding) = 0;

        virtual double getPotentialEnergy(const ForceDirectedEmbedding& embedding) = 0;
};

#endif
