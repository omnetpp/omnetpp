package org.omnetpp.figures.layout;

import java.awt.Canvas;
import java.awt.Color;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

public class EmbeddingTest
{
	private double mass = 10;
	private double charge = 1;
	private GraphComponent graphComponent = new GraphComponent();

	public static void main(String[] args) {
		new EmbeddingTest().run();
	}

	private void add333() {
		Vertex vertex1 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex2 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex3 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex4 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex5 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex6 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex7 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex8 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex9 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		graphComponent.addVertex(vertex1);
		graphComponent.addVertex(vertex2);
		graphComponent.addVertex(vertex3);
		graphComponent.addVertex(vertex4);
		graphComponent.addVertex(vertex5);
		graphComponent.addVertex(vertex6);
		graphComponent.addVertex(vertex7);
		graphComponent.addVertex(vertex8);
		graphComponent.addVertex(vertex9);
		graphComponent.addEdge(new Edge(vertex1, vertex4));
		graphComponent.addEdge(new Edge(vertex1, vertex5));
		graphComponent.addEdge(new Edge(vertex1, vertex6));
		graphComponent.addEdge(new Edge(vertex1, vertex7));
		graphComponent.addEdge(new Edge(vertex1, vertex8));
		graphComponent.addEdge(new Edge(vertex1, vertex9));
		graphComponent.addEdge(new Edge(vertex2, vertex3));
		graphComponent.addEdge(new Edge(vertex2, vertex7));
		graphComponent.addEdge(new Edge(vertex2, vertex8));
		graphComponent.addEdge(new Edge(vertex2, vertex9));
		graphComponent.addEdge(new Edge(vertex3, vertex4));
		graphComponent.addEdge(new Edge(vertex3, vertex5));
		graphComponent.addEdge(new Edge(vertex3, vertex6));
	}

