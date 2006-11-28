package org.omnetpp.figures.layout;

import java.util.Random;

import org.eclipse.core.runtime.Assert;

public class ForceDirectedEmbedding extends GraphEmbedding
{
	private static boolean debug = true;

	// Algorithm parameters
	// Magnetism between vertexs having connections.
	public double connectionSpringCoefficient = 0.1;

	// Magnetism and repeal between two connections' endings.
	public double connectionMagneticFieldCoefficient = 0;// 1000;
	public Pt connectionHomogenousMagneticFieldPower = new Pt(0, 0.5);
	public double connectionMagneticDipolPower = 10;

	// Repeal between two vertexs.
	public double vertexElectricRepealCoefficient = 10000;

	// Magnetism between two vertexs.
	public double vertexSpringCoefficient = 0;// 0.001;

	// Friction reduces energy.
	public double frictionCoefficient = 0.1;

	// Simulation default time step.
	public double timeStep = 1;

	// maximum time spend on the calculation
	public double calculationTimeLimit = 10000;

	// Limit of acceleration difference (between bs in RK-4).
	public double accelerationErrorLimit = 1;

	// Acceleration limit during relaxing.
	public double accelerationRelaxLimit = 1;

	// Velocity limit during relaxing.
	public double velocityRelaxLimit = 1;

	// Maximim velocity;
	public double maxVelocity = 100;

	// Maximum spring force.
	public double maxSpringForce = 1000;

	// Maximum electric repeal force.
	public double maxElectricForce = 1000;

	// Maximum magnetic force.
	public double maxMagneticForce = 1000;

	// Minimum number of cycles of relaxing.
	public int minCycle = 0;

	// Maximum number of cycles of relaxing.
	public int maxCycle = 10000;

	// Maximum number of cycles of relaxing.
	public int maxProbCycle = 10;

	// Implementation fields
	private Random random;

	// position change listener
	IForceDirectedEmbeddingListener listener;

	// Constructors
	public ForceDirectedEmbedding(GraphComponent graphComponent) {
		this(graphComponent, null);
	}

	// Constructors
	public ForceDirectedEmbedding(GraphComponent graphComponent, IForceDirectedEmbeddingListener listener) {
		super(graphComponent);
		this.listener = listener;
	}

	public GraphComponent getGraphComponent() {
		return graphComponent;
	}

