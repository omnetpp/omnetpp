package org.omnetpp.figures.layout;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;

interface IPositioned {
	public Pt getPosition();
}

class Variable implements IPositioned {
	protected Pt position;

	protected Pt velocity;

	protected Pt acceleration = Pt.newNil();
	
	protected Pt force = Pt.newZero();

	protected List<Pt> forces = new ArrayList<Pt>();
	
	protected double mass = 0;
	
	private Pt tempForce = Pt.newNil();
	
	public Variable(Pt position) {
		this(position, Pt.newZero());
	}

	public Variable(Pt position, Pt velocity) {
		this.position = position;
		this.velocity = velocity;
	}

	public Pt getPosition() {
		return position;
	}

	public void assignPosition(Pt position) {
		this.position.assign(position);
	}

	public Pt getVelocity() {
		return velocity;
	}

	public void assignVelocity(Pt velocity) {
		this.velocity.assign(velocity);
	}

	public Pt getAcceleration() {
		acceleration.assign(force).divide(mass);
		return acceleration;
	}

	public double getMass() {
		return mass;
	}
	
	public void addMass(double mass) {
		this.mass += mass;
	}

	public void addForce(Pt vector, double power) {
		tempForce.assign(vector).normalize().multiply(power);

		Assert.isTrue(tempForce.isFullySpecified());

		force.add(tempForce);
		forces.add(tempForce.copy());
	}

	public void resetForces() {
		force.setZero();
		forces.clear();
	}
	
	public List<Pt> getForces() {
		return forces;
	}
}

interface IForceProvider {
	public void applyForces(ForceDirectedEmbedding2 embedding);
}

interface ILimitedForceProvider extends IForceProvider {
	public double getMaxForce();
	
	public double getValidForce(double force);
	
	public double getValidSignedForce(double force);
}

abstract class LimitedForceProvider implements ILimitedForceProvider {
	protected double maxForce = 1000;
	
	public double getMaxForce() {
		return maxForce;
	}
	
	public double getValidForce(double force) {
		Assert.isTrue(force >= 0);
		return Math.min(maxForce, force);
	}
	
	public double getValidSignedForce(double force) {
		return Math.signum(force) * getValidForce(Math.abs(force));
	}
}

interface IApplied {
	public Variable getVariable();
}

interface IPositionedApplied extends IPositioned, IApplied {
}

interface ICharge extends IPositionedApplied {
	public double getCharge();
}

interface IMass extends IPositionedApplied {
	public double getMass();
}

interface IBody extends IMass, ICharge {
	public Rs getSize();
}

abstract class AbstractBody implements IBody {
	protected double mass;
	
	protected double charge;
	
	protected Rs size = new Rs(10, 10);

	public AbstractBody() {
		this(10, 1);
	}

	public AbstractBody(double mass, double charge) {
		this.mass = mass;
		this.charge = charge;
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
}

class RelativelyPositionedBody extends Body {
	protected Pt relativePosition;

	protected IPositioned anchor;

	private Pt position;

	public RelativelyPositionedBody(Variable variable, Pt relativePosition) {
		this(variable, variable, relativePosition);
	}

	public RelativelyPositionedBody(Variable variable, IPositioned anchor, Pt relativePosition) {
		super(variable);
		this.anchor = anchor;
		this.relativePosition = relativePosition;
		this.position = Pt.newNil();
	}

	public Pt getPosition() {
		position.assign(anchor.getPosition()).add(relativePosition);
		return position;
	}
}

class Body extends AbstractBody {
	protected Variable variable;

	public Body(Variable variable) {
		this.variable = variable;
		variable.addMass(mass);
	}

	public Pt getPosition() {
		return variable.position;
	}

	public Variable getVariable() {
		return variable;
	}
}

class BorderBody extends Body {
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
}

interface IElectricRepeal extends IForceProvider {
	public ICharge getCharge1();
	
	public ICharge getCharge2();
}

abstract class AbstractElectricRepeal extends LimitedForceProvider implements IElectricRepeal {
	protected ICharge charge1;
	
	protected ICharge charge2;

	public AbstractElectricRepeal(ICharge charge1, ICharge charge2) {
		this.charge1 = charge1;
		this.charge2 = charge2;
	}

	public ICharge getCharge1() {
		return charge1;
	}
	
	public ICharge getCharge2() {
		return charge2;
	}
	
	public abstract Pt getVector();

	public void applyForces(ForceDirectedEmbedding2 embedding) {
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
}

class ElectricRepeal extends AbstractElectricRepeal {
	private Pt vector = Pt.newNil();

	public ElectricRepeal(ICharge charge1, ICharge charge2) {
		super(charge1, charge2);
	}