	private void addTree() {
		Vertex vertex1 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex2 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex3 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex4 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex5 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex6 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex7 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex8 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex9 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex10 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex11 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex12 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex13 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
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

	private void addCube() {
		Vertex vertex1 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex2 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex3 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex4 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex5 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex6 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex7 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
		Vertex vertex8 = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
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
	
	private void addNode60() {
		Vertex[] vertices = new Vertex[60];
		
		for (int i = 0; i < vertices.length; i++)
			graphComponent.addVertex(vertices[i] = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge));

		graphComponent.addEdge(new Edge(vertices[0], vertices[1]));
		graphComponent.addEdge(new Edge(vertices[0], vertices[1]));
		
		graphComponent.addEdge(new Edge(vertices[1], vertices[2]));
		graphComponent.addEdge(new Edge(vertices[1], vertices[2]));
		
		graphComponent.addEdge(new Edge(vertices[1], vertices[4]));
		graphComponent.addEdge(new Edge(vertices[1], vertices[4]));
		
		graphComponent.addEdge(new Edge(vertices[3], vertices[4]));
		graphComponent.addEdge(new Edge(vertices[3], vertices[4]));
		
		graphComponent.addEdge(new Edge(vertices[4], vertices[5]));
		graphComponent.addEdge(new Edge(vertices[4], vertices[5]));
		
		graphComponent.addEdge(new Edge(vertices[4], vertices[7]));
		graphComponent.addEdge(new Edge(vertices[4], vertices[7]));
		
		graphComponent.addEdge(new Edge(vertices[5], vertices[6]));
		graphComponent.addEdge(new Edge(vertices[5], vertices[6]));
		
		graphComponent.addEdge(new Edge(vertices[5], vertices[10]));
		graphComponent.addEdge(new Edge(vertices[5], vertices[10]));
		
		graphComponent.addEdge(new Edge(vertices[6], vertices[7]));
		graphComponent.addEdge(new Edge(vertices[6], vertices[7]));
		
		graphComponent.addEdge(new Edge(vertices[6], vertices[9]));
		graphComponent.addEdge(new Edge(vertices[6], vertices[9]));
		
		graphComponent.addEdge(new Edge(vertices[7], vertices[8]));
		graphComponent.addEdge(new Edge(vertices[7], vertices[8]));
		
		graphComponent.addEdge(new Edge(vertices[7], vertices[12]));
		graphComponent.addEdge(new Edge(vertices[7], vertices[12]));
		
		graphComponent.addEdge(new Edge(vertices[9], vertices[11]));
		graphComponent.addEdge(new Edge(vertices[9], vertices[11]));
		
		graphComponent.addEdge(new Edge(vertices[10], vertices[11]));
		graphComponent.addEdge(new Edge(vertices[10], vertices[11]));
		
		graphComponent.addEdge(new Edge(vertices[10], vertices[13]));
		graphComponent.addEdge(new Edge(vertices[10], vertices[13]));
		
		graphComponent.addEdge(new Edge(vertices[11], vertices[12]));
		graphComponent.addEdge(new Edge(vertices[11], vertices[12]));
		
		graphComponent.addEdge(new Edge(vertices[11], vertices[14]));
		graphComponent.addEdge(new Edge(vertices[11], vertices[14]));
		
		graphComponent.addEdge(new Edge(vertices[12], vertices[15]));
		graphComponent.addEdge(new Edge(vertices[12], vertices[15]));
		
		graphComponent.addEdge(new Edge(vertices[13], vertices[18]));
		graphComponent.addEdge(new Edge(vertices[13], vertices[18]));
		
		graphComponent.addEdge(new Edge(vertices[14], vertices[15]));
		graphComponent.addEdge(new Edge(vertices[14], vertices[15]));
		
		graphComponent.addEdge(new Edge(vertices[14], vertices[22]));
		graphComponent.addEdge(new Edge(vertices[14], vertices[22]));
		
		graphComponent.addEdge(new Edge(vertices[15], vertices[16]));
		graphComponent.addEdge(new Edge(vertices[15], vertices[16]));
		
		graphComponent.addEdge(new Edge(vertices[16], vertices[17]));
		graphComponent.addEdge(new Edge(vertices[16], vertices[17]));
		
		graphComponent.addEdge(new Edge(vertices[16], vertices[23]));
		graphComponent.addEdge(new Edge(vertices[16], vertices[23]));
		
		graphComponent.addEdge(new Edge(vertices[18], vertices[19]));
		graphComponent.addEdge(new Edge(vertices[18], vertices[19]));
		
		graphComponent.addEdge(new Edge(vertices[18], vertices[21]));
		graphComponent.addEdge(new Edge(vertices[18], vertices[21]));
		
		graphComponent.addEdge(new Edge(vertices[19], vertices[20]));
		graphComponent.addEdge(new Edge(vertices[19], vertices[20]));
		
		graphComponent.addEdge(new Edge(vertices[19], vertices[26]));
		graphComponent.addEdge(new Edge(vertices[19], vertices[26]));
		
		graphComponent.addEdge(new Edge(vertices[20], vertices[21]));
		graphComponent.addEdge(new Edge(vertices[20], vertices[21]));
		
		graphComponent.addEdge(new Edge(vertices[20], vertices[24]));
		graphComponent.addEdge(new Edge(vertices[20], vertices[24]));
		
		graphComponent.addEdge(new Edge(vertices[20], vertices[25]));
		graphComponent.addEdge(new Edge(vertices[20], vertices[25]));
		
		graphComponent.addEdge(new Edge(vertices[21], vertices[22]));
		graphComponent.addEdge(new Edge(vertices[21], vertices[22]));
		
		graphComponent.addEdge(new Edge(vertices[21], vertices[23]));
		graphComponent.addEdge(new Edge(vertices[21], vertices[23]));
		
		graphComponent.addEdge(new Edge(vertices[21], vertices[24]));
		graphComponent.addEdge(new Edge(vertices[21], vertices[24]));
		
		graphComponent.addEdge(new Edge(vertices[23], vertices[24]));
		graphComponent.addEdge(new Edge(vertices[23], vertices[24]));
		
		graphComponent.addEdge(new Edge(vertices[24], vertices[28]));
		graphComponent.addEdge(new Edge(vertices[24], vertices[28]));
		
		graphComponent.addEdge(new Edge(vertices[25], vertices[27]));
		graphComponent.addEdge(new Edge(vertices[25], vertices[27]));
		
		graphComponent.addEdge(new Edge(vertices[26], vertices[32]));
		graphComponent.addEdge(new Edge(vertices[26], vertices[32]));
		
		graphComponent.addEdge(new Edge(vertices[27], vertices[29]));
		graphComponent.addEdge(new Edge(vertices[27], vertices[29]));
		
		graphComponent.addEdge(new Edge(vertices[27], vertices[30]));
		graphComponent.addEdge(new Edge(vertices[27], vertices[30]));
		
		graphComponent.addEdge(new Edge(vertices[27], vertices[32]));
		graphComponent.addEdge(new Edge(vertices[27], vertices[32]));
		
		graphComponent.addEdge(new Edge(vertices[28], vertices[29]));
		graphComponent.addEdge(new Edge(vertices[28], vertices[29]));
		
		graphComponent.addEdge(new Edge(vertices[28], vertices[31]));
		graphComponent.addEdge(new Edge(vertices[28], vertices[31]));
		
		graphComponent.addEdge(new Edge(vertices[29], vertices[31]));
		graphComponent.addEdge(new Edge(vertices[29], vertices[31]));
		
		graphComponent.addEdge(new Edge(vertices[30], vertices[36]));
		graphComponent.addEdge(new Edge(vertices[30], vertices[36]));
		
		graphComponent.addEdge(new Edge(vertices[31], vertices[33]));
		graphComponent.addEdge(new Edge(vertices[31], vertices[33]));
		
		graphComponent.addEdge(new Edge(vertices[32], vertices[34]));
		graphComponent.addEdge(new Edge(vertices[32], vertices[34]));
		
		graphComponent.addEdge(new Edge(vertices[33], vertices[37]));
		graphComponent.addEdge(new Edge(vertices[33], vertices[37]));
		
		graphComponent.addEdge(new Edge(vertices[34], vertices[35]));
		graphComponent.addEdge(new Edge(vertices[34], vertices[35]));
		
		graphComponent.addEdge(new Edge(vertices[34], vertices[40]));
		graphComponent.addEdge(new Edge(vertices[34], vertices[40]));
		
		graphComponent.addEdge(new Edge(vertices[35], vertices[36]));
		graphComponent.addEdge(new Edge(vertices[35], vertices[36]));
		
		graphComponent.addEdge(new Edge(vertices[35], vertices[39]));
		graphComponent.addEdge(new Edge(vertices[35], vertices[39]));
		
		graphComponent.addEdge(new Edge(vertices[36], vertices[37]));
		graphComponent.addEdge(new Edge(vertices[36], vertices[37]));
		
		graphComponent.addEdge(new Edge(vertices[37], vertices[38]));
		graphComponent.addEdge(new Edge(vertices[37], vertices[38]));
		
		graphComponent.addEdge(new Edge(vertices[38], vertices[39]));
		graphComponent.addEdge(new Edge(vertices[38], vertices[39]));
		
		graphComponent.addEdge(new Edge(vertices[38], vertices[42]));
		graphComponent.addEdge(new Edge(vertices[38], vertices[42]));
		
		graphComponent.addEdge(new Edge(vertices[39], vertices[40]));
		graphComponent.addEdge(new Edge(vertices[39], vertices[40]));
		
		graphComponent.addEdge(new Edge(vertices[39], vertices[41]));
		graphComponent.addEdge(new Edge(vertices[39], vertices[41]));
		
		graphComponent.addEdge(new Edge(vertices[40], vertices[43]));
		graphComponent.addEdge(new Edge(vertices[40], vertices[43]));
		
		graphComponent.addEdge(new Edge(vertices[40], vertices[44]));
		graphComponent.addEdge(new Edge(vertices[40], vertices[44]));
		
		graphComponent.addEdge(new Edge(vertices[40], vertices[52]));
		graphComponent.addEdge(new Edge(vertices[40], vertices[52]));
		
		graphComponent.addEdge(new Edge(vertices[41], vertices[42]));
		graphComponent.addEdge(new Edge(vertices[41], vertices[42]));
		
		graphComponent.addEdge(new Edge(vertices[41], vertices[43]));
		graphComponent.addEdge(new Edge(vertices[41], vertices[43]));
		
		graphComponent.addEdge(new Edge(vertices[42], vertices[43]));
		graphComponent.addEdge(new Edge(vertices[42], vertices[43]));
		
		graphComponent.addEdge(new Edge(vertices[43], vertices[46]));
		graphComponent.addEdge(new Edge(vertices[43], vertices[46]));
		
		graphComponent.addEdge(new Edge(vertices[44], vertices[45]));
		graphComponent.addEdge(new Edge(vertices[44], vertices[45]));
		
		graphComponent.addEdge(new Edge(vertices[44], vertices[47]));
		graphComponent.addEdge(new Edge(vertices[44], vertices[47]));
		
		graphComponent.addEdge(new Edge(vertices[45], vertices[46]));
		graphComponent.addEdge(new Edge(vertices[45], vertices[46]));
		
		graphComponent.addEdge(new Edge(vertices[46], vertices[49]));
		graphComponent.addEdge(new Edge(vertices[46], vertices[49]));
		
		graphComponent.addEdge(new Edge(vertices[47], vertices[48]));
		graphComponent.addEdge(new Edge(vertices[47], vertices[48]));
		
		graphComponent.addEdge(new Edge(vertices[48], vertices[50]));
		graphComponent.addEdge(new Edge(vertices[48], vertices[50]));
		
		graphComponent.addEdge(new Edge(vertices[48], vertices[51]));
		graphComponent.addEdge(new Edge(vertices[48], vertices[51]));
		
		graphComponent.addEdge(new Edge(vertices[49], vertices[50]));
		graphComponent.addEdge(new Edge(vertices[49], vertices[50]));
		
		graphComponent.addEdge(new Edge(vertices[50], vertices[53]));
		graphComponent.addEdge(new Edge(vertices[50], vertices[53]));
		
		graphComponent.addEdge(new Edge(vertices[51], vertices[52]));
		graphComponent.addEdge(new Edge(vertices[51], vertices[52]));
		
		graphComponent.addEdge(new Edge(vertices[51], vertices[53]));
		graphComponent.addEdge(new Edge(vertices[51], vertices[53]));
		
		graphComponent.addEdge(new Edge(vertices[52], vertices[53]));
		graphComponent.addEdge(new Edge(vertices[52], vertices[53]));
		
		graphComponent.addEdge(new Edge(vertices[52], vertices[54]));
		graphComponent.addEdge(new Edge(vertices[52], vertices[54]));
		
		graphComponent.addEdge(new Edge(vertices[53], vertices[56]));
		graphComponent.addEdge(new Edge(vertices[53], vertices[56]));
		
		graphComponent.addEdge(new Edge(vertices[54], vertices[55]));
		graphComponent.addEdge(new Edge(vertices[54], vertices[55]));
		
		graphComponent.addEdge(new Edge(vertices[55], vertices[56]));
		graphComponent.addEdge(new Edge(vertices[55], vertices[56]));
	}
	
	private void addWireless(int count) {
		for (int i = 0; i < count; i++)
			graphComponent.addVertex(new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge));
	}
	