	// Find the solution for the differential equation using the
	// Runge-Kutta-4 velocity dependent forces algorithm.
	//
	// a1 = b[pn, vn]
	// a2 = b[vn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a1]
	// a3 = b[vn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a2]
	// a4 = b[vn + h * vn + h * h / 2 * a3, vn + h * a3]
	//
	// vn+1 = vn + h * vn + h * h / 6 * [a1 + a2 + a3]
	// vn+1 = vn + h / 6 * (a1 + 2 * a2 + 2 * a3 + a4)
	public void embed() {
		long begin = System.currentTimeMillis();
		int cycle = 0;
		int probCycle = 0;
		boolean relaxed = false;
		// h (Low and High for log h search)
		double h = timeStep;
		// vn : positions
		Pt[] ps = createPtArray();
		// accelerations
		Pt[] acs = createPtArray();
		// vn : velocities
		Pt[] vs = createPtArray();
		// bs
		Pt[] a1 = createPtArray();
		Pt[] a2 = createPtArray();
		Pt[] a3 = createPtArray();
		Pt[] a4 = createPtArray();
		// velocity deltas
		Pt[] dvs = createPtArray();
		Pt[] tvs = createPtArray();
		// position deltas
		Pt[] dps = createPtArray();
		Pt[] tps = createPtArray();
		// initialize random for no distance repealing
		// make it stable and always use the same seed
		random = new Random(0);

		setInitialPositions(ps);

		// Runge Kutta 4th order
		while (!relaxed) {
			long time;
			double hMultiplier = 0;
			double accelerationError = 0;
			double accelerationErrorLimit = 0;
			cycle++;

			while (true) {
				probCycle++;
				
				time = System.currentTimeMillis();
				if (time > begin + calculationTimeLimit)
					break;
				else
					accelerationErrorLimit = (time - begin) / calculationTimeLimit * this.accelerationErrorLimit;

				// a1 = b[vn, vn]
				a(a1, ps, vs);

				// a2 = b[vn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a1]
				addMul(tps, ps, h / 2, vs);
				incMul(tps, h * h / 8, a1);
				addMul(tvs, vs, h / 2, a1);
				a(a2, tps, tvs);

				// a3 = b[vn + h / 2 * vn + h * h / 8 * a1, vn + h / 2 * a2]
				addMul(tps, ps, h / 2, vs);
				incMul(tps, h * h / 8, a1);
				addMul(tvs, vs, h / 2, a2);
				a(a3, tps, tvs);

				// a4 = b[vn + h * vn + h * h / 2 * a3, vn + h * a3]
				addMul(tps, ps, h, vs);
				incMul(tps, h * h / 2, a3);
				addMul(tvs, vs, h, a3);
				a(a4, tps, tvs);

				// Adjust time step (h)
				accelerationError = diff(a1, a2, a3, a4);
				if (debug)
					System.out.println("Cycle at time: " + (time - begin) + " acceleration error limit: " + accelerationErrorLimit + " acceleration error: " + accelerationError + " cycle: " + cycle + " prob cycle: " + probCycle);

				if (accelerationError < accelerationErrorLimit) {
					if (hMultiplier == 0)
						hMultiplier = 2;
					else
						break;
				}
				else {
					if (hMultiplier == 0)
						hMultiplier = 2;
					else if (hMultiplier == 2)
						hMultiplier = 0.5;
				}
				
				h *= hMultiplier;
			}

			time = System.currentTimeMillis();
			if (debug)
				System.out.println("Cycle at time: " + (time - begin) + " acceleration error limit: " + accelerationErrorLimit + " acceleration error: " + accelerationError + " cycle: " + cycle + " prob cycle: " + probCycle);

			// vn+1 = vn + h * vn + h * h / 6 * [a1 + a2 + a3]
			add(dps, a1, a2);
			inc(dps, a3);
			mul(dps, h * h / 6);
			incMul(dps, h, vs);
			inc(ps, dps);

			// vn+1 = vn + h / 6 * (a1 + 2 * a2 + 2 * a3 + a4)
			addMul(dvs, a1, 2, a2);
			incMul(dvs, 2, a3);
			inc(dvs, a4);
			mul(dvs, h / 6);
			inc(vs, dvs);

			// Maximize velocity
			for (int i = 0; i < vs.length; i++) {
				if (vs[i].getLength() > maxVelocity) {
					vs[i].Normalize();
					vs[i].multiply(maxVelocity);
				}
			}

			// move vertices
			addToVertexPositions(dps);

			// Check if relaxed
			relaxed = true;
			for (int i = 0; i < vs.length; i++) {
				acs[i].assign(a1[i]).add(a2[i]).add(a3[i]).add(a4[i]).divide(4);

				if (vs[i].getLength() > velocityRelaxLimit) {
					if (debug)
						System.out.println("Not relax due to velocity: " + vs[i].getLength());
					relaxed = false;
					break;
				}

				if (acs[i].getLength() > accelerationRelaxLimit) {
					if (debug)
						System.out.println("Not relax due to acceleration: " + acs[i].getLength());
					relaxed = false;
					break;
				}
			}

			if (listener != null)
				listener.positionsChanged();
		}

		for (Vertex vertex : graphComponent.getVertices())
			if (vertex.positionConstraint != null)
				vertex.pt = vertex.positionConstraint.getFinalPosition(vertex.pt);

		long end = System.currentTimeMillis();
		System.out.println("Runge-Kutta-4 ended, at time: " + (end - begin) + " cycle: " + cycle + " prob cycle: " + probCycle);
	}

	private void setInitialPositions(Pt[] ps) {
		double width = graphComponent.rc.rs.width;
		double height = graphComponent.rc.rs.height;

		for (Vertex vertex : graphComponent.getVertices()) {
			if (vertex.pt.isNil()) {
				vertex.pt.x = graphComponent.rc.pt.x + random.nextDouble() * (width - vertex.rs.width);
				vertex.pt.y = graphComponent.rc.pt.y + random.nextDouble() * (height - vertex.rs.height);
			}
		}

		for (int i = 0; i < graphComponent.getVertexCount(); i++)
			ps[i] = graphComponent.getVertex(i).getCenter();
	}

