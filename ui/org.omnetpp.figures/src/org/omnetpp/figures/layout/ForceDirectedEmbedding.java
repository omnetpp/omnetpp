package org.omnetpp.figures.layout;

import java.awt.Canvas;
import java.awt.Color;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.ArrayList;
import java.util.Random;

public class ForceDirectedEmbedding
{	
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
	
	public boolean adaptive = true;

	// Simulation time step.
	public double timeStep = 1;

	// Maximum simulation time step allowed.
	public double minTimeStep = 0.001;

	// Maximum simulation time step allowed.
	public double maxTimeStep = 10;

	// Limit of acceleration difference (between bs in RK-4).
	// Error has to be between Min and Max
	public double accelerationMinErrorLimit = 0.1;
	public double accelerationMaxErrorLimit = 1;

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

	private GraphComponent graphComponent;
	
	IForceDirectedEmbeddingListener listener;

	// Constructors
	public ForceDirectedEmbedding(GraphComponent graphComponent, IForceDirectedEmbeddingListener listener) {
		this.graphComponent = graphComponent;
		this.listener = listener;
	}

	// Find the solution for the differential equation using the
	// Runge-Kutta-4 velocity dependent forces algorithm.
	//
	// b1 = b[yn, yn']
	// b2 = b[yn + h / 2 * yn' + h * h / 8 * b1, yn' + h / 2 * b1]
	// b3 = b[yn + h / 2 * yn' + h * h / 8 * b1, yn' + h / 2 * b2]
	// b4 = b[yn + h * yn' + h * h / 2 * b3, yn' + h * b3]
	//
	// yn+1 = yn + h * yn' + h * h / 6 * [b1 + b2 + b3]
	// yn+1' = yn' + h / 6 * (b1 + 2 * b2 + 2 * b3 + b4)
	public void embed() {
		int cycle = 0;
		int probCycle = 0;
		boolean relaxed = false;
		// h (Low and High for log h search)
		double h = timeStep;
		double hLow;
		double hHigh;
		// yn : positions
		Pt[] ps = createPtArray();
		// accelerations
		Pt[] acs = createPtArray();
		// yn' : velocities
		Pt[] vs = createPtArray();
		// bs
		Pt[] b1 = createPtArray();
		Pt[] b2 = createPtArray();
		Pt[] b3 = createPtArray();
		Pt[] b4 = createPtArray();
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

		while (!relaxed) {
			// Runge Kutta 4th order
			hLow = minTimeStep;
			hHigh = maxTimeStep;
			boolean hFound = false;
			int currentProbCycle = 0;

			while (!hFound) {
				probCycle++;
				currentProbCycle++;

				if (currentProbCycle > maxProbCycle)
					break;

				// b1 = b[yn, yn']
				b(b1, ps, vs);

				// b2 = b[yn + h / 2 * yn' + h * h / 8 * b1, yn' + h / 2 * b1]
				addMul(tps, ps, h / 2, vs);
				addMul(tps, tps, h * h / 8, b1);
				addMul(tvs, vs, h / 2, b1);
				b(b2, tps, tvs);
			
				// b3 = b[yn + h / 2 * yn' + h * h / 8 * b1, yn' + h / 2 * b2]
				addMul(tps, ps, h / 2, vs);
				addMul(tps, tps, h * h / 8, b2);// ???? b1
				addMul(tvs, vs, h / 2, b2);
				b(b3, tps, tvs);
			
				// b4 = b[yn + h * yn' + h * h / 2 * b3, yn' + h * b3]
				addMul(tps, ps, h, vs);
				addMul(tps, tps, h * h / 2, b3);
				addMul(tvs, vs, h, b3);
				b(b4, tps, tvs);

				// Adjust time step (h)
				double accelerationError = diff(b1, b2, b3, b4);

				System.out.println("**** " + accelerationError + " ***** " + h);

				if (!adaptive)
					break;
				
				if (accelerationError < accelerationMinErrorLimit) {
					hLow = h;
					h = (hLow + hHigh) / 2;
				}
				else if (accelerationError > accelerationMaxErrorLimit) {
					hHigh = h;					
					h = (hLow + hHigh) / 2;
				}
				else
					hFound = true;
			}

			System.out.println("Found h: " + h);

			// yn+1 = yn + h * yn' + h * h / 6 * [b1 + b2 + b3]
			add(dps, b1, b2);
			add(dps, dps, b3);
			mul(dps, h * h / 6, dps);
			addMul(dps, dps, h, vs);
			add(ps, ps, dps);

			// yn+1' = yn' + h / 6 * (b1 + 2 * b2 + 2 * b3 + b4)
			addMul(dvs, b1, 2, b2);
			addMul(dvs, dvs, 2, b3);
			add(dvs, dvs, b4);
			mul(dvs, h / 6, dvs);
			add(vs, vs, dvs);
			
			// Maximize velocity
			for (int i = 0; i < vs.length; i++) {
				if (vs[i].getLength() > maxVelocity) {
					vs[i].Normalize();
					vs[i].multiply(maxVelocity);
				}
			}
				
			// Check if relaxed
			cycle++;

			if (cycle > minCycle)
				relaxed = true;

			if (relaxed && cycle < maxCycle)
				for (int i = 0; i < vs.length; i++) {
					acs[i].assign(b1[i]).add(b2[i]).add(b3[i]).add(b4[i]).divide(4);

					if (vs[i].getLength() > velocityRelaxLimit || acs[i].getLength() > accelerationRelaxLimit) {
						relaxed = false;
						break;
					}
				}

			addToVertexPositions(dps);
			
			if (listener != null)
				listener.positionsChanged();
		}

		for (Vertex vertex : graphComponent.getVertices())
			if (vertex.positionConstraint != null)
				vertex.pt = vertex.positionConstraint.getFinalPosition(vertex.pt);

		System.out.println("Runge-Kutta-4 number of cycles to relax: " + cycle + " Prob cycle: " + probCycle);
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
		Pt[] result = new Pt[graphComponent.getVertexCount() + 1];
		
		for (int i = 0; i < result.length; i++)
			result[i] = new Pt(0, 0);
		
		return result;
	}