	private void addLine(int count) {
		Vertex previous = null;

		for (int i = 0; i < count; i++) {
			Vertex vertex = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
			graphComponent.addVertex(vertex);
			
			if (previous != null)
				graphComponent.addEdge(new Edge(previous, vertex));

			previous = vertex;
		}
	}
	
	private void addCircle(int count) {
		Vertex first = null;
		Vertex previous = null;

		for (int i = 0; i < count; i++) {
			Vertex vertex = new Vertex(Pt.newNil(), new Rs(10, 10), mass, charge);
			
			if (first == null)
				first = vertex;

			graphComponent.addVertex(vertex);
			
			if (previous != null)
				graphComponent.addEdge(new Edge(previous, vertex));

			previous = vertex;
		}

		graphComponent.addEdge(new Edge(previous, first));
	}
	
	private void addAnchoredLine(int count) {
		Vertex anchor = new Vertex(new Pt(count / 2.0 * 20, 0), new Rs(10, 10), mass, charge);
		graphComponent.addVertex(anchor);
		Vertex previous = null;

		for (int i = 0; i < count; i++) {
			Vertex vertex = new Vertex(new Pt((i - count / 2.0) * 20, 0), new Rs(10, 10), mass, charge);
			vertex.anchor = anchor;
			graphComponent.addVertex(vertex);
			
			if (previous != null)
				graphComponent.addEdge(new Edge(previous, vertex));

			previous = vertex;
		}
	}