	private Pt[] createPtArray() {
		Pt[] pts = new Pt[graphComponent.getVertexCount()];

		for (int i = 0; i < pts.length; i++)
			pts[i] = new Pt(0, 0);

		return pts;
	}

	// Calculation
	private double diff(Pt[] a1, Pt[] a2, Pt[] a3, Pt[] a4)
	{
		double max = 0;
		Assert.isTrue((a1.length == a2.length));
		Assert.isTrue(a2.length == a3.length);
		Assert.isTrue(a3.length == a4.length);

		for (int i = 0; i < a1.length; i++) {
			max = Math.max(max, a1[i].getDistance(a2[i]));
			max = Math.max(max, a2[i].getDistance(a3[i]));
			max = Math.max(max, a3[i].getDistance(a4[i]));
		}

		return max;
	}

	private void addToVertexPositions(Pt[] dps)
	{
		Assert.isTrue(dps.length == graphComponent.getVertexCount());

		for (int i = 0; i < graphComponent.getVertexCount(); i++)
			graphComponent.getVertex(i).pt.add(dps[i]);
	}

	// pts = a + b * c
	private void addMul(Pt[] pts, Pt[] a, double b, Pt[] c)
	{
		Assert.isTrue(a.length == c.length);
		Assert.isTrue(pts != a);
		Assert.isTrue(pts != c);

		for (int i = 0; i < pts.length; i++)
			pts[i].assign(c[i]).multiply(b).add(a[i]);
	}

	// pts += a * b
	private void incMul(Pt[] pts, double a, Pt[] b)
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

	// pts = a + b
	private void add(Pt[] pts, Pt[] a, Pt[] b)
	{
		Assert.isTrue(a.length == b.length);
		Assert.isTrue(pts != a);
		Assert.isTrue(pts != b);

		for (int i = 0; i < pts.length; i++)
			pts[i].assign(a[i]).add(b[i]);
	}

	// pts += a
	private void inc(Pt[] pts, Pt[] a)
	{
		Assert.isTrue(pts.length == a.length);
		Assert.isTrue(pts != a);

		for (int i = 0; i < pts.length; i++)
			pts[i].add(a[i]);
	}

	// pts *= a
	private void mul(Pt[] pts, double a)
	{
		for (int i = 0; i < pts.length; i++)
			pts[i].multiply(a);
	}

