package org.omnetpp.figures.layout;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import org.eclipse.core.runtime.Assert;

public class ForceDirectedEmbedding
{
	protected static boolean debug = false;

	protected Random random;

	/**
	 * Spring force coefficient.
	 */
	public double springCoefficient = 0.1;

	/**
	 * Electric repeal force coefficient.
	 */
	public double electricRepealCoefficient = 100000;

	/**
	 * Position change listener gets notification during the solution.
	 */
	protected IForceDirectedEmbeddingListener listener;

	/**
	 * Friction reduces the energy of the system. The friction force points in the opposite direction of the current velocity.
	 * Friction is automatically updated during the solution. The current kinetic energy is compared against the average kinetic
	 * energy reduced by the time spent on the calculation and the friction coefficient is updated so that towards the end of the
	 * calculation the kinetic energy of the system converges to zero.
	 */
	public double frictionCoefficient = 1;
	
	/**
	 * This gets updated.
	 */
	protected double updatedFrictionCoefficient;
	
	/**
	 * Lower limit for friction coefficient.
	 */
	public double minFrictionCoefficient = 0.1;
	
	/**
	 * Higher limit for friction coefficient update.
	 */
	public double maxFrictionCoefficient = 10;
	
	/**
	 * Multiplier used to update the friction coefficient.
	 */
	public double frictionCoefficientMultiplier = 2;

	/**
	 * The default time step when solution starts.
	 * Time step is automatically updated during the solution. It will always have the highest value so that the acceleration error
	 * is less than the acceleration error limit. The time step is either multiplied or divided by the time step multiplier according to
	 * the current acceleration error.
	 */
	public double timeStep = 1;
	
	/**
	 * This gets updated.
	 */
	protected double updatedTimeStep;

	/**
	 * Lower limit for time step update.
	 */
	public double minTimeStep = 0.001;

	/**
	 * Lower limit for time step update.
	 */
	public double maxTimeStep = 1000;

	/**
	 * Multiplier used to update the time step.
	 */
	public double timeStepMultiplier = 2;

	/**
	 * Maximum time spend on the calculation in milliseconds.
	 * The algorithm will always return after this time has elapsed.
	 */
	public double calculationTimeLimit = 100000;

	/**
	 * Limit of acceleration approximation difference (between a1, a2, a3 and a4 in RK-4).
	 * Acceleration error limit is updated automatically during the solution. It decreases towards zero proportional to the
	 * time spent on the calculation.
	 */
	public double accelerationErrorLimit = 1;
	
	/**
	 * This gets updated.
	 */
	protected double updatedAccelerationErrorLimit;

	/**
	 * Acceleration limit during the solution.
	 * When all vertices has lower acceleration than this limit then the algorithm may be stopped.
	 */
	public double accelerationRelaxLimit = 1;

	/**
	 * Velocity limit during the solution.
	 * When all vertices has lower velocity than this limit then the algorithm may be stopped.
	 */
	public double velocityRelaxLimit = 0.1;

	/**
	 * Maximim velocity;
	 */
	public double maxVelocity = 100;
	
	private List<Variable> variables = new ArrayList<Variable>();
	
	private List<IForceProvider> forceProviders = new ArrayList<IForceProvider>();
	
	private List<IBody> bodies = new ArrayList<IBody>();
	
	public ForceDirectedEmbedding() {
	}
	
	public ForceDirectedEmbedding(IForceDirectedEmbeddingListener listener) {
		this.listener = listener;
	}

	public List<Variable> getVariables() {
		return variables;
	}

	public List<IForceProvider> getForceProviders() {
		return forceProviders;
	}

	public void addForceProvider(IForceProvider forceProvider) {
		forceProviders.add(forceProvider);
	}

	public void removeForceProvider(IForceProvider forceProvider) {
		forceProviders.remove(forceProvider);
	}

	public List<IBody> getBodies() {
		return bodies;
	}
	
	public void addBody(IBody body) {
		bodies.add(body);
		
		if (variables.indexOf(body.getVariable()) == -1)
			variables.add(body.getVariable());
	}