	private void run() {
		//addCube();
	 	//addTree();
	 	//add333();
	 	//addNode60();
	 	//addWireless(60);
		//addLine(6);
		addCircle(6);
		addAnchoredLine(6);
		//graphComponent.addVertex(new Vertex(Pt.getNil(), new Rs(10, 10), mass, charge));
		
		graphComponent.calculateSpanningTree();
		
		new RandomEmbedding(graphComponent, new Rc(0, 0, 500, 500)).embed();
		new StripTreeEmbedding(graphComponent, 0, 0, 0, false).embed();
		//new StripTreeEmbedding(graphComponent, 40, 40, 40, false).embed();
		//new StarTreeEmbedding(graphComponent, 40).embed();

		Rc rc = graphComponent.getRc();
		double d = 50;
		Vertex top = new Vertex(new Pt(Double.NaN, rc.getTop() - d), new Rs(Double.NaN, 0), mass, charge);
		Vertex bottom = new Vertex(new Pt(Double.NaN, rc.getBottom() + d), new Rs(Double.NaN, 0), mass, charge);
		Vertex left = new Vertex(new Pt(rc.getLeft() - d, Double.NaN), new Rs(0, Double.NaN), mass, charge);
		Vertex right = new Vertex(new Pt(rc.getRight() + d, Double.NaN), new Rs(0, Double.NaN), mass, charge);
		graphComponent.addVertex(top);
		graphComponent.addVertex(bottom);
		graphComponent.addVertex(left);
		graphComponent.addVertex(right);
		graphComponent.addEdge(new Edge(top, bottom, null, 0.01));
		graphComponent.addEdge(new Edge(left, right, null, 0.01));

		TestFrame testFrame = new TestFrame(new ForceDirectedEmbedding(graphComponent), 1000, 800);
		//TestFrame testFrame = new TestFrame(new StarTreeEmbedding(graphComponent, 40), 500, 500);
		//TestFrame testFrame = new TestFrame(new StripTreeEmbedding(graphComponent, 40, 10, 10, false), 500, 500);
		testFrame.embed();
	}
}

