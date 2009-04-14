package org.omnetpp.test.unit.layout;

import junit.framework.TestCase;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.junit.Test;
import org.omnetpp.common.Debug;
import org.omnetpp.common.ui.FigureCanvas;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.layout.SpringEmbedderLayout;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.figures.misc.FigureUtils;
import org.omnetpp.ned.model.DisplayString;


public class LayoutTest extends TestCase {

	@Test
	public void testLayout() {
		Debug.debug = true;
        Shell parent = new Shell(Display.getCurrent(), SWT.RESIZE | SWT.DIALOG_TRIM);
        parent.setText("TestLayout");
        parent.setActive();
        parent.setLayout(new FillLayout());
        parent.setSize(500, 500);
        FigureCanvas figureCanvas = new FigureCanvas(parent,SWT.BORDER);
        parent.layout(true);
        parent.setVisible(true);
        parent.setFocus();
        
        figureCanvas.setBackground(new Color(null, 255, 255, 255));
        CompoundModuleFigure cmodule = new CompoundModuleFigure();
		figureCanvas.getRootFigure().add(cmodule);
		figureCanvas.getRootFigure().setConstraint(cmodule, new Rectangle(10,10,-1,-1));
		cmodule.setDisplayString(new DisplayString("bgb=300,200;bgg=50"));


		// adding 3 pinned nodes
		for (int y=50; y<=150; y += 50)
			addSubmodule(cmodule, "p=40,"+y+";b=rect,40,40");
		((SpringEmbedderLayout)cmodule.getSubmoduleLayer().getLayoutManager()).requestFullLayout();
		long t = System.currentTimeMillis();
		while (System.currentTimeMillis()-t < 3000)
			while (Display.getCurrent().readAndDispatch())
				;

		// add 3 movable nodes
		for (int i=0; i<3; ++i) 
			addSubmodule(cmodule, "");
		((SpringEmbedderLayout)cmodule.getSubmoduleLayer().getLayoutManager()).requestIncrementalLayout();
		t = System.currentTimeMillis();
		while (System.currentTimeMillis()-t < 3000)
			while (Display.getCurrent().readAndDispatch())
				;
		
		// adding additional movables (the previous 6 nodes should not change their location)
		for (int i=0; i<3; ++i) 
			addSubmodule(cmodule, "");
		((SpringEmbedderLayout)cmodule.getSubmoduleLayer().getLayoutManager()).requestIncrementalLayout();
		t = System.currentTimeMillis();
		while (System.currentTimeMillis()-t < 3000)
			while (Display.getCurrent().readAndDispatch())
		
        MessageDialog.openInformation(null, "End of test", "End of layout test");
	}
	
	
	private void addSubmodule(CompoundModuleFigure cmodule, String displayStr) {
		SubmoduleFigure sm = new SubmoduleFigure();
		cmodule.getSubmoduleLayer().add(sm);
		DisplayString dps = new DisplayString(displayStr);
		sm.setDisplayString(dps);
		SubmoduleConstraint constr = new SubmoduleConstraint();
		Point loc = dps.getLocation(1.0f);
		constr.setPinned(loc != null);
		if (loc!=null)
			constr.setLocation(loc);
		constr.setSize(sm.getPreferredSize());
		sm.setPinDecoration(constr.isPinned());
		sm.setSubmoduleConstraint(constr);
//		System.out.println("----------");
//		System.out.println("submodule added: "+sm);
//		FigureUtils.debugPrintRootFigureHierarchy(cmodule);
	}
}
