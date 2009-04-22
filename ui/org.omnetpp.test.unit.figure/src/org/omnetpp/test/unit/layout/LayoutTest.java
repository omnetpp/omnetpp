package org.omnetpp.test.unit.layout;

import junit.framework.TestCase;

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
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.FigureCanvas;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.layout.CompoundModuleLayout;
import org.omnetpp.ned.model.DisplayString;



public class LayoutTest extends TestCase {

	static {
		ImageFactory.initialize(new String[] {System.getenv("HOME")+"/omnetpp/images"});
	}

	@Test
	public void testLayout() {
        Shell parent = new Shell(Display.getCurrent(), SWT.RESIZE | SWT.DIALOG_TRIM);
        parent.setText("TestLayout");
        parent.setActive();
        parent.setLayout(new FillLayout());
        parent.setSize(500, 500);
        final FigureCanvas figureCanvas = new FigureCanvas(parent,SWT.BORDER);
        final boolean exitFlag[] = new boolean[1];
        figureCanvas.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseDoubleClick(MouseEvent e) {
				exitFlag[0] = true;
			}
        });
        parent.layout(true);
        parent.setVisible(true);
        parent.setFocus();

        
        figureCanvas.setBackground(new Color(null, 255, 255, 255));
        CompoundModuleFigure cmodule = new CompoundModuleFigure();
		figureCanvas.getRootFigure().add(cmodule);
		figureCanvas.getRootFigure().setConstraint(cmodule, new Rectangle(10,10,-1,-1));
		cmodule.setDisplayString(new DisplayString("bgg=50"));


		// adding 3 pinned nodes
		for (int y=50; y<=150; y += 50)
			addSubmodule(cmodule, "pinned", "p=40,"+y);
		((CompoundModuleLayout)cmodule.getSubmoduleLayer().getLayoutManager()).requestFullLayout();
		sleep();

		// add 3 movable nodes
		for (int i=0; i<3; ++i) 
			addSubmodule(cmodule, "movable A", "");
		sleep();
		
		// adding additional movables (the previous 6 nodes should not change their location)
		for (int i=0; i<3; ++i) 
			addSubmodule(cmodule, "movable B", "");
		sleep();

		while (!exitFlag[0])
			Display.getCurrent().readAndDispatch();
	}

	@Test
	public void testVectorLayout() {
		Debug.debug = true;
        Shell parent = new Shell(Display.getCurrent(), SWT.RESIZE | SWT.DIALOG_TRIM);
        parent.setText("TestLayout");
        parent.setActive();
        parent.setLayout(new FillLayout());
        parent.setSize(800, 600);
        final FigureCanvas figureCanvas = new FigureCanvas(parent,SWT.BORDER);
        final boolean exitFlag[] = new boolean[1];
        figureCanvas.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseDoubleClick(MouseEvent e) {
				exitFlag[0] = true;
			}
        });
        parent.layout(true);
        parent.setVisible(true);
        parent.setFocus();

        
        figureCanvas.setBackground(new Color(null, 255, 255, 255));
        CompoundModuleFigure cmodule = new CompoundModuleFigure();
		figureCanvas.getRootFigure().add(cmodule);
		figureCanvas.getRootFigure().setConstraint(cmodule, new Rectangle(10,10,-1,-1));
		cmodule.setDisplayString(new DisplayString("bgg=50"));

		// adding 3 pinned nodes
	    addSubmoduleVector(cmodule, "m,2", "p=30,30,m,2", 7);
	    addSubmoduleVector(cmodule, "m,2,100,60", "p=260,30,m,2,100,60", 7);
	    addSubmoduleVector(cmodule, "r", "p=500,30,r", 3);
	    addSubmoduleVector(cmodule, "r,100", "p=500,100,r,100", 3);
	    addSubmoduleVector(cmodule, "c", "p=600,220,c", 3);
	    addSubmoduleVector(cmodule, "c,100", "p=680,220,c,100", 3);
	    addSubmoduleVector(cmodule, "ri", "p=350,350,ri", 5);
	    addSubmoduleVector(cmodule, "ri,100,50", "p=30,350,ri,100,50", 6);
	    addSubmoduleVector(cmodule, "e,50,-100", "p=350,350,e,50,-100", 2);
	    addSubmoduleVector(cmodule, "bubu", "p=500,250,bubu", 2);

	    ((CompoundModuleLayout)cmodule.getSubmoduleLayer().getLayoutManager()).requestFullLayout();

		while (!exitFlag[0])
			Display.getCurrent().readAndDispatch();
	}

	@Test
	public void testMovableVectorLayout() {
		Debug.debug = true;
        Shell parent = new Shell(Display.getCurrent(), SWT.RESIZE | SWT.DIALOG_TRIM);
        parent.setText("TestLayout");
        parent.setActive();
        parent.setLayout(new FillLayout());
        parent.setSize(800, 600);
        final FigureCanvas figureCanvas = new FigureCanvas(parent,SWT.BORDER);
        final boolean exitFlag[] = new boolean[1];
        figureCanvas.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseDoubleClick(MouseEvent e) {
				exitFlag[0] = true;
			}
        });
        parent.layout(true);
        parent.setVisible(true);
        parent.setFocus();

        figureCanvas.setBackground(new Color(null, 255, 255, 255));
        CompoundModuleFigure cmodule = new CompoundModuleFigure();
		figureCanvas.getRootFigure().add(cmodule);
		figureCanvas.getRootFigure().setConstraint(cmodule, new Rectangle(10,10,-1,-1));
		cmodule.setDisplayString(new DisplayString("bgg=50"));

		// adding 3 pinned nodes
	    addSubmoduleVector(cmodule, "m,2", "p=,,m,2", 4);
	    ((CompoundModuleLayout)cmodule.getSubmoduleLayer().getLayoutManager()).requestFullLayout();
	    sleep();
	    addSubmoduleVector(cmodule, "r", "p=,,r", 3);
	    ((CompoundModuleLayout)cmodule.getSubmoduleLayer().getLayoutManager()).requestFullLayout();
	    sleep();
	    addSubmoduleVector(cmodule, "c", "p=,,c", 3);
	    ((CompoundModuleLayout)cmodule.getSubmoduleLayer().getLayoutManager()).requestFullLayout();
	    sleep();
	    addSubmoduleVector(cmodule, "ri", "p=,,ri", 5);
	    ((CompoundModuleLayout)cmodule.getSubmoduleLayer().getLayoutManager()).requestFullLayout();

		while (!exitFlag[0])
			Display.getCurrent().readAndDispatch();
	}

	private void sleep() {
		long t;
		t = System.currentTimeMillis();
		while (System.currentTimeMillis()-t < 3000)
			while (Display.getCurrent().readAndDispatch())
				;
	}
	
	private SubmoduleFigure addSubmodule(CompoundModuleFigure cmodule, String name, String displayStr) {
		SubmoduleFigure sm = new SubmoduleFigure();
		cmodule.getSubmoduleLayer().add(sm);
		sm.setName(name);
		DisplayString dps = new DisplayString(displayStr);
		sm.setDisplayString(1.0f, dps);
		sm.setPinVisible(dps.getLocation(1.0f) != null);
		return sm;
	}
	
	private void addSubmoduleVector(CompoundModuleFigure cmodule, String name, String displayStr, int size) {
		
		for (int i=0; i<size; i++) {
			SubmoduleFigure sm = addSubmodule(cmodule, name+"["+i+"]", displayStr);
			sm.setSubmoduleVectorIndex(name, size, i);
		}
	}
}
