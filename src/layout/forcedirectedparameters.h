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
        void constructor(Variable *variable, double mass, double charge, const Rs& size) {
            this->variable = variable;
            this->mass = mass;
            this->charge = charge;
            this->size = size;
        }

    public:
        Body(Variable *variable) {
            constructor(variable, -1, -1, Rs::getNil());
        }

        Body(Variable *variable, const Rs& size) {
            constructor(variable, -1, -1, size);
        }

        Body(Variable *variable, double mass, double charge, const Rs& size) {
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

        /**
         * Center of the body.
         */
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
        void constructor(IPositioned *anchor, const Pt& relativePosition) {
            this->anchor = anchor;
            this->relativePosition = relativePosition;
        }

    public:
        RelativelyPositionedBody(Variable *variable, const Pt& relativePosition) : Body(variable) {
            constructor(variable, relativePosition);
        }

        RelativelyPositionedBody(Variable *variable, const Pt& relativePosition, const Rs& size) : Body(variable) {
            constructor(variable, relativePosition);
        }

        RelativelyPositionedBody(Variable *variable, IPositioned *anchor, const Pt& relativePosition) : Body(variable) {
            constructor(anchor, relativePosition);
        }

        virtual const char *getClassName() {
            return "RelativelyPositionedBody";
        }

        /**
         * Center of the body.
         */
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

class AbstractForceProvider : public IForceProvider {
    protected:
        double maxForce;

        int slippery;

        int pointLikeDistance;

    private:
        void constructor(double maxForce, int pointLikeDistance, int slippery) {
            this->maxForce = maxForce;
            this->pointLikeDistance = pointLikeDistance;
            this->slippery = slippery;
        }

    public:
        AbstractForceProvider() {
            constructor(-1, -1, -1);
        }

        AbstractForceProvider(int slippery) {
            constructor(-1, -1, slippery);
        }

        AbstractForceProvider(double maxForce, int slippery) {
            constructor(maxForce, -1, slippery);
        }

        virtual void setForceDirectedEmbedding(ForceDirectedEmbedding *embedding) {
            IForceProvider::setForceDirectedEmbedding(embedding);

            if (maxForce == -1)
                maxForce = embedding->parameters.defaultMaxForce;

            if (slippery == -1)
                slippery = embedding->parameters.defaultSlippery;

            if (pointLikeDistance == -1)
                pointLikeDistance = embedding->parameters.defaultPointLikeDistance;
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

        Pt getDistanceAndVector(IBody *body1, IBody *body2, double &distance) {
            if (slippery)
                return getSlipperyDistanceAndVector(body1, body2, distance);
            else
                return getStandardDistanceAndVector(body1, body2, distance);
        }

        Pt getStandardDistanceAndVector(IBody *body1, IBody *body2, double &distance) {
            Pt pt1 = body1->getPosition();
            Pt pt2 = body2->getPosition();
            Pt vector = Pt(pt1).subtract(pt2);
            distance = vector.getLength();

            if (!pointLikeDistance) {
                Rs rs1 = body1->getSize();
                Rs rs2 = body2->getSize();
                double dx = fabs(pt1.x - pt2.x);
                double dy = fabs(pt1.y - pt2.y);
                double dHalf = distance / 2;
                double d1 = dHalf * std::min(rs1.width / dx, rs1.height / dy);
                double d2 = dHalf * std::min(rs2.width / dx, rs2.height / dy);

                distance = std::max(0.0, distance - d1 - d2);
            }

            return vector;
        }

        Pt getStandardHorizontalDistanceAndVector(IBody *body1, IBody *body2, double &distance) {
            Pt vector = Pt(body1->getPosition()).subtract(body2->getPosition());
            vector.y = 0;
            vector.z = 0;
            distance = vector.getLength();

            if (!pointLikeDistance)
                distance = std::max(0.0, distance - body1->getSize().width - body2->getSize().width);

            return vector;
        }

        Pt getStandardVerticalDistanceAndVector(IBody *body1, IBody *body2, double &distance) {
            Pt vector = Pt(body1->getPosition()).subtract(body2->getPosition());
            vector.x = 0;
            vector.z = 0;
            distance = vector.getLength();

            if (!pointLikeDistance)
                distance = std::max(0.0, distance - body1->getSize().height - body2->getSize().height);

            return vector;
        }

        Pt getSlipperyDistanceAndVector(IBody *body1, IBody *body2, double &distance) {
            Rc rc1 = Rc::getRcFromCenterSize(body1->getPosition(), body1->getSize());
            Rc rc2 = Rc::getRcFromCenterSize(body2->getPosition(), body2->getSize());
            Ln ln = rc1.getBasePlaneProjectionDistance(rc2, distance);
            Pt vector = ln.begin;
            vector.subtract(ln.end);
            vector.setNaNToZero();
            return vector;
        }
};

/**
 * A repulsive force which decreases in a quadratic way propoportional to the distance of the bodies.
 * Abstract base class for electric repulsive forces.
 */
class AbstractElectricRepulsion : public AbstractForceProvider {
    protected:
        IBody *charge1;

        IBody *charge2;

        double linearityDistance;

        double maxDistance;

    private:
        void constructor(IBody *charge1, IBody *charge2, double linearityDistance, double maxDistance) {
            this->charge1 = charge1;
            this->charge2 = charge2;
            this->linearityDistance = linearityDistance;
            this->maxDistance = maxDistance;
        }

    public:
        AbstractElectricRepulsion(IBody *charge1, IBody *charge2) : AbstractForceProvider(-1) {
            constructor(charge1, charge2, -1, -1);
        }

        AbstractElectricRepulsion(IBody *charge1, IBody *charge2, double linearityDistance, double maxDistance, int slippery = -1) : AbstractForceProvider(slippery) {
            constructor(charge1, charge2, linearityDistance, maxDistance);
        }

        virtual void setForceDirectedEmbedding(ForceDirectedEmbedding *embedding) {
            AbstractForceProvider::setForceDirectedEmbedding(embedding);

            if (linearityDistance == -1)
                linearityDistance = embedding->parameters.defaultElectricRepulsionLinearityDistance;

            if (maxDistance == -1)
                maxDistance = embedding->parameters.defaultElectricRepulsionMaxDistance;
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

            double power;
            if (distance == 0)
                power = maxForce;
            else
                power = getValidForce(embedding->parameters.electricRepulsionCoefficient * charge1->getCharge() * charge2->getCharge() / distance / distance);

            if (linearityDistance != -1 && distance > linearityDistance)
                power *= 1 - std::min(1.0, (distance - linearityDistance) / (maxDistance - linearityDistance));

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
        ElectricRepulsion(IBody *charge1, IBody *charge2, double linearityDistance = -1, double maxDistance = -1, int slippery = -1) : AbstractElectricRepulsion(charge1, charge2, linearityDistance, maxDistance, slippery) {
        }

        virtual Pt getDistanceAndVector(double &distance) {
            return AbstractForceProvider::getDistanceAndVector(charge1, charge2, distance);
        }

        virtual const char *getClassName() {
            return "ElectricRepulsion";
        }
};

class VerticalElectricRepulsion : public AbstractElectricRepulsion {
    public:
        VerticalElectricRepulsion(IBody *charge1, IBody *charge2) : AbstractElectricRepulsion(charge1, charge2) {
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
        HorizontalElectricRepulsion(IBody *charge1, IBody *charge2) : AbstractElectricRepulsion(charge1, charge2) {
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
 * Abstract base class for spring attractive forces.
 */
class AbstractSpring : public AbstractForceProvider {
    protected:
        IBody *body1;

        IBody *body2;

        double springCoefficient;

        double reposeLength;

    private:
        void constructor(IBody *body1, IBody *body2, double springCoefficient, double reposeLength) {
            this->body1 = body1;
            this->body2 = body2;
            this->springCoefficient = springCoefficient;
            this->reposeLength = reposeLength;
        }

    public:
        AbstractSpring(IBody *body1, IBody *body2)  : AbstractForceProvider(-1) {
            constructor(body1, body2, -1, -1);
        }

        AbstractSpring(IBody *body1, IBody *body2, double springCoefficient, double reposeLength, int slippery) : AbstractForceProvider(slippery) {
            constructor(body1, body2, springCoefficient, reposeLength);
        }

        virtual void setForceDirectedEmbedding(ForceDirectedEmbedding *embedding) {
            AbstractForceProvider::setForceDirectedEmbedding(embedding);

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

        Spring(IBody *body1, IBody *body2, double springCoefficient, double reposeLength, int slippery = -1)
            : AbstractSpring(body1, body2, springCoefficient, reposeLength, slippery) {
        }

        virtual Pt getDistanceAndVector(double &distance) {
            return AbstractForceProvider::getDistanceAndVector(body1, body2, distance);
        }

        virtual const char *getClassName() {
            return "Spring";
        }
};

class VerticalSpring : public AbstractSpring {
    public:
        VerticalSpring(IBody *body1, IBody *body2) : AbstractSpring(body1, body2) {
        }

        VerticalSpring(IBody *body1, IBody *body2, double springCoefficient, double reposeLength) : AbstractSpring(body1, body2, springCoefficient, reposeLength, -1){
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

        HorizonalSpring(IBody *body1, IBody *body2, double springCoefficient, double reposeLength) : AbstractSpring(body1, body2, springCoefficient, reposeLength, -1) {
        }

        virtual Pt getDistanceAndVector(double &distance) {
            return getStandardHorizontalDistanceAndVector(body1, body2, distance);
        }

        virtual const char *getClassName() {
            return "HorizonalSpring";
        }
};

class LeastExpandedSpring : public AbstractForceProvider {
    private:
        std::vector<AbstractSpring *> springs;

    public:
        LeastExpandedSpring(std::vector<AbstractSpring *> springs) {
            this->springs = springs;
        }

        ~LeastExpandedSpring() {
            for (std::vector<AbstractSpring *>::iterator it = springs.begin(); it != springs.end(); it++)
                delete *it;
        }

        virtual void setForceDirectedEmbedding(ForceDirectedEmbedding *embedding) {
            AbstractForceProvider::setForceDirectedEmbedding(embedding);

            for (std::vector<AbstractSpring *>::iterator it = springs.begin(); it != springs.end(); it++) {
                AbstractSpring *spring = *it;
                spring->setForceDirectedEmbedding(embedding);
            }
        }

        AbstractSpring *findLeastExpandedSpring() {
            AbstractSpring *leastExpandedSpring;
            double leastExpansion = POSITIVE_INFINITY;

            for (std::vector<AbstractSpring *>::iterator it = springs.begin(); it != springs.end(); it++) {
                AbstractSpring *spring = *it;
                double distance;
                Pt vector = spring->getDistanceAndVector(distance);
                double expansion = fabs(distance - spring->getReposeLength());

                if (expansion < leastExpansion) {
                    leastExpansion = expansion;
                    leastExpandedSpring = spring;
                }
            }

            return leastExpandedSpring;
        }

        virtual void applyForces() {
            findLeastExpandedSpring()->applyForces();
        }

        virtual double getPotentialEnergy() {
            return findLeastExpandedSpring()->getPotentialEnergy();
        }

        virtual const char *getClassName() {
            return "LeastExpandedSpring";
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

        BasePlaneSpring(IBody *body, double springCoefficient, double reposeLength) : AbstractSpring(body, NULL, springCoefficient, reposeLength, -1) {
        }

        virtual double getSpringCoefficient() {
            return springCoefficient * embedding->relaxFactor;
        }

        virtual Pt getDistanceAndVector(double &distance) {
            Pt vector = body1->getPosition();
            vector.x = 0;
            vector.y = 0;
            distance = fabs(vector.z);
            return vector;
        }

        virtual const char *getClassName() {
            return "BasePlaneSpring";
        }
};

/**
 * Base class for velocity based kinetic energy reducers.
 */
class AbstractVelocityBasedForceProvider : public AbstractForceProvider {
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
class BodyConstraint : public AbstractForceProvider {
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

        virtual double getPotentialEnergy() = 0;
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
