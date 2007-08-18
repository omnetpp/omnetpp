package org.omnetpp.figures.layout;

/*
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
import java.util.List;
import java.util.Random;
*/

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class EmbeddingTest
{
/*	public static void main(String[] args) {
		new EmbeddingTest().run();
	}


	private void addTree(ForceDirectedEmbedding embedding) {
		IBody body1 = new Body(new Variable(Pt.newNil()));
		IBody body2 = new Body(new Variable(Pt.newNil()));
		IBody body3 = new Body(new Variable(Pt.newNil()));
		IBody body4 = new Body(new Variable(Pt.newNil()));
		IBody body5 = new Body(new Variable(Pt.newNil()));
		IBody body6 = new Body(new Variable(Pt.newNil()));
		IBody body7 = new Body(new Variable(Pt.newNil()));
		IBody body8 = new Body(new Variable(Pt.newNil()));
		IBody body9 = new Body(new Variable(Pt.newNil()));
		IBody body10 = new Body(new Variable(Pt.newNil()));
		IBody body11 = new Body(new Variable(Pt.newNil()));
		IBody body12 = new Body(new Variable(Pt.newNil()));
		IBody body13 = new Body(new Variable(Pt.newNil()));
		embedding.addBody(body1);
		embedding.addBody(body2);
		embedding.addBody(body3);
		embedding.addBody(body4);
		embedding.addBody(body5);
		embedding.addBody(body6);
		embedding.addBody(body7);
		embedding.addBody(body8);
		embedding.addBody(body9);
		embedding.addBody(body10);
		embedding.addBody(body11);
		embedding.addBody(body12);
		embedding.addBody(body13);
		embedding.addForceProvider(new Spring(body1, body2));
		embedding.addForceProvider(new Spring(body1, body3));
		embedding.addForceProvider(new Spring(body1, body4));
		embedding.addForceProvider(new Spring(body2, body5));
		embedding.addForceProvider(new Spring(body2, body6));
		embedding.addForceProvider(new Spring(body3, body7));
		embedding.addForceProvider(new Spring(body3, body8));
		embedding.addForceProvider(new Spring(body4, body9));
		embedding.addForceProvider(new Spring(body4, body10));
		embedding.addForceProvider(new Spring(body4, body11));
		embedding.addForceProvider(new Spring(body4, body12));
		embedding.addForceProvider(new Spring(body4, body13));
	}

	private void addCube(ForceDirectedEmbedding embedding) {
		IBody body1 = new Body(new Variable(Pt.newNil()));
		IBody body2 = new Body(new Variable(Pt.newNil()));
		IBody body3 = new Body(new Variable(Pt.newNil()));
		IBody body4 = new Body(new Variable(Pt.newNil()));
		IBody body5 = new Body(new Variable(Pt.newNil()));
		IBody body6 = new Body(new Variable(Pt.newNil()));
		IBody body7 = new Body(new Variable(Pt.newNil()));
		IBody body8 = new Body(new Variable(Pt.newNil()));
		embedding.addBody(body1);
		embedding.addBody(body2);
		embedding.addBody(body3);
		embedding.addBody(body4);
		embedding.addBody(body5);
		embedding.addBody(body6);
		embedding.addBody(body7);
		embedding.addBody(body8);
		embedding.addForceProvider(new Spring(body1, body2));
		embedding.addForceProvider(new Spring(body1, body3));
		embedding.addForceProvider(new Spring(body1, body5));
		embedding.addForceProvider(new Spring(body2, body4));
		embedding.addForceProvider(new Spring(body2, body6));
		embedding.addForceProvider(new Spring(body3, body4));
		embedding.addForceProvider(new Spring(body3, body7));
		embedding.addForceProvider(new Spring(body4, body8));
		embedding.addForceProvider(new Spring(body5, body6));
		embedding.addForceProvider(new Spring(body5, body7));
		embedding.addForceProvider(new Spring(body6, body8));
		embedding.addForceProvider(new Spring(body7, body8));
	}

	private void addWireless(ForceDirectedEmbedding embedding, int count) {
		for (int i = 0; i < count; i++)
			embedding.addBody(new Body(new Variable(Pt.newNil())));
	}

	private void addLine(ForceDirectedEmbedding embedding, int count) {
		IBody previous = null;

		for (int i = 0; i < count; i++) {
			IBody body = new Body(new Variable(Pt.newNil()));
			embedding.addBody(body);

			if (previous != null)
				embedding.addForceProvider(new Spring(previous, body));

			previous = body;
		}
	}

	private void addAnchoredLine(ForceDirectedEmbedding embedding, int count) {
		Variable anchor = new Variable(new Pt(50, 50));

		for (int i = 0; i < count; i++) {
			IBody body = new RelativelyPositionedBody(anchor, new Pt((i - count / 2.0) * 20, 0));
			embedding.addBody(body);
		}
	}

	private void addCircle(ForceDirectedEmbedding embedding, int count) {
		IBody first = null;
		IBody previous = null;

		for (int i = 0; i < count; i++) {
			IBody body = new Body(new Variable(Pt.newNil()));

			if (first == null)
				first = body;

			embedding.addBody(body);

			if (previous != null)
				embedding.addForceProvider(new Spring(previous, body));

			previous = body;
		}

		embedding.addForceProvider(new Spring(previous, first));
	}

	private void addAnchoredCircle(ForceDirectedEmbedding embedding, int count) {
		Pt origin = new Pt(150, 150);
		Variable anchor = new Variable(origin);

		for (int i = 0; i < count; i++) {
			IBody body = new RelativelyPositionedBody(anchor, Pt.newRadialPt(50, 2 * Math.PI * i / count));
			embedding.addBody(body);
		}
	}

	private void addBorder(ForceDirectedEmbedding embedding) {
		IBody top = new BorderBody(true, 0);
		IBody bottom = new BorderBody(true, 300);
		IBody left = new BorderBody(false, 0);
		IBody right = new BorderBody(false, 300);

		for (IBody body : embedding.getBodies()) {
			embedding.addForceProvider(new VerticalElectricRepeal(top, body));
			embedding.addForceProvider(new VerticalElectricRepeal(bottom, body));
			embedding.addForceProvider(new HorizontalElectricRepeal(left, body));
			embedding.addForceProvider(new HorizontalElectricRepeal(right, body));
		}

		embedding.addBody(top);
		embedding.addBody(bottom);
		embedding.addBody(left);
		embedding.addBody(right);
		embedding.addForceProvider(new VerticalSpring(top, bottom));
		embedding.addForceProvider(new HorizonalSpring(left, right));
	}

	private void addElectricRepeals(ForceDirectedEmbedding embedding) {
		List<IBody> bodies = embedding.getBodies();

		for (int i = 0; i < bodies.size(); i++)
			for (int j = i + 1; j < bodies.size(); j++) {
				IBody body1 = bodies.get(i);
				IBody body2 = bodies.get(j);

				if (!(body1 instanceof BorderBody) &&
					!(body2 instanceof BorderBody) &&
					body1.getVariable() != body2.getVariable())
					embedding.addForceProvider(new ElectricRepeal(body1, body2));
			}
	}

	private void addRandomPositions(ForceDirectedEmbedding embedding) {
		Random random = new Random();

		for (Variable variable : embedding.getVariables())
			if (variable.getPosition().isNil())
				variable.assignPosition(new Pt(random.nextDouble() * 300, random.nextDouble() * 300));
	}

	private void addStrange(ForceDirectedEmbedding embedding) {
		Variable v1 = new Variable(new Pt(150, 100));
		IBody b1 = new RelativelyPositionedBody(v1, new Pt(-50, 0));
		IBody b2 = new Body(new Variable(new Pt(100, 200)));
		IBody b3 = new Body(new Variable(new Pt(200, 200)));
		IBody b4 = new RelativelyPositionedBody(v1, new Pt(50, 0));
		embedding.addBody(b1);
		embedding.addBody(b2);
		embedding.addBody(b3);
		embedding.addBody(b4);
		embedding.addForceProvider(new Spring(b1, b2));
		embedding.addForceProvider(new Spring(b2, b3));
		embedding.addForceProvider(new Spring(b3, b4));
	}

	private void run() {
		ForceDirectedEmbedding embedding = new ForceDirectedEmbedding();

	 	//addNode60();
	 	//addWireless(60);
		//addLine(6);
		addCircle(embedding, 6);
		//addAnchoredLine(6);

		//graphComponent.calculateSpanningTree();

		//new RandomEmbedding(graphComponent, new Rc(0, 0, 500, 500)).embed();
		//new StripTreeEmbedding(graphComponent, 0, 0, 0, false).embed();
		//new StripTreeEmbedding(graphComponent, 40, 40, 40, false).embed();
		//new StarTreeEmbedding(graphComponent, 40).embed();

		//addStrange(embedding);
		//addCube(embedding);
	 	//addTree(embedding);
		//addAnchoredLine(embedding, 4);
		//addAnchoredCircle(embedding, 8);

		IBody body = new Body(new Variable(Pt.newNil()));
		embedding.addBody(body);
		//embedding.addForceProvider(new PointConstraint(body, 1, new Pt(500, 500)));
		//embedding.addForceProvider(new LineConstraint(body, 1, new Ln(0, 500, 500, 0)));
		embedding.addForceProvider(new CircleConstraint(body, 10, new Cc(200, 200, 200)));
		embedding.addForceProvider(new Spring(body, embedding.getBodies().get(0)));
		embedding.addForceProvider(new LineConstraint(embedding.getBodies().get(3), 10, new Ln(0, 500, 500, 0)));

		addElectricRepeals(embedding);
		addBorder(embedding);
		addRandomPositions(embedding);
		embedding.addForceProvider(new Friction());

		TestFrame2 testFrame = new TestFrame2(embedding, 1000, 800);
		testFrame.embed();
	}
	*/
}
/*
class TestCanvas2 extends Canvas implements ForceDirectedEmbedding.IForceDirectedEmbeddingListener {
	private static final long serialVersionUID = 1L;

	private boolean animateLayout = true;

	private boolean showForces = true;

	private boolean showCircles = true;

	private ForceDirectedEmbedding embedding;

	private IBody dragBody;

	private Pt dragPt;

	private Point dragPoint;

	private double centerX = 0;

	private double centerY = 0;

	public TestCanvas2(ForceDirectedEmbedding e) {
		embedding = e;
		embedding.listener = this;

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
					centerX += delta;
					//embedding.embed();
				}
				else if (e.getKeyCode() == 39) {
					centerX -= delta;
					//embedding.embed();
				}
				else if (e.getKeyCode() == 38) {
					centerY += delta;
					//embedding.embed();
				}
				else if (e.getKeyCode() == 40) {
					centerY -= delta;
					//embedding.embed();
				}
				else if (e.getKeyCode() == 27) {
					((TestFrame2)(getParent())).dispose();
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
				// TODO: shitf event into the right coordinate system
				if (dragBody == null)
					for (IBody body : embedding.getBodies())
						if (new Rc(body.getPosition(), body.getSize()).contains(new Pt(e.getX(), e.getY()))) {
							dragBody = body;
							dragPoint = e.getPoint();
							dragPt = body.getPosition().copy();
						}
			}

			public void mouseReleased(MouseEvent e) {
				dragBody = null;
			}
		});
		this.addMouseMotionListener(new MouseMotionListener() {
			private long last;

			public void mouseDragged(MouseEvent e) {
				if (dragPoint != null && dragPt != null) {
					long begin = System.currentTimeMillis();
					Pt fixPt = new Pt(dragPt.x + e.getX() - dragPoint.getX(), dragPt.y + e.getY() - dragPoint.getY());

					if (begin - last > 100)
					{
						for (IForceProvider forceProvider : embedding.getForceProviders())
							if (forceProvider instanceof BodyConstraint) {
								BodyConstraint bodyConstraint = (BodyConstraint)forceProvider;

								if (bodyConstraint.getBody() == dragBody)
									embedding.removeForceProvider(forceProvider);
							}

						embedding.addForceProvider(new PointConstraint(dragBody, 1, fixPt));
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

	public void variablesChanged() {
		if (animateLayout) {
			repaint();

			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();  //FIXME log it or something. Just "print" is not OK!
			}
		}
	}

	@Override
	public void paint(Graphics g)
	{
		Rectangle r = g.getClipBounds();
		int x, y, width, height;

		g.setColor(new Color(0, 0, 0));

		for (IBody body : embedding.getBodies()) {
			x = (int)(body.getPosition().x + centerX - body.getSize().width / 2);
			y = (int)(body.getPosition().y + centerY - body.getSize().height / 2);
			width = (int)body.getSize().width;
			height = (int)body.getSize().height;

			if (body instanceof BorderBody) {
				if (Double.isNaN(body.getPosition().x)) {
					x = r.x;
					width = r.width;
				}

				if (Double.isNaN(body.getPosition().y)) {
					y = r.y;
					height = r.height;
				}
			}

			g.setColor(new Color(0, 0, 0));
			g.drawRect(x, y, width, height);
		}

		if (showForces) {
			g.setColor(new Color(255, 0, 0));

			for (Variable variable : embedding.getVariables()) {
				for (int i = 0; i < variable.getForces().size(); i++) {
					int forceScale1 = 0;
					int forceScale2 = 5;
					Pt f = variable.getForces().get(i);
					int x1 = (int)(variable.getPosition().x + centerX - f.x * forceScale1);
					int y1 = (int)(variable.getPosition().y + centerY - f.y * forceScale1);
					int x2 = (int)(variable.getPosition().x + centerX + f.x * forceScale2);
					int y2 = (int)(variable.getPosition().y + centerY + f.y * forceScale2);

					g.drawLine(x1, y1, x2, y2);
					g.drawLine(x1 + 1, y1, x2 + 1, y2);
					g.drawLine(x1, y1 + 1, x2, y2 + 1);
				}
			}
		}

		for (IForceProvider forceProvider : embedding.getForceProviders()) {
			if (forceProvider instanceof ISpring) {
				ISpring spring = (ISpring)forceProvider;
				int x1 = (int)(spring.getBody1().getPosition().x + centerX);
				int y1 = (int)(spring.getBody1().getPosition().y + centerY);
				int x2 = (int)(spring.getBody2().getPosition().x + centerX);
				int y2 = (int)(spring.getBody2().getPosition().y + centerY);
				g.setColor(new Color(0, 0, 0));
				g.drawLine(x1, y1, x2, y2);
			}
		}

		g.drawLine(0, 500, 500, 0);
		drawCircle(g, new Cc(200, 200, 200));
	}

	private void drawCircle(Graphics g, Cc cc) {
		int x = (int)(cc.origin.x - cc.radius + centerX);
		int y = (int)(cc.origin.y - cc.radius + centerY);
		int width = (int)cc.radius * 2 + 1;
		int height = (int)cc.radius * 2 + 1;
		g.drawOval(x, y, width, height);
	}
}

class TestFrame2 extends Frame {
	private static final long serialVersionUID = 1L;

	private TestCanvas2 testCanvas;

	public TestFrame2(ForceDirectedEmbedding embedding, int width, int height) {
		testCanvas = new TestCanvas2(embedding);
		testCanvas.setSize(width, height);
		setSize(width, height);
		add(testCanvas);
		setVisible(true);
	}

	public void embed() {
		testCanvas.embed();
	}
}
*/