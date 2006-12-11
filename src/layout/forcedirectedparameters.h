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

class ForceDirectedEmbedding;

class IPositioned {
    public:
        virtual Pt& getPosition() = 0;
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
	    Pt tempForce;
	
    public:
	    Variable(const Pt& position) {
            constructor(position, Pt::getZero());
	    }

	    Variable(const Pt& position, const Pt& velocity) {
            constructor(position, velocity);
	    }

        void constructor(const Pt& position, const Pt& velocity) {
		    this->position = position;
		    this->velocity = velocity;
            mass = 0;
            force = Pt::getZero();
        }

	    Pt& getPosition() {
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
		    acceleration.assign(force).divide(mass);
		    return acceleration;
	    }

	    double getMass() {
		    return mass;
	    }
    	
	    void addMass(double mass) {
		    this->mass += mass;
	    }

	    void addForce(const Pt& vector, double power) {
		    tempForce.assign(vector).normalize().multiply(power);

		    ASSERT(tempForce.isFullySpecified());

		    force.add(tempForce);
		    forces.push_back(tempForce.copy());
	    }

	    void resetForces() {
		    force.setZero();
		    forces.clear();
	    }
    	
	    const std::vector<Pt>& getForces() {
		    return forces;
	    }
};

class IForceProvider {
    public:
    	virtual void applyForces(const ForceDirectedEmbedding& embedding) = 0;
};

class ILimitedForceProvider : public IForceProvider {
    protected:
	    virtual double getMaxForce() = 0;
    	
	    virtual double getValidForce(double force) = 0;
    	
	    virtual double getValidSignedForce(double force) = 0;
};

class LimitedForceProvider : public ILimitedForceProvider {
    protected:
        double maxForce;
	
    public:
        LimitedForceProvider() {
            maxForce = 1000;
        }

    protected:
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
};

class IApplied {
    public:
    	virtual Variable *getVariable() = 0;
};

class IPositionedApplied : public IPositioned, public IApplied {
};

class ICharge : public virtual IPositionedApplied {
    public:
    	virtual double getCharge() = 0;
};

class IMass : public virtual IPositionedApplied {
    public:
	    virtual double getMass() = 0;
};