	/**
	 * Find the solution for the differential equation using a
     * modified Runge-Kutta-4 algorithm.
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
	public void embed() {
		long begin = System.currentTimeMillis();
		int cycle = 0;
		int probCycle = 0;
		boolean relaxed = false;
		double timeStepSum = 0;
		double energySum = 0;
		double energy;
		// vn : positions
		Pt[] pn = createPtArray();
		// accelerations
		Pt[] an = createPtArray();
		// vn : velocities
		Pt[] vn = createPtArray();
		// as : accelerations
		Pt[] a1 = createPtArray();
		Pt[] a2 = createPtArray();
		Pt[] a3 = createPtArray();
		Pt[] a4 = createPtArray();
		// velocity deltas
		Pt[] dvn = createPtArray();
		Pt[] tvn = createPtArray();
		// position deltas
		Pt[] dpn = createPtArray();
		Pt[] tpn = createPtArray();
		// initialize random for no distance repealing
		// make it stable and always use the same seed
		random = new Random(0);

		// set positions and velocities
		for (int i = 0; i < variables.size(); i++) {
			Variable variable = variables.get(i);
			pn[i].assign(variable.getPosition());
			vn[i].assign(variable.getVelocity());
		}

		// initial values
		updatedTimeStep = timeStep;
		updatedAccelerationErrorLimit = accelerationErrorLimit;
		updatedFrictionCoefficient = frictionCoefficient;

		// Runge Kutta 4th order
		while (!relaxed) {
			long time;
			double hMultiplier = 0;
			double accelerationError = 0;
			cycle++;

			while (true) {
				probCycle++;

				// update acceleration error limit
				time = System.currentTimeMillis() - begin;
				if (time > calculationTimeLimit)
					break;
				else
					updatedAccelerationErrorLimit = (calculationTimeLimit - time) / calculationTimeLimit * this.accelerationErrorLimit;
				if (debug)
					System.out.println("Prob cycle at real time: " + time + " time: " + timeStepSum + " h: " + updatedTimeStep + " friction: " + updatedFrictionCoefficient + " acceleration error limit: " + updatedAccelerationErrorLimit + " acceleration error: " + accelerationError + " cycle: " + cycle + " prob cycle: " + probCycle);

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
				
				if (accelerationError < updatedAccelerationErrorLimit) {
					if (hMultiplier == 0)
						hMultiplier = timeStepMultiplier;
					else
						break;
				}
				else {
					if (hMultiplier == 0)
						hMultiplier = timeStepMultiplier;
					else if (hMultiplier == timeStepMultiplier)
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
			for (int i = 0; i < vn.length; i++) {
				if (vn[i].getLength() > maxVelocity) {
					vn[i].normalize();
					vn[i].multiply(maxVelocity);
				}
			}

			// check time limit
			time = System.currentTimeMillis() - begin;
			if (time > calculationTimeLimit)
				break;
			if (debug)
				System.out.println("Cycle at real time: " + time + " time: " + timeStepSum + " h: " + updatedTimeStep + " friction: " + updatedFrictionCoefficient + " acceleration error limit: " + updatedAccelerationErrorLimit + " acceleration error: " + accelerationError + " cycle: " + cycle + " prob cycle: " + probCycle);

			// update friction
			timeStepSum += updatedTimeStep;
			energy = 0;
			for (Variable variable : variables) {
				double velocity = variable.velocity.getLength();
				energy +=  1.0 / 2.0 * variable.getMass() * velocity * velocity;
			}
			energySum += energy * updatedTimeStep;
			double energyAvg = (energySum / timeStepSum);
			double energyExpected = energyAvg * (calculationTimeLimit - time) / calculationTimeLimit;
			if (debug)
				System.out.println("Energy: " + energy + " energy expected: " + energyExpected);
			if (energy <  energyExpected && updatedFrictionCoefficient / frictionCoefficientMultiplier > minFrictionCoefficient)
				updatedFrictionCoefficient /= frictionCoefficientMultiplier;
			else if (energy > energyExpected && updatedFrictionCoefficient * frictionCoefficientMultiplier < maxFrictionCoefficient)
				updatedFrictionCoefficient *= frictionCoefficientMultiplier;
			
			if (listener != null)
				listener.variablesChanged();

			// Check if relaxed
			relaxed = true;
			for (int i = 0; i < vn.length; i++) {
				an[i].assign(a1[i]).add(a2[i]).add(a3[i]).add(a4[i]).divide(4);

				if (vn[i].getLength() > velocityRelaxLimit) {
					if (debug)
						System.out.println("Not relax due to velocity: " + vn[i].getLength());
					relaxed = false;
					break;
				}

				if (an[i].getLength() > accelerationRelaxLimit) {
					if (debug)
						System.out.println("Not relax due to acceleration: " + an[i].getLength());
					relaxed = false;
					break;
				}
			}
		}
		
		if (listener != null)
			listener.variablesChanged();

		long end = System.currentTimeMillis();
		System.out.println("Runge-Kutta-4 ended, at real time: " + (end - begin) + " cycle: " + cycle + " prob cycle: " + probCycle);
	}

	private Pt[] createPtArray() {
		Pt[] pts = new Pt[variables.size()];

		for (int i = 0; i < pts.length; i++)
			pts[i] = Pt.newZero();

		return pts;
	}

	private double maximumDifference(Pt[] a1, Pt[] a2, Pt[] a3, Pt[] a4)
	{
		double max = 0;
		Assert.isTrue(a1.length == a2.length && a2.length == a3.length && a3.length == a4.length);

		for (int i = 0; i < a1.length; i++) {
			max = Math.max(max, a1[i].getDistance(a2[i]));
			max = Math.max(max, a2[i].getDistance(a3[i]));
			max = Math.max(max, a3[i].getDistance(a4[i]));
		}

		return max;
	}

	/**
	 * pts = a + b * c
	 */
	private void addMultiplied(Pt[] pts, Pt[] a, double b, Pt[] c)
	{
		Assert.isTrue(a.length == c.length);
		Assert.isTrue(pts.length == c.length);
		Assert.isTrue(pts != a);
		Assert.isTrue(pts != c);

		for (int i = 0; i < pts.length; i++)
			pts[i].assign(c[i]).multiply(b).add(a[i]);
	}

