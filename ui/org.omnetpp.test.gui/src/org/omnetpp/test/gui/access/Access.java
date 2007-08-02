package org.omnetpp.test.gui.access;

import java.util.ArrayList;
import java.util.List;

import junit.framework.Assert;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.win32.MSG;
import org.eclipse.swt.internal.win32.OS;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.PlatformUI;

public class Access {
	protected final static boolean debug = true;

	public IWorkbench getWorkbench() {
		return PlatformUI.getWorkbench();
	}
	
	public Display getDisplay() {
		return Display.getCurrent();
	}
	
	public Shell getActiveShell() {
		return getDisplay().getActiveShell();
	}
	
	public void postEvent(Event event) {
		if (debug)
			System.out.println("Posting event: " + event);

		Display.getCurrent().post(event);
	}

	public void processEvents() {
		if (debug)
			System.out.println("Processing events started");

		while (getDisplay().readAndDispatch())
			if (debug)
				System.out.println("Processed an event");

		if (debug)
			System.out.println("Processing events finished");
	}

	public void waitUntilEventQueueBecomesEmpty() {
		MSG msg = new MSG();
		while (OS.PeekMessage (msg, 0, 0, 0, OS.PM_NOREMOVE))
			Thread.yield();
	}

	public void sleep(double seconds) {
		try {
			Thread.sleep((long)(seconds * 1000));
		}
		catch (InterruptedException e) {
			throw new RuntimeException(e);
		}
	}

	public Object findObject(Object[] objects, IPredicate predicate) {
		return theOnlyObject(collectObjects(objects, predicate));
	}

	public List<Object> collectObjects(Object[] objects, IPredicate predicate) {
		ArrayList<Object> resultObjects = new ArrayList<Object>();

		for (Object object : objects)
			if (predicate.matches(object))
				resultObjects.add(object);
		
		return resultObjects;
	}

	public Widget findWidget(Widget[] widgets, IPredicate predicate) {
		return theOnlyWidget(collectWidgets(widgets, predicate));
	}

	public List<Widget> collectWidgets(Widget[] widgets, IPredicate predicate) {
		ArrayList<Widget> resultWidgets = new ArrayList<Widget>();

		for (Widget widget : widgets)
			if (predicate.matches(widget))
				resultWidgets.add(widget);
		
		return resultWidgets;
	}

	public Control findChildControl(Composite composite, final Class<? extends Control> clazz) {
		return theOnlyControl(collectDescendantControls(composite, new IPredicate() {
			public boolean matches(Object control) {
				return clazz.isInstance(control);
			}
		}));
	}
	
	public Control findDescendantControl(Composite composite, IPredicate predicate) {
		return theOnlyControl(collectDescendantControls(composite, predicate));
	}
	
	public List<Control> collectDescendantControls(Composite composite, IPredicate predicate) {
		ArrayList<Control> controls = new ArrayList<Control>();
		collectDescendantControls(composite, predicate, controls);
		return controls;
	}

	protected void collectDescendantControls(Composite composite, IPredicate predicate, List<Control> controls) {
		for (Control control : composite.getChildren()) {
			if (control instanceof Composite)
				collectDescendantControls((Composite)control, predicate, controls);
			
			if (predicate.matches(control))
				controls.add(control);
		}
	}

	protected Object theOnlyObject(List<? extends Object> objects) {
		Assert.assertTrue("Found more than one objects when exactly one is expected", objects.size() < 2);		
		Assert.assertTrue("Found zero object when exactly one is expected", objects.size() > 0);		
		return objects.get(0);
	}

	protected Widget theOnlyWidget(List<? extends Widget> widgets) {
		return (Widget)theOnlyObject(widgets);
	}

	protected Control theOnlyControl(List<? extends Control> controls) {
		return (Control)theOnlyWidget(controls);
	}

	protected Point getCenter(Rectangle rectangle) {
		return new Point(rectangle.x + rectangle.width / 2, rectangle.y + rectangle.height / 2);
	}
}