	// Calculation
	private double diff(Pt[] b1, Pt[] b2, Pt[] b3, Pt[] b4)
	{
		double max = 0;
		assert (b1.length == b2.length);
		assert (b2.length == b3.length);
		assert (b3.length == b4.length);

		for (int i = 0; i < b1.length; i++) {
			max = Math.max(max, b1[i].getDistance(b2[i]));
			max = Math.max(max, b2[i].getDistance(b3[i]));
			max = Math.max(max, b3[i].getDistance(b4[i]));
		}

		return max;
	}

	private void addToVertexPositions(Pt[] dps)
	{
		assert (dps.length == graphComponent.getVertexCount());

		for (int i = 0; i < graphComponent.getVertexCount(); i++)
			graphComponent.getVertex(i).pt.add(dps[i]);
	}
	
	// result = a + b * c
	private void addMul(Pt[] result, Pt[] a, double b, Pt[] c)
	{
		assert (a.length == c.length);

		for (int i = 0; i < result.length; i++)
			result[i].assign(c[i].copy().multiply(b).add(a[i]));
	}

	// result = a + b
	private void add(Pt[] result, Pt[] a, Pt[] b)
	{
		assert (a.length == b.length);

		for (int i = 0; i < result.length; i++)
			result[i].assign(a[i].copy().add(b[i]));
	}

	// result = a * b
	private void mul(Pt[] result, double a, Pt[] b)
	{
		for (int i = 0; i < result.length; i++)
			result[i].assign(b[i].copy().multiply(a));
	}

