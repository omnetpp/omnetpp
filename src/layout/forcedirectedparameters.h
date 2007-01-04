//=========================================================================
//  FORCEDIRECTEDPARAMETERS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FORCEDIRECTEDPARAMETERS_H_
#define __FORCEDIRECTEDPARAMETERS_H_

#include <math.h>
#include "geometry.h"
#include "forcedirectedparametersbase.h"
#include "forcedirectedembedding.h"

class Body : public IBody {
    protected:
        Variable *variable;

        double mass;
    
        double charge;
    
        Rs size;

    private:
        void constructor(Variable *variable, Rs& size) {
            constructor(variable, 10, 1, size);
        }

        void constructor(Variable *variable, double mass, double charge, Rs& size) {
            this->variable = variable;
            this->mass = mass;
            this->charge = charge;
            this->size = size;

            variable->addMass(mass);
        }

    public:
        Body(Variable *variable) {
            constructor(variable, Rs(10, 10));
        }

        Body(Variable *variable, Rs& size) {
            constructor(variable, size);
        }

        Body(Variable *variable, double mass, double charge, Rs& size) {
            constructor(variable, mass, charge, size);
        }

        virtual const char *getClassName() {
            return "Body";
        }

        virtual Pt getPosition() {
            return variable->getPosition();
        }

        virtual Variable *getVariable() {
            return variable;
        }

        virtual double getMass() {
            return mass;
        }
        
        virtual double getCharge() {
            return charge;
        }

        virtual Rs& getSize() {
            return size;
        }
};

class RelativelyPositionedBody : public Body {
    protected:
        Pt relativePosition;

        IPositioned *anchor;

    private:
        void constructor(IPositioned *anchor, Pt& relativePosition) {
            this->anchor = anchor;
            this->relativePosition = relativePosition;
        }

    public:
        RelativelyPositionedBody(Variable *variable, Pt& relativePosition) : Body(variable) {
            constructor(variable, relativePosition);
        }

        RelativelyPositionedBody(Variable *variable, Pt& relativePosition, Rs& size) : Body(variable) {
            constructor(variable, relativePosition);
        }

        RelativelyPositionedBody(Variable *variable, IPositioned *anchor, Pt& relativePosition) : Body(variable) {
            constructor(anchor, relativePosition);
        }

        virtual const char *getClassName() {
            return "RelativelyPositionedBody";
        }

        virtual Pt getPosition() {
            return Pt(anchor->getPosition()).add(relativePosition);
        }
};

class WallBody : public Body {
    public:
        WallBody(bool horizontal, double position) : Body(new Variable(Pt(horizontal ? NaN : position, horizontal ? position : NaN, NaN))) {
            if (horizontal)
                size = Rs(POSITIVE_INFINITY, 0);
            else
                size = Rs(0, POSITIVE_INFINITY);
        }

        ~WallBody() {
            delete variable;
        }

        virtual const char *getClassName() {
            return "WallBody";
        }
};

class IElectricRepeal : public IForceProvider {
    public:
        virtual IBody *getCharge1() = 0;
    
        virtual IBody *getCharge2() = 0;
};

class AbstractElectricRepeal : public IElectricRepeal {
    protected:
        IBody *charge1;
    
        IBody *charge2;

        bool slippery;

    public:
        AbstractElectricRepeal(IBody *charge1, IBody *charge2, bool slippery) {
            this->charge1 = charge1;
            this->charge2 = charge2;
            this->slippery = slippery;
        }

        virtual IBody *getCharge1() {
            return charge1;
        }
        
        virtual IBody *getCharge2() {
            return charge2;
        }
        
        virtual Pt getDistanceAndVector(double &distance) = 0;

        virtual double getDistance() {
            double distance;
            getDistanceAndVector(distance);
            return distance;
        }

        virtual void applyForces() {
            double distance;
            Pt vector = getDistanceAndVector(distance);
            ASSERT(distance >= 0);

            if (!vector.isZero()) {
                double power;

                if (distance == 0)
                    power = maxForce;
                else
                    power = getValidForce(embedding->electricRepealCoefficient * charge1->getCharge() * charge2->getCharge() / distance / distance);

                charge1->getVariable()->addForce(vector, +power, embedding->inspected);
                charge2->getVariable()->addForce(vector, -power, embedding->inspected);
            }
        }

        virtual double getPotentialEnergy() {
            return embedding->electricRepealCoefficient * charge1->getCharge() * charge2->getCharge() / getDistance();
        }
};

class ElectricRepeal : public AbstractElectricRepeal {
    public:
        ElectricRepeal(IBody *charge1, IBody *charge2, bool slippery = false) : AbstractElectricRepeal(charge1, charge2, slippery) {
        }