class IBody : public IMass, public ICharge {
    public:
        virtual Rs& getSize() = 0;
};
/*
abstract class AbstractBody implements IBody {
	protected double mass;
	
	protected double charge;
	
	protected Rs size = new Rs(10, 10);

	public AbstractBody() {
		this(10, 1);
	}

	public AbstractBody(double mass, double charge) {
		this->mass = mass;
		this->charge = charge;
	}

	public double getCharge() {
		return charge;
	}

	public double getMass() {
		return mass;
	}
	
	public Rs getSize() {
		return size;
	}
};

class RelativelyPositionedBody : public Body {
	protected Pt relativePosition;

	protected IPositioned anchor;

	private Pt position;

	public RelativelyPositionedBody(Variable variable, Pt relativePosition) {
		this(variable, variable, relativePosition);
	}

	public RelativelyPositionedBody(Variable variable, IPositioned anchor, Pt relativePosition) {
		super(variable);
		this->anchor = anchor;
		this->relativePosition = relativePosition;
		this->position = Pt.newNil();
	}

	public Pt getPosition() {
		position.assign(anchor.getPosition()).add(relativePosition);
		return position;
	}
};

class Body : public AbstractBody {
	protected Variable variable;

	public Body(Variable variable) {
		this->variable = variable;
		variable.addMass(mass);
	}

	public Pt getPosition() {
		return variable.position;
	}

	public Variable getVariable() {
		return variable;
	}
};

class BorderBody : public Body {
	public BorderBody(Variable variable) {
		super(variable);
	}
	
	public BorderBody(boolean horizontal, double position) {
		this(new Variable(new Pt(horizontal ? Double.NaN : position, horizontal ? position : Double.NaN)));
		
		if (horizontal)
			size = new Rs(Double.POSITIVE_INFINITY, 0);
		else
			size = new Rs(0, Double.POSITIVE_INFINITY);
	}
};

class IElectricRepeal : public IForceProvider {
	public ICharge getCharge1();
	
	public ICharge getCharge2();
};

abstract class AbstractElectricRepeal : public LimitedForceProvider implements IElectricRepeal {
	protected ICharge charge1;
	
	protected ICharge charge2;

	public AbstractElectricRepeal(ICharge charge1, ICharge charge2) {
		this->charge1 = charge1;
		this->charge2 = charge2;
	}

	public ICharge getCharge1() {
		return charge1;
	}
	
	public ICharge getCharge2() {
		return charge2;
	}
	
	public abstract Pt getVector();

	public void applyForces(ForceDirectedEmbedding embedding) {
		Pt vector = getVector();
		double distance = vector.getLength();
		// TODO: find intersection with boxes and use the distance between the boxes
//			double modifiedDistance = distance - vertex1.rs.getDiagonalLength() / 2 - vertex2.rs.getDiagonalLength() / 2;
		double modifiedDistance = distance;
		double force;
		
		if (modifiedDistance <= 0)
			force = maxForce;
		else
			force = getValidForce(embedding.electricRepealCoefficient * charge1.getCharge() * charge2.getCharge() / modifiedDistance / modifiedDistance);
		
		getCharge1().getVariable().addForce(vector, +force);
		getCharge2().getVariable().addForce(vector, -force);
	}
};

class ElectricRepeal : public AbstractElectricRepeal {
	private Pt vector = Pt.newNil();

	public ElectricRepeal(ICharge charge1, ICharge charge2) {
		super(charge1, charge2);
	}

	public Pt getVector() {
		vector.assign(charge1.getPosition()).subtract(charge2.getPosition());
		return vector;
	}
};

class VerticalElectricRepeal : public AbstractElectricRepeal {
	private Pt vector = Pt.newNil();

	public VerticalElectricRepeal(ICharge charge1, ICharge charge2) {
		super(charge1, charge2);
	}

	public Pt getVector() {
		vector.assign(0, charge1.getPosition().y - charge2.getPosition().y);
		return vector;
	}
};

class HorizontalElectricRepeal : public AbstractElectricRepeal {
	private Pt vector = Pt.newNil();

	public HorizontalElectricRepeal(ICharge charge1, ICharge charge2) {
		super(charge1, charge2);
	}

	public Pt getVector() {
		vector.assign(charge1.getPosition().x - charge2.getPosition().x, 0);
		return vector;
	}
};

class ISpring : public IForceProvider {
	public double getSpringCoefficient();
	
	public double getReposeLength();
	
	public IPositionedApplied getBody1();
	
	public IPositionedApplied getBody2();
};

abstract class AbstractSpring : public LimitedForceProvider implements ISpring {
	protected IPositionedApplied body1;
	
	protected IPositionedApplied body2;

	protected double springCoefficient;

	protected double reposeLength;

	public AbstractSpring(IPositionedApplied body1, IPositionedApplied body2) {
		this(body1, body2, 1, 0);
	}
	
	public AbstractSpring(IPositionedApplied body1, IPositionedApplied body2, double springCoefficient, double reposeLength) {
		this->body1 = body1;
		this->body2 = body2;
		this->springCoefficient = springCoefficient;
		this->reposeLength = reposeLength;
	}

	public double getSpringCoefficient() {
		return springCoefficient;
	}

	public double getReposeLength() {
		return reposeLength;
	}

	public IPositionedApplied getBody1() {
		return body1;
	}

	public IPositionedApplied getBody2() {
		return body2;
	}
	
	public abstract Pt getVector();
	
	public void applyForces(ForceDirectedEmbedding embedding) {
		Pt vector = getVector();
		double distance = vector.getLength() - reposeLength;

		if (distance != 0) {
			double force = getValidSignedForce(embedding.springCoefficient * springCoefficient * distance);
			
			getBody1().getVariable().addForce(vector, -force);
			getBody2().getVariable().addForce(vector, +force);
		}
	}
};

class Spring : public AbstractSpring {
	private Pt vector = Pt.newNil();

	public Spring(IPositionedApplied body1, IPositionedApplied body2) {
		super(body1, body2);
	}
	
	public Spring(IPositionedApplied body1, IPositionedApplied body2, double springCoefficient, double reposeLength) {
		super(body1, body2, springCoefficient, reposeLength);
	}

	public Pt getVector() {
		vector.assign(body1.getPosition()).subtract(body2.getPosition());
		return vector;
	}
};

class VerticalSpring : public AbstractSpring {
	private Pt vector = Pt.newNil();

	public VerticalSpring(IPositionedApplied body1, IPositionedApplied body2) {
		super(body1, body2);
	}
	
	public VerticalSpring(IPositionedApplied body1, IPositionedApplied body2, double springCoefficient, double reposeLength) {
		super(body1, body2, springCoefficient, reposeLength);
	}

	public Pt getVector() {
		vector.assign(0, body1.getPosition().y - body2.getPosition().y);
		return vector;
	}
};

class HorizonalSpring : public AbstractSpring {
	private Pt vector = Pt.newNil();

	public HorizonalSpring(IPositionedApplied body1, IPositionedApplied body2) {
		super(body1, body2);
	}
	
	public HorizonalSpring(IPositionedApplied body1, IPositionedApplied body2, double springCoefficient, double reposeLength) {
		super(body1, body2, springCoefficient, reposeLength);
	}

	public Pt getVector() {
		vector.assign(body1.getPosition().x - body2.getPosition().x, 0);
		return vector;
	}
};

class Friction implements IForceProvider {
	private Pt vector = Pt.newNil();

	public void applyForces(ForceDirectedEmbedding embedding) {
		for (Variable variable : embedding.getVariables()) {
			vector.assign(variable.getVelocity()).reverse();
			double vlen = variable.getVelocity().getLength();

			if (vlen != 0)
				variable.addForce(vector, embedding.updatedFrictionCoefficient * vlen);
		}
	}	
};

abstract class BodyConstraint : public LimitedForceProvider {
	protected IBody body;
	
	public BodyConstraint(IBody body) {
		this->body = body;
	}

	public IBody getBody() {
		return body;
	}
};

class PointConstraint : public BodyConstraint {
	protected Pt constraint;
	
	protected double coefficient;
	
	private Pt vector = Pt.newNil();

	public PointConstraint(IBody body, double coefficient, Pt constraint) {
		super(body);
		this->coefficient = coefficient;
		this->constraint = constraint;
	}

	public void applyForces(ForceDirectedEmbedding embedding) {
		vector.assign(constraint).subtract(body.getPosition());
		body.getVariable().addForce(vector, coefficient * vector.getLength());
	}
};

class LineConstraint : public BodyConstraint {
	protected Ln constraint;
	
	protected double coefficient;
	
	private Pt vector = Pt.newNil();

	public LineConstraint(IBody body, double coefficient, Ln constraint) {
		super(body);
		this->coefficient = coefficient;
		this->constraint = constraint;
	}

	public void applyForces(ForceDirectedEmbedding embedding) {
		Pt position = body.getPosition();
		vector.assign(constraint.getClosestPoint(position)).subtract(position);
		body.getVariable().addForce(vector, coefficient * vector.getLength());
	}
};

class CircleConstraint : public BodyConstraint {
	protected Cc constraint;
	
	protected double coefficient;
	
	private Pt vector = Pt.newNil();

	public CircleConstraint(IBody body, double coefficient, Cc constraint) {
		super(body);
		this->coefficient = coefficient;
		this->constraint = constraint;
	}

	public void applyForces(ForceDirectedEmbedding embedding) {
		Pt position = body.getPosition();
		vector.assign(constraint.origin).subtract(position);
		double power = coefficient * (constraint.origin.getDistance(position) - constraint.radius);

		body.getVariable().addForce(vector, power);
	}
};
*/

#endif
