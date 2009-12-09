package org.omnetpp.test.unit.figure;

import junit.framework.TestCase;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.junit.Test;
import org.omnetpp.common.Debug;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.FigureCanvas;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.ITooltipTextProvider;
import org.omnetpp.figures.NedFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.misc.FigureUtils;
import org.omnetpp.ned.model.DisplayString;

// TODO scale tests
public class SubmoduleFigureTest extends TestCase {
	static {
		ImageFactory.initialize(new String[] {System.getenv("HOME")+"/omnetpp/images"});
	}

	private boolean drawBoundingBox = true;
	private boolean antialiasing = true;

	@Test
	public void testSubmoduleFigureIcons() {
        Shell parent = new Shell(Display.getCurrent(), SWT.RESIZE | SWT.DIALOG_TRIM);
        parent.setText("SubmoduleFigureTest");
        parent.setActive();
        parent.setLayout(new FillLayout());
        parent.setSize(840, 720);
        final FigureCanvas figureCanvas = new FigureCanvas(parent,SWT.BORDER);
        FigureUtils.addTooltipSupport(figureCanvas, figureCanvas.getRootFigure());
        parent.layout(true);
        parent.setVisible(true);
        parent.setFocus();
        final boolean exitFlag[] = new boolean[1];
        figureCanvas.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseDoubleClick(MouseEvent e) {
				exitFlag[0] = true;
			}
        });


        figureCanvas.setBackground(new Color(null, 255, 255, 255));
        CompoundModuleFigure cmodule = new CompoundModuleFigure();
		figureCanvas.getRootFigure().add(cmodule);
		figureCanvas.getRootFigure().setConstraint(cmodule, new Rectangle(10,10,-1,-1));
		cmodule.setDisplayString(new DisplayString("bg=800,700"));

		int x = 0;
		int y = 50; int rowDist = 90; int colDist = 90;
		for (String s : "vs,s,,l,vl".split(","))
			addSubmodule(cmodule,"icon "+s, "p="+ (x+=colDist) +","+y+";i=block/cogwheel;is="+s);

		// PIN, error marker, icon2, queue indicator, icon coloring
		y+= rowDist;
		x = 0;
		addSubmodule(cmodule,"empty", "p="+ (x+=colDist) +","+y);
		addSubmodule(cmodule,"bubu", "p="+ (x+=colDist) +","+y+";i=bubu");
		addSubmodule(cmodule,"empty i2", "p="+ (x+=colDist) +","+y+";i2=status/checkmark");
		addSubmodule(cmodule,"bubu i2", "p="+ (x+=colDist) +","+y+";i=bubu;i2=status/checkmark");
		addSubmodule(cmodule,"i2", "p="+ (x+=colDist) +","+y+";i=block/cogwheel;i2=status/checkmark");

		y+= rowDist;
		x = 0;
		addSubmodule(cmodule,"pin", "p="+ (x+=colDist) +","+y+";i=block/cogwheel").setPinVisible(true);
		addSubmodule(cmodule,"i2 pin", "p="+ (x+=colDist) +","+y+";i=block/cogwheel;i2=status/checkmark").setPinVisible(true);
		addSubmodule(cmodule,"error", "p="+ (x+=colDist) +","+y+";i=block/cogwheel").setProblemDecoration(NedFigure.SEVERITY_ERROR, new ITooltipTextProvider() {
			@Override
			public String getTooltipText(int x, int y) {
				return "Error description";
			}});
		addSubmodule(cmodule,"queue", "p="+ (x+=colDist) +","+y+";i=block/cogwheel").setQueueText("q:queuename");

		// rectangle
		y+= rowDist;
		x = 0;
        for (int size : new int[] {0,1,5,20,50} ) {
        	String s = ""+size+","+(int)(size*1.2);
        	addSubmodule(cmodule,"rect "+s, "p="+ (x+=colDist) +","+y+";b="+s+",r");
        }
        // missing width or height
        String str = ",50";
        addSubmodule(cmodule,"b="+str, "p="+ (x+=colDist) +","+y+";b="+str+",r");
        str = "50,";
        addSubmodule(cmodule,"b="+str, "p="+ (x+=colDist) +","+y+";b="+str+",r");
        str = ",";
        addSubmodule(cmodule,"b="+str, "p="+ (x+=colDist) +","+y+";b="+str+",r");
        str = "error,50";
        addSubmodule(cmodule,"b="+str, "p="+ (x+=colDist) +","+y+";b="+str+",r");

		// oval
		y+= rowDist;
		x = 0;
        for (int size : new int[] {0,1,5,20,50} ) {
        	String s = ""+(int)(size*1.2)+","+size;
        	addSubmodule(cmodule,"oval "+s, "p="+ (x+=colDist) +","+y+";b="+s+",o");
        }

		// shorts for shape (o,r rec, ov, rect, oval
		y+= rowDist;
		x = 0;
		for (String s : "o,ov,oval,r,rec,rect,,bubu".split(","))
			addSubmodule(cmodule,"shape: "+s, "p="+ (x+=colDist) +","+y+";b=40,40,"+s);

		// icon and shape
		y+= rowDist;
		x = 0;
		for (String s : "50,50:20,50:50,20:20,20".split(":"))
			addSubmodule(cmodule,"i+s: "+s, "p="+(x+=colDist)+","+y+";i=block/cogwheel;b="+s);


		cmodule.getLayoutManager().layout(cmodule);