        virtual Pt getDistanceAndVector(double &distance) {
            if (slippery)
                return getSlipperyDistanceAndVector(charge1, charge2, distance);
            else
                return getStandardDistanceAndVector(charge1, charge2, distance);
        }

        virtual const char *getClassName() {
            return "ElectricRepeal";
        }
};

class VerticalElectricRepeal : public AbstractElectricRepeal {
    public:
        VerticalElectricRepeal(IBody *charge1, IBody *charge2) : AbstractElectricRepeal(charge1, charge2, false) {
        }

        virtual Pt getDistanceAndVector(double &distance) {
            return getStandardVerticalDistanceAndVector(charge1, charge2, distance);
       }

        virtual const char *getClassName() {
            return "VerticalElectricRepeal";
        }
};

class HorizontalElectricRepeal : public AbstractElectricRepeal {
    public:
        HorizontalElectricRepeal(IBody *charge1, IBody *charge2) : AbstractElectricRepeal(charge1, charge2, false) {
        }

        virtual Pt getDistanceAndVector(double &distance) {
            return getStandardHorizontalDistanceAndVector(charge1, charge2, distance);
        }

        virtual const char *getClassName() {
            return "HorizontalElectricRepeal";
        }
};

class ISpring : public IForceProvider {
    public:
        virtual double getSpringCoefficient() = 0;
        
        virtual double getReposeLength() = 0;
        
        virtual IBody *getBody1() = 0;
        
        virtual IBody *getBody2() = 0;
};

class AbstractSpring : public ISpring {
    protected:
        IBody *body1;
        
        IBody *body2;

        double springCoefficient;

        double reposeLength;

        bool slippery;

    private:
        void constructor(IBody *body1, IBody *body2, double springCoefficient, double reposeLength, bool slippery) {
            this->body1 = body1;
            this->body2 = body2;
            this->springCoefficient = springCoefficient;
            this->reposeLength = reposeLength;
            this->slippery = slippery;
        }

    public:
        AbstractSpring(IBody *body1, IBody *body2) {
            constructor(body1, body2, 1, 0, false);
        }
        
        AbstractSpring(IBody *body1, IBody *body2, double springCoefficient, double reposeLength, bool slippery) {
            constructor(body1, body2, springCoefficient, reposeLength, slippery);
        }

        virtual double getSpringCoefficient() {
            return springCoefficient;
        }

        double getReposeLength() {
            return reposeLength;
        }

        virtual IBody *getBody1() {
            return body1;
        }

        virtual IBody *getBody2() {
            return body2;
        }
        
        virtual Pt getDistanceAndVector(double &distance) = 0;

        virtual double getDistance() {
            double distance;
            getDistanceAndVector(distance);
            return distance;
        }

        virtual void applyForces() {
            double distance;
            Pt vector = getDistanceAndVector(distance);
            ASSERT(distance >= 0);
            double expansion = distance - reposeLength;

            if (expansion != 0 && !vector.isZero()) {
                double power = getValidSignedForce(embedding->springCoefficient * getSpringCoefficient() * expansion);

                if (body1)
                    body1->getVariable()->addForce(vector, -power, embedding->inspected);

                if (body2)
                    body2->getVariable()->addForce(vector, +power, embedding->inspected);
            }
        }

        virtual double getPotentialEnergy() {
            double expansion = getDistance() - reposeLength;
            return embedding->springCoefficient * getSpringCoefficient() * expansion * expansion / 2; 
        }
};

class Spring : public AbstractSpring {
    public:
        Spring(IBody *body1, IBody *body2) : AbstractSpring(body1, body2) {
        }
        
        Spring(IBody *body1, IBody *body2, double springCoefficient, double reposeLength, bool slippery = false) : AbstractSpring(body1, body2, springCoefficient, reposeLength, slippery) {
        }

        virtual Pt getDistanceAndVector(double &distance) {
            if (slippery)
                return getSlipperyDistanceAndVector(body1, body2, distance);
            else
                return getStandardDistanceAndVector(body1, body2, distance);
        }

        virtual const char *getClassName() {
            return "Spring";
        }
};

class VerticalSpring : public AbstractSpring {
    public:
        VerticalSpring(IBody *body1, IBody *body2) : AbstractSpring(body1, body2) {
        }
        
        VerticalSpring(IBody *body1, IBody *body2, double springCoefficient, double reposeLength) : AbstractSpring(body1, body2, springCoefficient, reposeLength, false){
        }

        virtual Pt getDistanceAndVector(double &distance) {
            return getStandardVerticalDistanceAndVector(body1, body2, distance);
        }

        virtual const char *getClassName() {
            return "VerticalSpring";
        }
};

class HorizonalSpring : public AbstractSpring {
    public:
        HorizonalSpring(IBody *body1, IBody *body2) : AbstractSpring(body1, body2) {
        }
        