	public Pt getVector() {
		vector.assign(charge1.getPosition()).subtract(charge2.getPosition());
		return vector;
	}
}

class VerticalElectricRepeal extends AbstractElectricRepeal {
	private Pt vector = Pt.newNil();

	public VerticalElectricRepeal(ICharge charge1, ICharge charge2) {
		super(charge1, charge2);
	}

	public Pt getVector() {
		vector.assign(0, charge1.getPosition().y - charge2.getPosition().y);
		return vector;
	}
}

class HorizontalElectricRepeal extends AbstractElectricRepeal {
	private Pt vector = Pt.newNil();

	public HorizontalElectricRepeal(ICharge charge1, ICharge charge2) {
		super(charge1, charge2);
	}

	public Pt getVector() {
		vector.assign(charge1.getPosition().x - charge2.getPosition().x, 0);
		return vector;
	}
}

interface ISpring extends IForceProvider {
	public double getSpringCoefficient();
	
	public double getReposeLength();
	
	public IPositionedApplied getBody1();
	
	public IPositionedApplied getBody2();
}

abstract class AbstractSpring extends LimitedForceProvider implements ISpring {
	protected IPositionedApplied body1;
	
	protected IPositionedApplied body2;

	protected double springCoefficient;

	protected double reposeLength;

	public AbstractSpring(IPositionedApplied body1, IPositionedApplied body2) {
		this(body1, body2, 1, 0);
	}
	
	public AbstractSpring(IPositionedApplied body1, IPositionedApplied body2, double springCoefficient, double reposeLength) {
		this.body1 = body1;
		this.body2 = body2;
		this.springCoefficient = springCoefficient;
		this.reposeLength = reposeLength;
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
	
	public void applyForces(ForceDirectedEmbedding2 embedding) {
		Pt vector = getVector();
		double distance = vector.getLength() - reposeLength;

		if (distance != 0) {
			double force = getValidSignedForce(embedding.springCoefficient * springCoefficient * distance);
			
			getBody1().getVariable().addForce(vector, -force);
			getBody2().getVariable().addForce(vector, +force);
		}
	}
}

class Spring extends AbstractSpring {
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
}

class VerticalSpring extends AbstractSpring {
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
}

class HorizonalSpring extends AbstractSpring {
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
}

class Friction implements IForceProvider {
	private Pt vector = Pt.newNil();

	public void applyForces(ForceDirectedEmbedding2 embedding) {
		for (Variable variable : embedding.getVariables()) {
			vector.assign(variable.getVelocity()).reverse();
			double vlen = variable.getVelocity().getLength();

			if (vlen != 0)
				variable.addForce(vector, embedding.updatedFrictionCoefficient * vlen);
		}
	}	
}

abstract class BodyConstraint extends LimitedForceProvider {
	protected IBody body;
	
	public BodyConstraint(IBody body) {
		this.body = body;
	}

	public IBody getBody() {
		return body;
	}
}

class PointConstraint extends BodyConstraint {
	protected Pt constraint;
	
	protected double coefficient;
	
	private Pt vector = Pt.newNil();

	public PointConstraint(IBody body, double coefficient, Pt constraint) {
		super(body);
		this.coefficient = coefficient;
		this.constraint = constraint;
	}

	public void applyForces(ForceDirectedEmbedding2 embedding) {
		vector.assign(constraint).subtract(body.getPosition());
		body.getVariable().addForce(vector, coefficient * vector.getLength());
	}
}

class LineConstraint extends BodyConstraint {
	protected Ln constraint;
	
	protected double coefficient;
	
	private Pt vector = Pt.newNil();

	public LineConstraint(IBody body, double coefficient, Ln constraint) {
		super(body);
		this.coefficient = coefficient;
		this.constraint = constraint;
	}

	public void applyForces(ForceDirectedEmbedding2 embedding) {
		Pt position = body.getPosition();
		vector.assign(constraint.getClosestPoint(position)).subtract(position);
		body.getVariable().addForce(vector, coefficient * vector.getLength());
	}
}

class CircleConstraint extends BodyConstraint {
	protected Cc constraint;
	
	protected double coefficient;
	
	private Pt vector = Pt.newNil();

	public CircleConstraint(IBody body, double coefficient, Cc constraint) {
		super(body);
		this.coefficient = coefficient;
		this.constraint = constraint;
	}

	public void applyForces(ForceDirectedEmbedding2 embedding) {
		Pt position = body.getPosition();
		vector.assign(constraint.origin).subtract(position);
		double power = coefficient * (constraint.origin.getDistance(position) - constraint.radius);

		body.getVariable().addForce(vector, power);
	}
}