	// fs = b[ps, vs]
	private void a(Pt[] fs, Pt[] ps, Pt[] vs)
	{
		Pt ptDistance = new Pt(0, 0);
		Rc rc1 = new Rc(0, 0, 0, 0);
		Rc rc2 = new Rc(0, 0, 0, 0);
		
		// Reset fs
		for (int i = 0; i < fs.length; i++) {
			fs[i].setZero();

			if (listener != null && i < graphComponent.getVertexCount())
				graphComponent.getVertex(i).fs.clear();
		}

		// Apply connection spring force
		if (connectionSpringCoefficient != 0)
			for (int i = 0; i < graphComponent.getEdgeCount(); i++) {
				Edge edge = graphComponent.getEdge(i);
				Vertex vertex1 = edge.source;
				Vertex vertex2 = edge.target;

				if (vertex1 != null && vertex2 != null && vertex1 != vertex2) {
					int i1 = graphComponent.IndexOfVertex(edge.source);
					int i2 = graphComponent.IndexOfVertex(edge.target);

					ptDistance.assign(ps[i2]).subtract(ps[i1]);
					double distance = ptDistance.getLength();

					if (distance != 0) {
						double springForce = connectionSpringCoefficient * distance;

						if (springForce > maxSpringForce)
							springForce = maxSpringForce;

						addForcePair(vertex1, vertex2, fs[i1], fs[i2], springForce, ptDistance, distance);
					}
				}
			}

		// Apply magnetic force
		if (connectionMagneticFieldCoefficient != 0)
			for (int i = 0; i < graphComponent.getEdgeCount(); i++) {
				Edge edge1 = graphComponent.getEdge(i);
				int iSource = graphComponent.IndexOfVertex(edge1.source);
				int iTarget = graphComponent.IndexOfVertex(edge1.target);
				Pt pSource1 = ps[iSource];
				Pt pTarget1 = ps[iTarget];
				Pt connectionCenter1 = new Pt(pSource1).add(pTarget1).divide(2);
				Pt magneticFieldPower = connectionHomogenousMagneticFieldPower;
				Pt m1 = new Pt(pSource1).subtract(pTarget1);
				double connectionLength = m1.getLength();

				if (connectionLength != 0) {
					m1.Normalize();
					m1.multiply(connectionMagneticDipolPower);

					for (int j = i + 1; j < graphComponent.getEdgeCount(); j++) {
						Edge edge2 = graphComponent.getEdge(j);
						Pt pSource2 = ps[graphComponent.IndexOfVertex(edge2.source)];
						Pt pTarget2 = ps[graphComponent.IndexOfVertex(edge2.target)];
						Pt connectionCenter2 = new Pt(pSource2).add(pTarget2).divide(2);
						Pt m2 = new Pt(pTarget2).subtract(pSource2);
						m2.Normalize();
						m2.multiply(connectionMagneticDipolPower);

						// magnetic dipol field power
						// B = (3(m*r)r - mr^2) / r^5
						Pt r = new Pt(connectionCenter1).subtract(connectionCenter2);
						double rLength = r.getLength();
						Pt dipolFieldPower = (r.copy().multiply(3 * m2.crossProduct(r)).subtract(m2.copy().multiply(Math.pow(rLength, 2)))).divide(Math.pow(rLength, 5)).multiply(connectionMagneticDipolPower);

						magneticFieldPower.add(dipolFieldPower);
					}

					double magneticFieldMoment = magneticFieldPower.x * m1.y - magneticFieldPower.y * m1.x;
					double magneticFieldForce = connectionMagneticFieldCoefficient * magneticFieldMoment / (connectionLength / 2);

					if (Math.abs(magneticFieldForce) > maxMagneticForce)
						magneticFieldForce = Math.signum(magneticFieldForce) * maxMagneticForce;
					else if (Double.isNaN(magneticFieldForce))
						magneticFieldForce = maxMagneticForce;

					fs[iSource].x -= magneticFieldForce * m1.y / connectionLength;
					fs[iSource].y += magneticFieldForce * m1.x / connectionLength;
					fs[iTarget].x += magneticFieldForce * m1.y / connectionLength;
					fs[iTarget].y -= magneticFieldForce * m1.x / connectionLength;
				}
			}

		// Apply electric force
		if (vertexElectricRepealCoefficient != 0)
			for (int i = 0; i < graphComponent.getVertexCount(); i++) {
				Vertex vertex1 = graphComponent.getVertex(i);
				rc1.assign(vertex1.pt, vertex1.rs);

				for (int j = i + 1; j < graphComponent.getVertexCount(); j++) {
					Vertex vertex2 = graphComponent.getVertex(j);
					rc2.assign(vertex2.pt, vertex2.rs);

					if (!rc1.intersects(rc2)) {
						ptDistance.assign(ps[i]).subtract(ps[j]);
						double distance = ptDistance.getLength();
						double modifiedDistance = distance - Math.sqrt(vertex1.rs.width * vertex1.rs.width + vertex1.rs.height * vertex1.rs.height) / 2 - Math.sqrt(vertex2.rs.width * vertex2.rs.width + vertex2.rs.height * vertex2.rs.height) / 2;
						double electricForce = vertexElectricRepealCoefficient * vertex1.charge * vertex2.charge / modifiedDistance / modifiedDistance;

						if (modifiedDistance <= 0 || electricForce > maxElectricForce)
							electricForce = maxElectricForce * vertex1.charge * vertex2.charge;

						addForcePair(vertex1, vertex2, fs[i], fs[j], electricForce, ptDistance, distance);
					}
				}
			}

		// Apply vertex spring force
		if (vertexSpringCoefficient != 0)
			for (int i = 0; i < graphComponent.getVertexCount(); i++) {
				Vertex vertex1 = graphComponent.getVertex(i);

				for (int j = i + 1; j < graphComponent.getVertexCount(); j++) {
					Vertex vertex2 = graphComponent.getVertex(j);
					ptDistance.assign(ps[j]).subtract(ps[i]);
					double distance = ptDistance.getLength();

					if (distance != 0) {
						double springForce = vertexSpringCoefficient * distance;

						if (springForce > maxSpringForce)
							springForce = maxSpringForce;

						addForcePair(vertex1, vertex2, fs[i], fs[j], springForce, ptDistance, distance);
					}
				}
			}

		// Apply graph component bounding forces
		if (vertexElectricRepealCoefficient != 0)
			for (int i = 0; i < graphComponent.getVertexCount(); i++) {
				Vertex vertex = graphComponent.getVertex(i);

				ptDistance.assign(ps[i].x - vertex.rs.width / 2 - graphComponent.rc.pt.x, 0);
				double distance = ptDistance.getLength();
				double electricForce = vertexElectricRepealCoefficient * vertex.charge / distance / distance;
				addForcePair(vertex, null, fs[i], null, electricForce, ptDistance, distance);

				ptDistance.assign(ps[i].x + vertex.rs.width / 2 - graphComponent.rc.pt.x - graphComponent.rc.rs.width, 0);
				distance = ptDistance.getLength();
				electricForce = vertexElectricRepealCoefficient * vertex.charge / distance / distance;
				addForcePair(vertex, null, fs[i], null, electricForce, ptDistance, distance);

				ptDistance.assign(0, ps[i].y - vertex.rs.height / 2 - graphComponent.rc.pt.y);
				distance = ptDistance.getLength();
				electricForce = vertexElectricRepealCoefficient * vertex.charge / distance / distance;
				addForcePair(vertex, null, fs[i], null, electricForce, ptDistance, distance);

				ptDistance.assign(0, ps[i].y + vertex.rs.height / 2 - graphComponent.rc.pt.y - graphComponent.rc.rs.height);
				distance = ptDistance.getLength();
				electricForce = vertexElectricRepealCoefficient * vertex.charge / distance / distance;
				addForcePair(vertex, null, fs[i], null, electricForce, ptDistance, distance);
			}

		// Apply vertex position constraint
		for (int i = 0; i < graphComponent.getVertexCount(); i++) {
			Vertex vertex = graphComponent.getVertex(i);

			if (vertex.positionConstraint != null) {
				Pt force = vertex.positionConstraint.getForce(ps[i]);
				addForce(vertex, fs[i], force);
			}
		}

		// Apply friction force
		if (frictionCoefficient != 0)
			for (int i = 0; i <  graphComponent.getVertexCount(); i++) {
				double vlen = vs[i].getLength();
				fs[i].x -= frictionCoefficient * vlen * vs[i].x;
				fs[i].y -= frictionCoefficient * vlen * vs[i].y;
			}

		// Convert forces to acceleration
		for (int i = 0; i < graphComponent.getVertexCount(); i++)
			fs[i].divide(graphComponent.getVertex(i).mass);
	}

	private void addForcePair(Vertex vertex1, Vertex vertex2, Pt f1, Pt f2, double force, Pt d, double distance)
	{
		double forceX;
		double forceY;

		if (distance != 0) {
			forceX = force * d.x / distance;
			forceY = force * d.y / distance;
		}
		else {
			forceX = random.nextDouble() - 0.5;
			forceY = random.nextDouble() - 0.5;
		}

		Assert.isTrue(!Double.isNaN(forceX));
		Assert.isTrue(!Double.isNaN(forceY));

		if (f1 != null) {
			f1.x += forceX;
			f1.y += forceY;
		}

		if (f2 != null) {
			f2.x -= forceX;
			f2.y -= forceY;
		}

		if (listener != null && vertex1 != null)
			vertex1.fs.add(new Pt(forceX, forceY));

		if (listener != null && vertex2 != null)
			vertex2.fs.add(new Pt(-forceX, -forceY));
	}

	private void addForce(Vertex vertex, Pt f, Pt force) {
		if (f != null)
			f.add(force);

		if (vertex != null)
			vertex.fs.add(force);
	}

	public interface IForceDirectedEmbeddingListener {
		public void positionsChanged();
	}
}