        HorizonalSpring(IBody *body1, IBody *body2, double springCoefficient, double reposeLength) : AbstractSpring(body1, body2, springCoefficient, reposeLength, false) {
        }

        virtual Pt getDistanceAndVector(double &distance) {
            return getStandardHorizontalDistanceAndVector(body1, body2, distance);
        }

        virtual const char *getClassName() {
            return "HorizonalSpring";
        }
};

class BasePlaneSpring : public AbstractSpring {
    public:
        BasePlaneSpring(IBody *body) : AbstractSpring(body, NULL) {
        }
        
        BasePlaneSpring(IBody *body, double springCoefficient, double reposeLength) : AbstractSpring(body, NULL, springCoefficient, reposeLength, false) {
        }

        virtual double getSpringCoefficient() {
            return springCoefficient * embedding->elapsedCalculationTime / embedding->maxCalculationTime;
        }

        virtual Pt getDistanceAndVector(double &distance) {
            Pt vector = body1->getPosition();
            vector.x = 0;
            vector.y = 0;
            distance = std::abs(vector.z);
            return vector;
        }

        virtual const char *getClassName() {
            return "BasePlaneSpring";
        }
};

class AbstractVelocityBasedForceProvider : public IForceProvider {
    public:
        virtual double getPower(Variable *variable, double vlen) = 0;

        virtual void applyForces() {
            const std::vector<Variable *>& variables = embedding->getVariables();

            for (std::vector<Variable *>::const_iterator it = variables.begin(); it != variables.end(); it++) {
                Variable *variable = *it;
                Pt vector(variable->getVelocity());
                vector.reverse();
                double vlen = vector.getLength();

                if (!vector.isZero()) {
                    double power = getPower(variable, vlen);

                    variable->addForce(vector, power, embedding->inspected);
                }
            }
        }

        virtual double getPotentialEnergy() {
            return 0;
        }
};

class Friction : public AbstractVelocityBasedForceProvider {
    public:
        virtual double getPower(Variable *variable, double vlen) {
            return std::max(embedding->updatedFrictionCoefficient * 0.01, vlen / embedding->updatedTimeStep) * variable->getMass();
        }

        virtual const char *getClassName() {
            return "Friction";
        }
};

class Drag : public AbstractVelocityBasedForceProvider {
    public:
        virtual double getPower(Variable *variable, double vlen) {
            return embedding->updatedFrictionCoefficient * vlen;
        }

        virtual const char *getClassName() {
            return "Drag";
        }
};

class BodyConstraint : public IForceProvider {
    protected:
        IBody *body;

    public:
        BodyConstraint(IBody *body) {
            this->body = body;
        }

        virtual IBody *getBody() {
            return body;
        }

        virtual void applyForces() = 0;

        virtual double getPotentialEnergy() {
            // TODO:
            return 0;
        }
};

class PointConstraint : public BodyConstraint {
    protected:
        Pt constraint;
    
        double coefficient;
    
    public:
        PointConstraint(IBody *body, double coefficient, Pt constraint) : BodyConstraint(body) {
            this->coefficient = coefficient;
            this->constraint = constraint;
        }

        virtual void applyForces() {
            Pt vector(constraint);
            vector.subtract(body->getPosition());
            double power = coefficient * vector.getLength();

            body->getVariable()->addForce(vector, power, embedding->inspected);
        }

        virtual const char *getClassName() {
            return "PointConstraint";
        }
};

class LineConstraint : public BodyConstraint {
    protected:
        Ln constraint;
    
        double coefficient;

    public:
        LineConstraint(IBody *body, double coefficient, Ln constraint) : BodyConstraint(body) {
            this->coefficient = coefficient;
            this->constraint = constraint;
        }

        virtual void applyForces() {
            Pt position = body->getPosition();
            Pt vector(constraint.getClosestPoint(position));
            vector.subtract(position);
            double power = coefficient * vector.getLength();

            body->getVariable()->addForce(vector, power, embedding->inspected);
        }

        virtual const char *getClassName() {
            return "LineConstraint";
        }
};

class CircleConstraint : public BodyConstraint {
    protected:
        Cc constraint;
    
    protected:
        double coefficient;
    
    public:
        CircleConstraint(IBody *body, double coefficient, Cc constraint) : BodyConstraint(body) {
            this->coefficient = coefficient;
            this->constraint = constraint;
        }

        virtual void applyForces() {
            Pt position = body->getPosition();
            Pt vector(constraint.origin);
            vector.subtract(position);
            double power = coefficient * (constraint.origin.getDistance(position) - constraint.radius);

            body->getVariable()->addForce(vector, power, embedding->inspected);
        }

        virtual const char *getClassName() {
            return "CircleConstraint";
        }
};

#endif