class TestCanvas extends Canvas implements ForceDirectedEmbedding.IForceDirectedEmbeddingListener {
	private static final long serialVersionUID = 1L;

	private boolean animateLayout = true;

	private boolean centerDrawing = true;

	private boolean showForces = true;

	private boolean showCircles = true;

	private GraphComponent graphComponent;

	private GraphEmbedding embedding;

	private Vertex dragVertex;

	private Pt dragPt;

	private Point dragPoint;
	
	private double centerX;

	private double centerY;

	public TestCanvas(GraphEmbedding e) {
		embedding = e;
		
		if (embedding instanceof ForceDirectedEmbedding)
			((ForceDirectedEmbedding)embedding).listener = this;

		graphComponent = e.getGraphComponent();
		this.addKeyListener(new KeyListener() {
			public void keyPressed(java.awt.event.KeyEvent e) {
				int delta = (e.getModifiersEx() & KeyEvent.SHIFT_DOWN_MASK) != 0 ? 5 : 1;

				if (e.getKeyChar() == 'f')
					showForces = !showForces;
				else if (e.getKeyChar() == 'a')
					animateLayout = !animateLayout;
				else if (e.getKeyCode() == 32)
					embedding.embed();
				else if (e.getKeyCode() == 37) {
					//graphComponent.rc.rs.width -= delta;
					embedding.embed();
				}
				else if (e.getKeyCode() == 39) {
					//graphComponent.rc.rs.width += delta;
					embedding.embed();
				}
				else if (e.getKeyCode() == 38) {
					//graphComponent.rc.rs.height -= delta;
					embedding.embed();
				}
				else if (e.getKeyCode() == 40) {
					//graphComponent.rc.rs.height += delta;
					embedding.embed();
				}
				else if (e.getKeyCode() == 27) {
					((TestFrame)(getParent())).dispose();
				}

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
			private long last;

			public void mouseDragged(MouseEvent e) {
				if (dragPoint != null && dragPt != null) {
					long begin = System.currentTimeMillis();
					Pt fixPt = new Pt(dragPt.x + e.getX() - dragPoint.getX(), dragPt.y + e.getY() - dragPoint.getY());
	
					IVertexPositionConstraint positionConstraint = dragVertex.positionConstraint;
	
					if (begin - last > 100 &&
						(positionConstraint == null ||
						!(positionConstraint instanceof PointPositionConstraint) ||
						!((PointPositionConstraint)positionConstraint).pt.equals(fixPt)))
					{
						dragVertex.positionConstraint = new PointPositionConstraint(fixPt);
						embed();
						last = begin;
					}
					long end = System.currentTimeMillis();
					System.out.println("Drag after " + (begin - last) + " in " + (end - begin) + " ms");
				}
			}

			public void mouseMoved(MouseEvent e) {
			}
		});
	}

	public void embed() {
		embedding.embed();
		repaint();
		System.gc();
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
		Rectangle r = g.getClipBounds();
		int x, y, width, height;
		centerX = centerDrawing ? getWidth() / 2.0 : 0;
		centerY = centerDrawing ? getHeight() / 2.0 : 0;

		g.setColor(new Color(0, 0, 0));

		for (Vertex vertex : graphComponent.getVertices()) {
			x = (int)(vertex.pt.x + centerX);
			y = (int)(vertex.pt.y + centerY);
			width = (int)vertex.rs.width;
			height = (int)vertex.rs.height;
			g.setColor(new Color(0, 0, 0));
			
			if (vertex.anchor != null) {
				x += vertex.anchor.pt.x;
				y += vertex.anchor.pt.y;
			}
			
			if (Double.isNaN(x))
				x = r.x;

			if (Double.isNaN(y))
				y = r.y;
			
			if (Double.isNaN(vertex.rs.width))
				width = r.width;

			if (Double.isNaN(vertex.rs.height))
				height = r.height;
			
			g.drawRect(x, y, width, height);

			g.setColor(new Color(255, 0, 0));

			if (showForces && vertex.fs.size() != 0)
				for (int i = 0; i < vertex.fs.size(); i++) {
					int forceScale = 5;
					Pt f = vertex.fs.get(i);
					int x1 = (int)(vertex.pt.x + centerX + vertex.rs.width / 2);
					int y1 = (int)(vertex.pt.y + centerY + vertex.rs.height / 2);
					int x2 = (int)(vertex.pt.x + centerX + vertex.rs.width / 2 + f.x * forceScale);
					int y2 = (int)(vertex.pt.y + centerY + vertex.rs.height / 2 + f.y * forceScale);

					g.drawLine(x1, y1, x2, y2);
					g.drawLine(x1 + 1, y1, x2 + 1, y2);
					g.drawLine(x1, y1 + 1, x2, y2 + 1);
				}
			
			if (showCircles && vertex.starTreeCircleCenter != null)
				drawCircle(g, new Cc(vertex.starTreeCircleCenter, vertex.starTreeRadius));
		}

		for (Edge edge : graphComponent.getEdges()) {
			int x1 = (int)(edge.source.pt.x + centerX + edge.source.rs.width / 2);
			int y1 = (int)(edge.source.pt.y + centerY + edge.source.rs.height / 2);
			int x2 = (int)(edge.target.pt.x + centerX + edge.target.rs.width / 2);
			int y2 = (int)(edge.target.pt.y + centerY + edge.target.rs.height / 2);
			g.setColor(new Color(0, 0, 0));
			g.drawLine(x1, y1, x2, y2);
		}
	}
	
	private void drawCircle(Graphics g, Cc cc) {
		int x = (int)(cc.origin.x - cc.radius + centerX);
		int y = (int)(cc.origin.y - cc.radius + centerY);
		int width = (int)cc.radius * 2 + 1;
		int height = (int)cc.radius * 2 + 1;
		g.drawOval(x, y, width, height);
	}
}

class TestFrame extends Frame {
	private static final long serialVersionUID = 1L;

	private TestCanvas testCanvas;

	public TestFrame(GraphEmbedding embedding, int width, int height) {
		testCanvas = new TestCanvas(embedding);
		testCanvas.setSize(width, height);
		setSize(width, height);
		add(testCanvas);
		setVisible(true);
	}
	
	public void embed() {
		testCanvas.embed();
	}
}
