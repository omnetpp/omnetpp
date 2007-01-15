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

/**
 * Represents a freely positioned two dimensional body with a finite mass and charge.
 * The actual position of the body is stored in the corresponding variable. This allows
 * having multiple bodies sharing the same variable which is important for relatively positioned
 * bodies.
 */
class Body : public IBody {
    protected:
        Variable *variable;

        double mass;
    
        double charge;
    
        Rs size;

    private:
        void constructor(Variable *variable, double mass, double charge, Rs& size) {
            this->variable = variable;
            this->mass = mass;
            this->charge = charge;
            this->size = size;

            variable->addMass(mass);
        }

    public:
        Body(Variable *variable) {
            constructor(variable, -1, -1, Rs::getNil());
        }

        Body(Variable *variable, Rs& size) {
            constructor(variable, -1, -1, size);
        }

        Body(Variable *variable, double mass, double charge, Rs& size) {
            constructor(variable, mass, charge, size);
        }

        virtual void setForceDirectedEmbedding(ForceDirectedEmbedding *embedding) {
            IBody::setForceDirectedEmbedding(embedding);

            if (mass == -1)
                mass = embedding->parameters.defaultBodyMass;

            if (charge == -1)
                charge = embedding->parameters.defaultBodyCharge;

            if (size.isNil())
                size = embedding->parameters.defaultBodySize;
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

/**
 * A body which is relatively positioned to something. The anchor may be a variable or another body.
 */
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

/**
 * Represents a body which has infinite size in one dimension and zero size in the other dimension.
 */
class WallBody : public Body {
    private:
        bool horizontal;

    public:
        WallBody(bool horizontal, double position) : Body(new Variable(Pt(horizontal ? NaN : position, horizontal ? position : NaN, NaN))) {
            this->horizontal = horizontal;

            if (horizontal)
                size = Rs(POSITIVE_INFINITY, 0);
            else
                size = Rs(0, POSITIVE_INFINITY);
        }

        ~WallBody() {
            delete variable;
        }

        void setPosition(double position) {
            getVariable()->assignPosition(Pt(horizontal ? NaN : position, horizontal ? position : NaN, NaN));
        }

        virtual const char *getClassName() {
            return "WallBody";
        }
};

/**
 * A repulsive force which decreases in a quadratic way propoportional to the distance of the bodies.
 */
class IElectricRepulsion : public IForceProvider {
    public:
        virtual IBody *getCharge1() = 0;
    
        virtual IBody *getCharge2() = 0;
};

/**
 * Abstract base class for electric repulsive forces.
 */
class AbstractElectricRepulsion : public IElectricRepulsion {
    protected:
        IBody *charge1;
    
        IBody *charge2;

        bool slippery;

        // TODO: add maximum distance parameter and do not process repulsions exceeding that

    public:
        AbstractElectricRepulsion(IBody *charge1, IBody *charge2, bool slippery) {
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

            // TODO: linearly decrease power from maxDistance1 to maxDistance2 (set both to POSITIVE_INFINITY by default)
            // TODO: this will make unconnected components look better

            double power;
            if (distance == 0)
                power = maxForce;
            else
                power = getValidForce(embedding->parameters.electricRepulsionCoefficient * charge1->getCharge() * charge2->getCharge() / distance / distance);

            charge1->getVariable()->addForce(vector, +power, embedding->inspected);
            charge2->getVariable()->addForce(vector, -power, embedding->inspected);
        }

        virtual double getPotentialEnergy() {
            return embedding->parameters.electricRepulsionCoefficient * charge1->getCharge() * charge2->getCharge() / getDistance();
        }
};

/**
 * Standard electric repulsion. Slippery mode means the distance will be calculated between the two
 * rectangles instead of the two center points.
 */
class ElectricRepulsion : public AbstractElectricRepulsion {
    public:
        ElectricRepulsion(IBody *charge1, IBody *charge2, bool slippery = false) : AbstractElectricRepulsion(charge1, charge2, slippery) {
        }

        virtual Pt getDistanceAndVector(double &distance) {
            if (slippery)
                return getSlipperyDistanceAndVector(charge1, charge2, distance);
            else
                return getStandardDistanceAndVector(charge1, charge2, distance);
        }

        virtual const char *getClassName() {
            return "ElectricRepulsion";
        }
};

class VerticalElectricRepulsion : public AbstractElectricRepulsion {
    public:
        VerticalElectricRepulsion(IBody *charge1, IBody *charge2) : AbstractElectricRepulsion(charge1, charge2, false) {
        }

        virtual Pt getDistanceAndVector(double &distance) {
            return getStandardVerticalDistanceAndVector(charge1, charge2, distance);
       }

        virtual const char *getClassName() {
            return "VerticalElectricRepulsion";
        }
};

class HorizontalElectricRepulsion : public AbstractElectricRepulsion {
    public:
        HorizontalElectricRepulsion(IBody *charge1, IBody *charge2) : AbstractElectricRepulsion(charge1, charge2, false) {
        }

        virtual Pt getDistanceAndVector(double &distance) {
            return getStandardHorizontalDistanceAndVector(charge1, charge2, distance);
        }

        virtual const char *getClassName() {
            return "HorizontalElectricRepulsion";
        }
};

/**
 * An attractive force which increases in a linear way proportional to the distance of the bodies.
 */
class ISpring : public IForceProvider {
    public:
        virtual double getSpringCoefficient() = 0;
        
        virtual double getReposeLength() = 0;
        
        virtual IBody *getBody1() = 0;
        
        virtual IBody *getBody2() = 0;
};

/**
 * Abstract base class for spring attractive forces.
 */
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
            constructor(body1, body2, -1, -1, false);
        }
        