//		Debug.debug = true;
//		FigureUtils.debugPrintFigureHierarchy(figureCanvas.getRootFigure(), "  ");
		while (!exitFlag[0])
			Display.getCurrent().readAndDispatch();
	}


	@Test
	public void testSubmoduleFigureStyle() {
        Shell parent = new Shell(Display.getCurrent(), SWT.RESIZE | SWT.DIALOG_TRIM);
        parent.setText("SubmoduleFigureTest");
        parent.setActive();
        parent.setLayout(new FillLayout());
        parent.setSize(820, 420);
        final FigureCanvas figureCanvas = new FigureCanvas(parent,SWT.BORDER);
        parent.layout(true);
        parent.setVisible(true);
        parent.setFocus();
        final boolean exitFlag[] = new boolean[1];
        figureCanvas.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseDoubleClick(MouseEvent e) {
				exitFlag[0] = true;
			}
        });


        figureCanvas.setBackground(new Color(null, 255, 255, 255));
        CompoundModuleFigure cmodule = new CompoundModuleFigure();
		figureCanvas.getRootFigure().add(cmodule);
		figureCanvas.getRootFigure().setConstraint(cmodule, new Rectangle(10,10,-1,-1));
		cmodule.setDisplayString(new DisplayString("bg=800,400"));

		// border/fill color
		int x = 0;
		int y = 50; int rowDist = 90; int colDist = 90;
		for (String s : "red,blue:,green:#7711CC,:@555555,black:bubu,".split(":"))
			addSubmodule(cmodule,"color: "+s, "p="+ (x+=colDist) +","+y+";b=40,40,rect,"+s);

		// border width
		y+= rowDist;
		x = 0;
		for (String s : "-1,0,1,2,5,20,90".split(","))
			addSubmodule(cmodule,"bwidth: "+s, "p="+(x+=colDist)+","+y+";b=50,50,rect,,,"+s);

		// border width (on odd center coordinates)
		y+= rowDist+1;
		x = 1;
		for (String s : "-1,0,1,2,5,20,90".split(","))
			addSubmodule(cmodule,"bwidth: "+s, "p="+(x+=colDist)+","+y+";b=50,50,rect,,,"+s);

		// text align
		y+= 100;
		x = 0;
		for (String s : "l,r,t".split(","))
			addSubmodule(cmodule,"text: "+s, "p="+(x+=100)+","+y+";i=block/cogwheel;t=Hello\nbel,"+s);

		// text color
		for (String s : "yellow,green,,bubu".split(","))
			addSubmodule(cmodule,"color: "+s, "p="+(x+=100)+","+y+";i=block/cogwheel;t=Hello,,"+s);


		cmodule.getLayoutManager().layout(cmodule);
//		Debug.debug = true;
//		FigureUtils.debugPrintFigureHierarchy(figureCanvas.getRootFigure(), "  ");
		while (!exitFlag[0])
			Display.getCurrent().readAndDispatch();
	}


	@Test
	public void testSubmoduleFigureRange() {
        Shell parent = new Shell(Display.getCurrent(), SWT.RESIZE | SWT.DIALOG_TRIM);
        parent.setText("SubmoduleFigureRangeTest");
        parent.setActive();
        parent.setLayout(new FillLayout());
        parent.setSize(500, 600);
        final FigureCanvas figureCanvas = new FigureCanvas(parent,SWT.BORDER);
        parent.layout(true);
        parent.setVisible(true);
        parent.setFocus();
        final boolean exitFlag[] = new boolean[1];
        figureCanvas.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseDoubleClick(MouseEvent e) {
				exitFlag[0] = true;
			}
        });


        figureCanvas.setBackground(new Color(null, 255, 255, 255));
        CompoundModuleFigure cmodule = new CompoundModuleFigure() {
        	@Override
        	public void paint(Graphics graphics) {
        		graphics.setAntialias(antialiasing ? SWT.ON : SWT.OFF);
        		super.paint(graphics);
        	}
        };
		figureCanvas.getRootFigure().add(cmodule);
		figureCanvas.getRootFigure().setConstraint(cmodule, new Rectangle(10,10,-1,-1));
		cmodule.setDisplayString(new DisplayString("bg=600,600"));

		int x = 0;
		int y = 50; int rowDist = 120; int colDist = 90;
		// border width
		for (String s : "0,5,24,60,100".split(","))
			addSubmodule(cmodule,"r: "+s, "p="+(x+=colDist)+","+y+";r="+s);

		y+= rowDist;
		x = 0;
		// range background color
		for (String s : "yellow,green,,bubu".split(","))
			addSubmodule(cmodule,"bcol: "+s, "p="+(x+=colDist)+","+y+";r=70,"+s);

		y+= rowDist;
		x = 0;
		// range text color
		for (String s : "yellow,green,,bubu".split(","))
			addSubmodule(cmodule,"ocol: "+s, "p="+(x+=colDist)+","+y+";r=70,,"+s);

		y+= rowDist;
		x = 0;
		// border width
		for (String s : "0,1,5,20".split(","))
			addSubmodule(cmodule,"bwidth: "+s, "p="+(x+=colDist)+","+y+";r=70,green,blue,"+s);

		cmodule.getLayoutManager().layout(cmodule);
