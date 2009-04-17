package org.omnetpp.test.unit.figure;

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
import org.omnetpp.common.ui.FigureCanvas;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.NedFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.TooltipFigure.ITextProvider;
import org.omnetpp.figures.misc.FigureUtils;
import org.omnetpp.ned.model.DisplayString;

public class SubmoduleFigureTest extends TestCase {

	@Test
	public void testLayout() {
		Debug.debug = true;
        Shell parent = new Shell(Display.getCurrent(), SWT.RESIZE | SWT.DIALOG_TRIM);
        parent.setText("TestLayout");
        parent.setActive();
        parent.setLayout(new FillLayout());
        parent.setSize(500, 700);
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
		cmodule.setDisplayString(new DisplayString("bg=600,400"));

		int x = 20;
		int y = 20; int rowDist = 60; int colDist = 70;
		for (String s : "vs,s,,l,vl".split(","))
			addSubmodule(cmodule,"icon "+s, "p="+ (x+=colDist) +","+y+";i=block/cogwheel;is="+s);
		
		// rectangle
		y+= rowDist;
		x = 20;
		for (String s : "1,5,20,25,50".split(","))
			addSubmodule(cmodule,"rect "+s, "p="+ (x+=colDist) +","+y+";b="+s+","+s+",r");

		// oval
		y+= rowDist;
		x = 20;
		for (String s : "1,5,20,25,50".split(","))
			addSubmodule(cmodule,"oval "+s, "p="+ (x+=colDist) +","+y+";b="+s+","+s+",o");

		// shorts for shape (o,r rec, ov, rect, oval
		y+= rowDist;
		x = 20;
		for (String s : "o,ov,oval,r,rec,rect".split(","))
			addSubmodule(cmodule,"shape: "+s, "p="+ (x+=colDist) +","+y+";b=40,40,"+s);
		
		// border/fill color
		y+= rowDist;
		x = 20;
		for (String s : "red,blue:,green:#7711CC,:@555555,black:bubu,".split(":"))
			addSubmodule(cmodule,"color: "+s, "p="+ (x+=colDist) +","+y+";b=40,40,rect,"+s);
				
		// border width
		y+= rowDist;
		x = 20;
		for (String s : "0,1,2,3,4,20".split(","))
			addSubmodule(cmodule,"bwidth: "+s, "p="+(x+=colDist)+","+y+";b=50,50,rect,,,"+s);

		// border width (on odd center coordinates)
		y+= rowDist+1;
		x = 21;
		for (String s : "0,1,2,3,4,20".split(","))
			addSubmodule(cmodule,"bwidth: "+s, "p="+(x+=colDist)+","+y+";b=50,50,rect,,,"+s);

		// icon and shape
		y+= rowDist;
		x = 20;
		for (String s : "50,50:20,50:50,20:20,20".split(":"))
			addSubmodule(cmodule,"i+s: "+s, "p="+(x+=colDist)+","+y+";i=bubu;b="+s);
		
		// text align
		y+= 100;
		x = 20;
		for (String s : "l,r,t".split(","))
			addSubmodule(cmodule,"text: "+s, "p="+(x+=100)+","+y+";i=bubu;t=Hello\nbel,"+s);
		
		// text color
		for (String s : "yellow,green,,bubu".split(","))
			addSubmodule(cmodule,"color: "+s, "p="+(x+=100)+","+y+";i=bubu;t=Hello,,"+s);

		// PIN, error marker, icon2, queue indicator, icon coloring
		y+= rowDist;
		x = 20;
		addSubmodule(cmodule,"icon2", "p="+ (x+=colDist) +","+y+";i=block/cogwheel;i2=hello");
		addSubmodule(cmodule,"pin", "p="+ (x+=colDist) +","+y+";i=block/cogwheel").setPinVisible(true);
		addSubmodule(cmodule,"error", "p="+ (x+=colDist) +","+y+";i=block/cogwheel").setProblemDecoration(NedFigure.SEVERITY_ERROR, new ITextProvider() {
			public String getTooltipText() {
				return "Error description";
			}});
		addSubmodule(cmodule,"queue", "p="+ (x+=colDist) +","+y+";i=block/cogwheel;q=queue");
		
		cmodule.getLayoutManager().layout(cmodule);
//		Debug.debug = true;
//		FigureUtils.debugPrintFigureHierarchy(figureCanvas.getRootFigure(), "  ");
		while (!exitFlag[0])
			Display.getCurrent().readAndDispatch();
	}
	
	
	private SubmoduleFigure addSubmodule(CompoundModuleFigure cmodule, String name, String displayStr) {
		SubmoduleFigure sm = new SubmoduleFigure();
		cmodule.getSubmoduleLayer().add(sm);
		DisplayString dps = new DisplayString(displayStr);
		sm.setDisplayString(dps);
		sm.setName(name);
		return sm;
	}

}