	// fs = b[ps, vs]
	private void b(Pt[] fs, Pt[] ps, Pt[] vs)
	{
		// Reset fs
		for (int i = 0; i < fs.length; i++) {
			fs[i].setZero();
			
			if (i < graphComponent.getVertexCount())
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

					Pt d = ps[i2].copy().subtract(ps[i1]);
					double distance = d.getLength();

					if (distance != 0) {
						double springForce = connectionSpringCoefficient * distance;

						if (springForce > maxSpringForce)
							springForce = maxSpringForce;

						addForcePair(vertex1, vertex2, fs[i1], fs[i2], springForce, d, distance);
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
				Rc rc1 = new Rc(vertex1.pt, vertex1.rs);

				for (int j = i + 1; j < graphComponent.getVertexCount(); j++) {
					Vertex vertex2 = graphComponent.getVertex(j);
					Rc rc2 = new Rc(vertex2.pt, vertex2.rs);

					if (!rc1.intersects(rc2)) {
						Pt d = ps[i].copy().subtract(ps[j]);
						double distance = d.getLength();
						double modifiedDistance = distance - Math.sqrt(vertex1.rs.width * vertex1.rs.width + vertex1.rs.height * vertex1.rs.height) / 2 - Math.sqrt(vertex2.rs.width * vertex2.rs.width + vertex2.rs.height * vertex2.rs.height) / 2;
						double electricForce = vertexElectricRepealCoefficient * vertex1.charge * vertex2.charge / modifiedDistance / modifiedDistance;
	
						if (modifiedDistance <= 0 || electricForce > maxElectricForce)
							electricForce = maxElectricForce * vertex1.charge * vertex2.charge;
	
						addForcePair(vertex1, vertex2, fs[i], fs[j], electricForce, d, distance);
					}
				}
			}

		// Apply vertex spring force
		if (vertexSpringCoefficient != 0)
			for (int i = 0; i < graphComponent.getVertexCount(); i++) {
				Vertex vertex1 = graphComponent.getVertex(i);

				for (int j = i + 1; j < graphComponent.getVertexCount(); j++) {
					Vertex vertex2 = graphComponent.getVertex(j);
					Pt d = ps[j].copy().subtract(ps[i]);
					double distance = d.getLength();

					if (distance != 0) {
						double springForce = vertexSpringCoefficient * distance;

						if (springForce > maxSpringForce)
							springForce = maxSpringForce;

						addForcePair(vertex1, vertex2, fs[i], fs[j], springForce, d, distance);
					}
				}
			}
		
		// Apply graph component bounding forces
		if (vertexElectricRepealCoefficient != 0)
			for (int i = 0; i < graphComponent.getVertexCount(); i++) {
				Vertex vertex = graphComponent.getVertex(i);

				Pt d = new Pt(ps[i].x - vertex.rs.width / 2 - graphComponent.rc.pt.x, 0);
				double distance = d.getLength();
				double electricForce = vertexElectricRepealCoefficient * vertex.charge / distance / distance;
				addForcePair(vertex, null, fs[i], null, electricForce, d, distance);

				d = new Pt(ps[i].x + vertex.rs.width / 2 - graphComponent.rc.pt.x - graphComponent.rc.rs.width, 0);
				distance = d.getLength();
				electricForce = vertexElectricRepealCoefficient * vertex.charge / distance / distance;	
				addForcePair(vertex, null, fs[i], null, electricForce, d, distance);

				d = new Pt(0, ps[i].y - vertex.rs.height / 2 - graphComponent.rc.pt.y);
				distance = d.getLength();
				electricForce = vertexElectricRepealCoefficient * vertex.charge / distance / distance;
				addForcePair(vertex, null, fs[i], null, electricForce, d, distance);

				d = new Pt(0, ps[i].y + vertex.rs.height / 2 - graphComponent.rc.pt.y - graphComponent.rc.rs.height);
				distance = d.getLength();
				electricForce = vertexElectricRepealCoefficient * vertex.charge / distance / distance;	
				addForcePair(vertex, null, fs[i], null, electricForce, d, distance);
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

		assert (!Double.isNaN(forceX));
		assert (!Double.isNaN(forceY));

		if (f1 != null) {
			f1.x += forceX;
			f1.y += forceY;
		}
		
		if (f2 != null) {
			f2.x -= forceX;
			f2.y -= forceY;
		}
		
		if (vertex1 != null)
			vertex1.fs.add(new Pt(forceX, forceY));

		if (vertex2 != null)
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

	/**
	 * Test.
	 */
	public static void main(String[] args) {
		double mass = 10;
		double charge = 1;
		GraphComponent graphComponent = new GraphComponent(new Rc(0, 0, 600, 600));

		if (false) {
			Vertex vertex1 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			Vertex vertex2 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			Vertex vertex3 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			Vertex vertex4 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			Vertex vertex5 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			Vertex vertex6 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			Vertex vertex7 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			Vertex vertex8 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			graphComponent.addVertex(vertex1);
			graphComponent.addVertex(vertex2);
			graphComponent.addVertex(vertex3);
			graphComponent.addVertex(vertex4);
			graphComponent.addVertex(vertex5);
			graphComponent.addVertex(vertex6);
			graphComponent.addVertex(vertex7);
			graphComponent.addVertex(vertex8);
			graphComponent.addEdge(new Edge(vertex1, vertex2));
			graphComponent.addEdge(new Edge(vertex1, vertex3));
			graphComponent.addEdge(new Edge(vertex1, vertex5));
			graphComponent.addEdge(new Edge(vertex2, vertex4));
			graphComponent.addEdge(new Edge(vertex2, vertex6));
			graphComponent.addEdge(new Edge(vertex3, vertex4));
			graphComponent.addEdge(new Edge(vertex3, vertex7));
			graphComponent.addEdge(new Edge(vertex4, vertex8));
			graphComponent.addEdge(new Edge(vertex5, vertex6));
			graphComponent.addEdge(new Edge(vertex5, vertex7));
			graphComponent.addEdge(new Edge(vertex6, vertex8));
			graphComponent.addEdge(new Edge(vertex7, vertex8));
		}
		else {
			Vertex vertex1 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex2 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex3 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex4 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex5 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex6 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex7 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex8 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex9 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex10 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex11 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex12 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 Vertex vertex13 = new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge);
			 graphComponent.addVertex(vertex1);
			 graphComponent.addVertex(vertex2);
			 graphComponent.addVertex(vertex3);
			 graphComponent.addVertex(vertex4);
			 graphComponent.addVertex(vertex5);
			 graphComponent.addVertex(vertex6);
			 graphComponent.addVertex(vertex7);
			 graphComponent.addVertex(vertex8);
			 graphComponent.addVertex(vertex9);
			 graphComponent.addVertex(vertex10);
			 graphComponent.addVertex(vertex11);
			 graphComponent.addVertex(vertex12);
			 graphComponent.addVertex(vertex13);
			 graphComponent.addEdge(new Edge(vertex1, vertex2));
			 graphComponent.addEdge(new Edge(vertex1, vertex3));
			 graphComponent.addEdge(new Edge(vertex1, vertex4));
			 graphComponent.addEdge(new Edge(vertex2, vertex5));
			 graphComponent.addEdge(new Edge(vertex2, vertex6));
			 graphComponent.addEdge(new Edge(vertex3, vertex7));
			 graphComponent.addEdge(new Edge(vertex3, vertex8));
			 graphComponent.addEdge(new Edge(vertex4, vertex9));
			 graphComponent.addEdge(new Edge(vertex4, vertex10));
			 graphComponent.addEdge(new Edge(vertex4, vertex11));
			 graphComponent.addEdge(new Edge(vertex4, vertex12));
			 graphComponent.addEdge(new Edge(vertex4, vertex13));
		}		

		TestCanvas testCanvas = new TestCanvas(graphComponent);
		testCanvas.setSize(500, 500);
		Frame frame = new Frame();
		frame.setSize(500, 500);
		frame.add(testCanvas);
		frame.show();
		testCanvas.embed();
		
		for (Vertex vertex : graphComponent.getVertices())
			System.out.println(vertex.pt);
	}
}

class TestCanvas extends Canvas implements ForceDirectedEmbedding.IForceDirectedEmbeddingListener {
	private GraphComponent graphComponent;
	
	private ForceDirectedEmbedding embedding;

	private Vertex dragVertex;
	
	private Pt dragPt;

	private Point dragPoint;

	private boolean showForces = true;

	private boolean animateLayout = true;

	public TestCanvas(GraphComponent gc) {
		this.graphComponent = gc;
		this.addKeyListener(new KeyListener() {
			public void keyPressed(java.awt.event.KeyEvent e) {
				int delta = (e.getModifiersEx() & KeyEvent.SHIFT_DOWN_MASK) != 0 ? 5 : 1;

				if (e.getKeyChar() == 'f')
					showForces = !showForces;
				else if (e.getKeyChar() == 'a')
					animateLayout = !animateLayout;
				else if (e.getKeyCode() == 37) {
					graphComponent.rc.rs.width -= delta;
					embedding.embed();
				}
				else if (e.getKeyCode() == 39) {
					graphComponent.rc.rs.width += delta;
					embedding.embed();
				}
				else if (e.getKeyCode() == 38) {
					graphComponent.rc.rs.height -= delta;
					embedding.embed();
				}
				else if (e.getKeyCode() == 40) {
					graphComponent.rc.rs.height += delta;
					embedding.embed();
				}
				else if (e.getKeyCode() == 27)
					System.exit(0);
				
				repaint();
			}

			public void keyReleased(KeyEvent e) {
			}

			public void keyTyped(KeyEvent e) {
			}
		});
		this.addMouseListener(new MouseListener() {
			public void mouseClicked(MouseEvent e) {
			}

			public void mouseEntered(MouseEvent e) {
			}

			public void mouseExited(MouseEvent e) {
			}

			public void mousePressed(MouseEvent e) {
				if (dragVertex == null)
					for (Vertex vertex : graphComponent.getVertices())
						if (new Rc(vertex.pt, vertex.rs).contains(new Pt(e.getX(), e.getY()))) {
							dragVertex = vertex;
							dragPoint = e.getPoint();
							dragPt = vertex.pt.copy();
						}
			}

			public void mouseReleased(MouseEvent e) {
				dragVertex = null;
			}
		});
		this.addMouseMotionListener(new MouseMotionListener() {
			public void mouseDragged(MouseEvent e) {
				Pt fixPt = new Pt(dragPt.x + e.getX() - dragPoint.getX(), dragPt.y + e.getY() - dragPoint.getY());
				dragVertex.positionConstraint = new FixPositionConstraint(fixPt);
				
				embed();
			}

			public void mouseMoved(MouseEvent e) {
			}
		});

		embedding = new ForceDirectedEmbedding(graphComponent, this);
	}

	public void embed() {
		embedding.embed();
		repaint();
	}

	public void positionsChanged() {
		if (animateLayout) {
			repaint();

			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
	
	@Override
	public void paint(Graphics g)
	{
		int forceScale =5;
		double centerX = 0;//getWidth() / 2.0;
		double centerY = 0;//getHeight() / 2.0;
		g.setColor(new Color(0, 0, 0));
		
		int x = (int)(graphComponent.rc.pt.x + centerX);
		int y = (int)(graphComponent.rc.pt.y + centerY);
		int width = (int)graphComponent.rc.rs.width;
		int height = (int)graphComponent.rc.rs.height;
		g.drawRect(x, y, width, height);
		
		for (Vertex vertex : graphComponent.getVertices()) {
			x = (int)(vertex.pt.x + centerX);
			y = (int)(vertex.pt.y + centerY);
			width = (int)vertex.rs.width;
			height = (int)vertex.rs.height;
			g.setColor(new Color(0, 0, 0));
			g.drawRect(x, y, width, height);

			g.setColor(new Color(255, 0, 0));

			if (showForces)
				for (Pt f : vertex.fs) {
					int x1 = (int)(vertex.pt.x + centerX + vertex.rs.height / 2);
					int y1 = (int)(vertex.pt.y + centerY + vertex.rs.height / 2);
					int x2 = (int)(vertex.pt.x + centerX + vertex.rs.height / 2 + f.x * forceScale);
					int y2 = (int)(vertex.pt.y + centerY + vertex.rs.height / 2 + f.y * forceScale);
	
					g.drawLine(x1, y1, x2, y2);
					g.drawLine(x1 + 1, y1, x2 + 1, y2);
					g.drawLine(x1, y1 + 1, x2, y2 + 1);
				}
		}
		
		for (Edge edge : graphComponent.getEdges()) {
			int x1 = (int)(edge.source.pt.x + centerX + edge.source.rs.height / 2);
			int y1 = (int)(edge.source.pt.y + centerY + edge.source.rs.height / 2);
			int x2 = (int)(edge.target.pt.x + centerX + edge.target.rs.height / 2);
			int y2 = (int)(edge.target.pt.y + centerY + edge.target.rs.height / 2);
			g.setColor(new Color(0, 0, 0));
			g.drawLine(x1, y1, x2, y2);
		}
	}
}

class Pt {
	public double x;

	public double y;
	
	public Pt(double x, double y) {
		this.x = x;
		this.y = y;
	}
	
	public Pt(Pt pt) {
		x = pt.x;
		y = pt.y;
	}

	public Pt copy() {
		return new Pt(x, y);
	}
	
	public Pt assign(Pt pt) {
		x = pt.x;
		y = pt.y;
		
		return this;
	}
	
	public void setZero() {
		x = 0;
		y = 0;
	}

	public double getLength() {
		return Math.sqrt(x * x + y * y);
	}
	
	public double getDistance(Pt other) {
		double dx = x - other.x;
		double dy = y - other.y;

		return Math.sqrt(dx * dx + dy * dy);
	}
	
	public Pt Normalize() {
		double length = getLength();
		x /= length;
		y /= length;

		return this;
	}
	
	public Pt add(Pt pt) {
		x += pt.x;
		y += pt.y;

		return this;
	}

	public Pt subtract(Pt pt) {
		x -= pt.x;
		y -= pt.y;

		return this;
	}

	public Pt multiply(double d) {
		x *= d;
		y *= d;

		return this;
	}
	
	public Pt divide(double d) {
		x /= d;
		y /= d;

		return this;
	}

	public double crossProduct(Pt pt) {
		return x * pt.y - y * pt.x;
	}
	
	public static Pt getNil() {
		return new Pt(Double.NaN, Double.NaN);
	}

	public boolean isNil() {
		return Double.isNaN(x) && Double.isNaN(y);
	}

	@Override
	public String toString() {
		return "x: " + x + " y: " + y;
	}
}

class Rs {
	public double width;

	public double height;

	public Rs(double width, double height) {
		this.width = width;
		this.height = height;
	}
	
	public static Rs getNil() {
		return new Rs(Double.NaN, Double.NaN);
	}

	public boolean isNil() {
		return Double.isNaN(width) && Double.isNaN(height);
	}

	@Override
	public String toString() {
		return "width: " + width + " height: " + height;
	}
}

class Rc {
	public Pt pt;
	
	public Rs rs;

	public Rc(double x, double y, double width, double height) {
		this.pt = new Pt(x, y);
		this.rs = new Rs(width, height);
	}

	public Rc(Pt pt, Rs rs) {
		this.pt = pt;
		this.rs = rs;
	}

	public boolean intersects(Rc rc2) {
		return rc2.contains(getTopLeft()) || rc2.contains(getTopRight()) || rc2.contains(getBottomLeft()) || rc2.contains(getBottomRight());
	}

	public boolean contains(Pt p) {
		return pt.x <= p.x && p.x <= pt.x + rs.width && pt.y <= p.y && p.y <= pt.y + rs.height;
	}

	public Pt getTopLeft() {
		return new Pt(pt.x, pt.y);
	}

	public Pt getTopRight() {
		return new Pt(pt.x + rs.width, pt.y);
	}

	public Pt getBottomLeft() {
		return new Pt(pt.x, pt.y + rs.height);
	}

	public Pt getBottomRight() {
		return new Pt(pt.x + rs.width, pt.y + rs.height);
	}
	
	public static Rc getNil() {
		return new Rc(Pt.getNil(), Rs.getNil());
	}

	public boolean isNil() {
		return pt.isNil() && rs.isNil();
	}
}

class GraphComponent {
	public Rc rc;
	
	private ArrayList<Vertex> vertices = new ArrayList<Vertex>();
	
	private ArrayList<Edge> edges = new ArrayList<Edge>();

	public GraphComponent(Rc rc) {
		this.rc = rc;
	}
	
	public void addVertex(Vertex vertex) {
		vertices.add(vertex);
	}
	
	public void addEdge(Edge edge) {
		edges.add(edge);
	}

	public int getVertexCount() {
		return vertices.size();
	}

	public int IndexOfVertex(Vertex vertex) {
		return vertices.indexOf(vertex);
	}

	public ArrayList<Vertex> getVertices() {
		return vertices;
	}

	public Vertex getVertex(int index) {
		return vertices.get(index);
	}

	public int getEdgeCount() {
		return edges.size();
	}

	public ArrayList<Edge> getEdges() {
		return edges;
	}

	public Edge getEdge(int index) {
		return edges.get(index);
	}
}

interface IVertexPositionConstraint {
	Pt getForce(Pt pt);
	
	Pt getFinalPosition(Pt pt);
}

class FixPositionConstraint implements IVertexPositionConstraint {
	public Pt pt;

	public FixPositionConstraint(Pt pt) {
		this.pt = pt;
	}

	public Pt getForce(Pt pt) {
		Pt d = this.pt.copy().subtract(pt);
		
		return d;
	}

	public Pt getFinalPosition(Pt pt) {
		return this.pt;
	}
}

class Vertex {
	public Pt pt;

	public Rs rs;
	
	public double mass;
	
	public double charge;
	
	public ArrayList<Pt> fs = new ArrayList<Pt>();
	
	public IVertexPositionConstraint positionConstraint;

	public Vertex() {
	}
	
	public Vertex(Pt pt, Rs rc, double mass, double charge) {
		this.pt = pt;
		this.rs = rc;
		this.mass = mass;
		this.charge = charge;
	}

	public Pt getCenter() {
		return new Pt(pt.x + rs.width / 2, pt.y + rs.height / 2);
	}
}

class Edge {
	public Vertex source;
	
	public Vertex target;
	
	public Edge() {
	}

	public Edge(Vertex source, Vertex target) {
		this.source = source;
		this.target = target;
	}
}