//		Debug.debug = true;
//		FigureUtils.debugPrintFigureHierarchy(figureCanvas.getRootFigure(), "  ");
		while (!exitFlag[0])
			Display.getCurrent().readAndDispatch();
	}

	@Test
	public void testSubmoduleFigureDisplayStringChange() {
        Shell parent = new Shell(Display.getCurrent(), SWT.RESIZE | SWT.DIALOG_TRIM);
        parent.setText("SubmoduleFigureDisplayStringChange");
        parent.setActive();
        parent.setLayout(new FillLayout());
        parent.setSize(500, 600);
        final FigureCanvas figureCanvas = new FigureCanvas(parent,SWT.BORDER);
        parent.layout(true);
        parent.setVisible(true);
        parent.setFocus();
        final boolean exitFlag[] = new boolean[1];
        figureCanvas.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseDoubleClick(MouseEvent e) {
				exitFlag[0] = true;
			}
        });


        figureCanvas.setBackground(new Color(null, 255, 255, 255));
        CompoundModuleFigure cmodule = new CompoundModuleFigure() {
        	@Override
        	public void paint(Graphics graphics) {
        		graphics.setAntialias(antialiasing ? SWT.ON : SWT.OFF);
        		super.paint(graphics);
        	}
        };
		figureCanvas.getRootFigure().add(cmodule);
		figureCanvas.getRootFigure().setConstraint(cmodule, new Rectangle(10,10,-1,-1));
		cmodule.setDisplayString(new DisplayString("bg=600,600"));

		Debug.debug = true;


		SubmoduleFigure sm1 = addSubmodule(cmodule,"name1", "i=block/cogwheel;r=60");
		SubmoduleFigure sm2 = addSubmodule(cmodule,"name2", "i=block/cogwheel");
		sleep(3);
		sm2.setDisplayString(1.0f, new DisplayString("i=block/cogwheel,red"));
		sm2.setName("name2 red");
		sleep(3);
		sm2.setName("name 2 bigger");
		sleep(3);
		sm2.setDisplayString(1.0f, new DisplayString("i=block/cogwheel,red;is=vl"));
		sleep(3);
		sm1.setDisplayString(1.0f, new DisplayString("i=block/cogwheel;r=100"));

//		Debug.debug = true;
//		FigureUtils.debugPrintFigureHierarchy(figureCanvas.getRootFigure(), "  ");
		while (!exitFlag[0])
			Display.getCurrent().readAndDispatch();
	}

	private void sleep(int sec) {
		long t;
		t = System.currentTimeMillis();
		while (System.currentTimeMillis()-t < sec*1000)
			while (Display.getCurrent().readAndDispatch())
				;
		System.out.println("waited "+sec+"s");
	}

	private SubmoduleFigure addSubmodule(CompoundModuleFigure cmodule, String name, String displayStr) {
		SubmoduleFigure sm;
			sm = new SubmoduleFigure() {
			@Override
			public void paint(Graphics graphics) {
        		graphics.setAntialias(antialiasing ? SWT.ON : SWT.OFF);
				if (drawBoundingBox) {
					graphics.pushState();
					graphics.setBackgroundColor(ColorFactory.LIGHT_BLUE);
					graphics.fillRectangle(getBounds());
					graphics.popState();
				}

				super.paint(graphics);
			}
		};

		cmodule.getSubmoduleLayer().add(sm);
		DisplayString dps = new DisplayString(displayStr);
		sm.setDisplayString(1.0f, dps);
		sm.setName(name);
		return sm;
	}
}