        AbstractSpring(IBody *body1, IBody *body2, double springCoefficient, double reposeLength, bool slippery) {
            constructor(body1, body2, springCoefficient, reposeLength, slippery);
        }

        virtual void setForceDirectedEmbedding(ForceDirectedEmbedding *embedding) {
            IForceProvider::setForceDirectedEmbedding(embedding);

            if (springCoefficient == -1)
                springCoefficient = embedding->parameters.defaultSpringCoefficient;

            if (reposeLength == -1)
                reposeLength = embedding->parameters.defaultSpringReposeLength;
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
            double power = getValidSignedForce(getSpringCoefficient() * expansion);

            if (body1)
                body1->getVariable()->addForce(vector, -power, embedding->inspected);

            if (body2)
                body2->getVariable()->addForce(vector, +power, embedding->inspected);
        }

        virtual double getPotentialEnergy() {
            double expansion = getDistance() - reposeLength;
            return getSpringCoefficient() * expansion * expansion / 2; 
        }
};

/**
 * Standard spring attraction. Slippery mode means the distance will be calculated between the two
 * rectangles instead of the two center points.
 */
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

class ShortestSpring : public IForceProvider {
    private:
        std::vector<AbstractSpring *> springs;

    public:
        ShortestSpring(std::vector<AbstractSpring *> springs) {
            this->springs = springs;
        }

        ~ShortestSpring() {
            for (std::vector<AbstractSpring *>::iterator it = springs.begin(); it != springs.end(); it++)
                delete *it;
        }

        virtual void setForceDirectedEmbedding(ForceDirectedEmbedding *embedding) {
            IForceProvider::setForceDirectedEmbedding(embedding);
            for (std::vector<AbstractSpring *>::iterator it = springs.begin(); it != springs.end(); it++) {
                AbstractSpring *spring = *it;
                spring->setForceDirectedEmbedding(embedding);
            }
        }

        virtual void applyForces() {
            AbstractSpring *shortestSpring;
            double shortestExpansion = POSITIVE_INFINITY;

            for (std::vector<AbstractSpring *>::iterator it = springs.begin(); it != springs.end(); it++) {
                AbstractSpring *spring = *it;
                double distance;
                Pt vector = spring->getDistanceAndVector(distance);
                double expansion = abs(distance - spring->getReposeLength());

                if (expansion < shortestExpansion) {
                    shortestExpansion = expansion;
                    shortestSpring = spring;
                }
            }

            shortestSpring->applyForces();
        }

        virtual double getPotentialEnergy() {
            // TODO:
            return 0;
        }

        virtual const char *getClassName() {
            return "ShortestSpring";
        }
};

/**
 * This constraint is like a spring between a body and the base plane (z = 0).
 * The spring coefficient increases by time proportional to the relax factor.
 * This allows nodes to move in the third dimension in the beginning of the calculation
 * while it forces to them to move to the base plane as time goes by. 
 */
class BasePlaneSpring : public AbstractSpring {
    public:
        BasePlaneSpring(IBody *body) : AbstractSpring(body, NULL) {
        }
        
        BasePlaneSpring(IBody *body, double springCoefficient, double reposeLength) : AbstractSpring(body, NULL, springCoefficient, reposeLength, false) {
        }

        virtual double getSpringCoefficient() {
            return springCoefficient * embedding->relaxFactor;
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

/**
 * Base class for velocity based kinetic energy reducers.
 */
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
                double power = getPower(variable, vlen);

                variable->addForce(vector, power, embedding->inspected);
            }
        }

        virtual double getPotentialEnergy() {
            return 0;
        }
};

/**
 * Reduces kinetic energy during the calculation.
 */
class Friction : public AbstractVelocityBasedForceProvider {
    public:
        virtual double getPower(Variable *variable, double vlen) {
            return std::max(embedding->parameters.frictionCoefficient * 0.01, vlen / embedding->updatedTimeStep) * variable->getMass();
        }

        virtual const char *getClassName() {
            return "Friction";
        }
};

/**
 * Reduces kinetic energy during the calculation.
 */
class Drag : public AbstractVelocityBasedForceProvider {
    public:
        virtual double getPower(Variable *variable, double vlen) {
            return embedding->parameters.frictionCoefficient * vlen;
        }

        virtual const char *getClassName() {
            return "Drag";
        }
};

/**
 * Abstract base class for position constraints applied to a single body. These constraints are
 * more expensive to calculate with than the constrained variables and the final position will not
 * exactly satisfy the constraint.
 */
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

/**
 * This constraint is like a spring between a body and a fixed point.
 */
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

/**
 * This constraint is like a spring between a body and a fixed line, the end connected to the line is slippery.
 */
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

/**
 * This constraint is like a spring between a body and a fixed circle, the end connected to the circle is slippery.
 */
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