	/**
	 * pts += a * b
	 */
	private void incrementWithMultiplied(Pt[] pts, double a, Pt[] b)
	{
		Assert.isTrue(pts.length == b.length);
		Assert.isTrue(pts != b);

		for (int i = 0; i < pts.length; i++) {
			Pt pt = pts[i];
			double x = pt.x;
			double y = pt.y;

			pts[i].assign(b[i]).multiply(a).add(x, y);
		}
	}

	/**
	 * pts = a + b
	 */
	private void add(Pt[] pts, Pt[] a, Pt[] b)
	{
		Assert.isTrue(a.length == b.length);
		Assert.isTrue(pts != a);
		Assert.isTrue(pts != b);

		for (int i = 0; i < pts.length; i++)
			pts[i].assign(a[i]).add(b[i]);
	}

	/**
	 * pts += a
	 */
	private void increment(Pt[] pts, Pt[] a)
	{
		Assert.isTrue(pts.length == a.length);
		Assert.isTrue(pts != a);

		for (int i = 0; i < pts.length; i++)
			pts[i].add(a[i]);
	}

	/**
	 * pts *= a
	 */
	private void multiply(Pt[] pts, double a)
	{
		for (int i = 0; i < pts.length; i++)
			pts[i].multiply(a);
	}

	/**
	 * an = b[pn, vn]
	 */
	private void a(Pt[] an, Pt[] pn, Pt[] vn)
	{
		// set positions and velocities and reset forces
		for (int i = 0; i < variables.size(); i++) {
			Variable variable = variables.get(i);
			variable.assignPosition(pn[i]);
			variable.assignVelocity(vn[i]);
			variable.resetForces();
		}

		// calculate forces
		for (IForceProvider forceProvider : forceProviders)
			forceProvider.applyForces(this);

		// return accelerations
		for (int i = 0; i < variables.size(); i++) {
			Variable variable = variables.get(i);
			an[i].assign(variable.getAcceleration());
		}
	}

	/**
	 * Notification interface.
	 */
	public interface IForceDirectedEmbeddingListener {
		public void variablesChanged();
	}
}
