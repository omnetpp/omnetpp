package org.omnetpp.test.gui.access;

import java.util.ArrayList;
import java.util.List;

import junit.framework.Assert;

import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.EditPart;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.InstanceofPredicate;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.test.gui.core.EventTracer;
import org.omnetpp.test.gui.core.InUIThread;
import org.omnetpp.test.gui.core.NotInUIThread;

public class Access
{
	protected final static boolean debug = true;
	
	public static double delayBetweenPostEvents;
	
	static {
	    String value = System.getProperty("org.omnetpp.test.gui.DelayBetweenPostEvents");
	    delayBetweenPostEvents = value == null ? 0 : Double.parseDouble(value);
	}

	public final static int LEFT_MOUSE_BUTTON = 1;
	public final static int MIDDLE_MOUSE_BUTTON = 2;
	public final static int RIGHT_MOUSE_BUTTON = 3;
	
	protected Access createAccess(Object instance) {
		Class<?> clazz = instance.getClass();

		while (clazz != Object.class) {
			try {
				String lookupClassName = "org.omnetpp.test.gui.access." + clazz.getSimpleName() + "Access";
				if (debug)
					System.out.println("Looking up access class " + lookupClassName);
		        Class<?> accessClass = Class.forName(lookupClassName);
		        
		        return (Access)ReflectionUtils.invokeConstructor(accessClass, instance);
			} 
			catch (ClassNotFoundException e) {
				// skip this class
			}
			
			clazz = clazz.getSuperclass();
		}

		Assert.fail("No Access class found for: " + instance.getClass());
		return null;
	}

	public static Display getDisplay() {
		Display display = Display.getCurrent();
		return display != null ? display : Display.getDefault();
	}
	
	@InUIThread
	public static WorkbenchWindowAccess getWorkbenchWindowAccess() {
		IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
		Assert.assertNotNull("no active workbench window", workbenchWindow);
		return new WorkbenchWindowAccess(workbenchWindow);
	}

	@InUIThread
	public static ShellAccess getActiveShell() {
		return new ShellAccess(getDisplay().getActiveShell());
	}

	@InUIThread
	public static ShellAccess findShellByTitle(final String title) {
		return new ShellAccess((Shell)findObject(getDisplay().getShells(), new IPredicate() {
			public boolean matches(Object object) {
				Shell shell = (Shell)object;

				if (debug)
					System.out.println("Trying to collect shell: " + shell.getText());

				return shell.getText().matches(title);
			}
		}));
	}

	@InUIThread
	public static void postEvent(Event event) {
        Shell activeShell = getDisplay().getActiveShell();
		if (debug)
			System.out.println("Active shell at post event is " + activeShell);
		Assert.assertTrue(activeShell != null);
        activeShell.forceActive();

		if (debug)
			System.out.println("Posting event: " + event);

		Assert.assertTrue(getDisplay().post(event));

		if (event.type != SWT.MouseDown && event.type != SWT.MouseUp) {
            try {
                Thread.sleep((long)(delayBetweenPostEvents * 1000));
            }
            catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
		}
	}

	@InUIThread
	public static void processEvents() {
		if (debug)
			System.out.println("Processing events started");

		while (getDisplay().readAndDispatch())
			if (debug)
				System.out.println("Processed an event");

		if (debug)
			System.out.println("Processing events finished");
	}

	@NotInUIThread
	public static void sleep(double seconds) {
		try {
			Thread.sleep((long)(seconds * 1000));
		}
		catch (InterruptedException e) {
			throw new RuntimeException(e);
		}
	}

	@InUIThread
	public static void startTracingEvents() {
		EventTracer.start();
	}

	public static Event newEvent(int type) {
		Event event = new Event();
		event.type = type;
		event.display = getDisplay();

		return event;
	}

	@InUIThread
	public void pressEnter() {
		pressKey(SWT.KEYPAD_CR);
	}
	
	@InUIThread
	public void pressKey(int keyCode) {
		pressKey(keyCode, SWT.None);
	}

	@InUIThread
	public void pressKey(int keyCode, int modifierKeys) {
		pressKey((char)0, keyCode, modifierKeys);
	}

	@InUIThread
	public void pressKey(char character) {
		pressKey(character, SWT.None);
	}

	@InUIThread
	public void pressKey(char character, int modifierKeys) {
		pressKey(character, 0, modifierKeys);
	}

	@InUIThread
	public void pressKey(char character, int keyCode, int modifierKeys) {
		Event event;

		if (modifierKeys != 0)
			holdDownModifiers(modifierKeys);

		event = newEvent(SWT.KeyDown);
		event.character = character;
		event.keyCode = keyCode;
		postEvent(event);

		event = newEvent(SWT.KeyUp);
		event.keyCode = keyCode;
		event.character = (char)keyCode;
		postEvent(event);

		if (modifierKeys != 0)
			releaseUpModifiers(modifierKeys);
	}

    @InUIThread
    public void holdDownModifiers(int modifierKeys) {
        Event event;
        
        if ((modifierKeys & SWT.SHIFT) != 0) {
        	event = newEvent(SWT.KeyDown);
        	event.keyCode = SWT.SHIFT;
        	postEvent(event);
        }

        if ((modifierKeys & SWT.CONTROL) != 0) {
        	event = newEvent(SWT.KeyDown);
        	event.keyCode = SWT.CONTROL;
        	postEvent(event);
        }

        if ((modifierKeys & SWT.ALT) != 0) {
        	event = newEvent(SWT.KeyDown);
        	event.keyCode = SWT.ALT;
        	postEvent(event);
        }
    }

    @InUIThread
    public void releaseUpModifiers(int modifierKeys) {
        Event event;

        if ((modifierKeys & SWT.SHIFT) != 0) {
            event = newEvent(SWT.KeyUp);
            event.keyCode = SWT.SHIFT;
            postEvent(event);
        }

        if ((modifierKeys & SWT.CONTROL) != 0) {
            event = newEvent(SWT.KeyUp);
            event.keyCode = SWT.CONTROL;
            postEvent(event);
        }

        if ((modifierKeys & SWT.ALT) != 0) {
            event = newEvent(SWT.KeyUp);
            event.keyCode = SWT.ALT;
            postEvent(event);
        }
    }

	protected void postMouseEvent(int type, int button, int x, int y) {
		Event event = newEvent(type); // e.g. SWT.MouseMove
		event.button = button;
		event.x = x;
		event.y = y;
		event.count = 1;
		postEvent(event);
	}

	@InUIThread
	public static Object findObject(List<Object> objects, IPredicate predicate) {
		return theOnlyObject(collectObjects(objects, predicate));
	}

	@InUIThread
	public static Object findObject(Object[] objects, IPredicate predicate) {
		return theOnlyObject(collectObjects(objects, predicate));
	}

    @InUIThread
    public static boolean hasObject(List<Object> objects, IPredicate predicate) {
        return collectObjects(objects, predicate).size() != 0;
    }

    @InUIThread
    public static Object hasObject(Object[] objects, IPredicate predicate) {
        return collectObjects(objects, predicate).size() != 0;
    }

	@InUIThread
	public static List<Object> collectObjects(List<Object> objects, IPredicate predicate) {
		return collectObjects(objects.toArray(new Object[0]), predicate);
	}
	
	@InUIThread
	public static List<Object> collectObjects(Object[] objects, IPredicate predicate) {
		ArrayList<Object> resultObjects = new ArrayList<Object>();

		for (Object object : objects)
			if (predicate.matches(object))
				resultObjects.add(object);

		return resultObjects;
	}

	@InUIThread
	public static Control findDescendantControl(Composite composite, final Class<? extends Control> clazz) {
		return findDescendantControl(composite, new InstanceofPredicate(clazz));
	}

	@InUIThread
	public static Control findDescendantControl(Composite composite, IPredicate predicate) {
		return theOnlyControl(collectDescendantControls(composite, predicate));
	}

    @InUIThread
    public static boolean hasDescendantControl(Composite composite, IPredicate predicate) {
        return collectDescendantControls(composite, predicate).size() != 0;
    }

	@InUIThread
	public static List<Control> collectDescendantControls(Composite composite, IPredicate predicate) {
		ArrayList<Control> controls = new ArrayList<Control>();
		collectDescendantControls(composite, predicate, controls);
		return controls;
	}

	protected static void collectDescendantControls(Composite composite, IPredicate predicate, List<Control> controls) {
		for (Control control : composite.getChildren()) {
			if (control instanceof Composite)
				collectDescendantControls((Composite)control, predicate, controls);

			if (predicate.matches(control))
				controls.add(control);
		}
	}

	protected static CTabItem findDescendantCTabItemByLabel(Composite composite, final String label) {
		CTabFolder cTabFolder = (CTabFolder)findDescendantControl(composite, new IPredicate() {
			public boolean matches(Object object) {
				if (object instanceof CTabFolder) {
					CTabFolder cTabFolder = (CTabFolder)object;
					List<Object> cTabItems = collectObjects(cTabFolder.getItems(), new IPredicate() {
						public boolean matches(Object object) {
							return ((CTabItem)object).getText().matches(label);
						}
					});

					Assert.assertTrue("more than one matching CTabItem found", cTabItems.size() < 2);

					if (cTabItems.size() == 0)
						return false;
					else
						return true;
				}
				return false;
			}
		});

		return (CTabItem)findObject(cTabFolder.getItems(), new IPredicate() {
			public boolean matches(Object object) {
				return ((CTabItem)object).getText().matches(label);
			}
		});
	}
	
	public interface IValue {
	    public double valueOf(Object object);
	}

    @InUIThread
    public static Control findControlMaximizing(List<Control> controls, IValue valueFunction) {
        Control result = null;
        double max = Double.NEGATIVE_INFINITY;
        
        for (Control control : controls) {
            double value = valueFunction.valueOf(control);

            if (value > max) {
                max = value;
                result = control;
            }
            else
                Assert.assertTrue(value != max);
        }

        return result;
    }

	@InUIThread
	public static IFigure findDescendantFigure(IFigure figure, final Class<? extends IFigure> clazz) {
		return findDescendantFigure(figure, new InstanceofPredicate(clazz));
	}

	@InUIThread
	public static IFigure findDescendantFigure(IFigure figure, IPredicate predicate) {
		return theOnlyFigure(collectDescendantFigures(figure, predicate));
	}

    @InUIThread
    public static boolean hasDescendantFigure(IFigure figure, IPredicate predicate) {
        return collectDescendantFigures(figure, predicate).size() != 0;
    }

	@InUIThread
	public static List<IFigure> collectDescendantFigures(IFigure figure, IPredicate predicate) {
		ArrayList<IFigure> figures = new ArrayList<IFigure>();
		collectDescendantFigures(figure, predicate, figures);
		return figures;
	}

	@SuppressWarnings("unchecked")
	protected static void collectDescendantFigures(IFigure parentFigure, IPredicate predicate, List<IFigure> figures) {
		for (IFigure figure: (List<IFigure>)parentFigure.getChildren()) {
			collectDescendantFigures(figure, predicate, figures);

			if (predicate.matches(figure))
				figures.add(figure);
		}
	}
	
	@InUIThread
	public static EditPart findDescendantEditPart(EditPart editPart, final Class<? extends EditPart> clazz) {
	    return findDescendantEditPart(editPart, new InstanceofPredicate(clazz));
	}

	@InUIThread
	public static EditPart findDescendantEditPart(EditPart editPart, IPredicate predicate) {
		return theOnlyEditPart(collectDescendantEditParts(editPart, predicate));
	}

    @InUIThread
    public static boolean hasDescendantEditPart(EditPart editPart, IPredicate predicate) {
        return collectDescendantEditParts(editPart, predicate).size() != 0;
    }

	@InUIThread
	public static List<EditPart> collectDescendantEditParts(EditPart editPart, IPredicate predicate) {
		ArrayList<EditPart> editParts = new ArrayList<EditPart>();
		collectDescendantEditParts(editPart, predicate, editParts);
		return editParts;
	}

	@SuppressWarnings("unchecked")
	protected static void collectDescendantEditParts(EditPart parentEditPart, IPredicate predicate, List<EditPart> editParts) {
		for (EditPart editPart: (List<EditPart>)parentEditPart.getChildren()) {
			collectDescendantEditParts(editPart, predicate, editParts);

			if (predicate.matches(editPart))
				editParts.add(editPart);
		}
	}

	protected static Object theOnlyObject(List<? extends Object> objects) {
		Assert.assertTrue("Found more than one objects when exactly one is expected", objects.size() < 2);
		Assert.assertTrue("Found zero object when exactly one is expected", objects.size() > 0);
		return objects.get(0);
	}

	protected static Widget theOnlyWidget(List<? extends Widget> widgets) {
		return (Widget)theOnlyObject(widgets);
	}

	protected static Control theOnlyControl(List<? extends Control> controls) {
		return (Control)theOnlyObject(controls);
	}

	protected static IFigure theOnlyFigure(List<? extends IFigure> controls) {
		return (IFigure)theOnlyObject(controls);
	}

	protected static EditPart theOnlyEditPart(List<? extends EditPart> controls) {
		return (EditPart)theOnlyObject(controls);
	}

	protected static Point getCenter(Rectangle rectangle) {
		return new Point(rectangle.x + rectangle.width / 2, rectangle.y + rectangle.height / 2);
	}

	protected static Point getCenter(org.eclipse.draw2d.geometry.Rectangle rectangle) {
		return new Point(rectangle.x + rectangle.width / 2, rectangle.y + rectangle.height / 2);
	}

	/**
	 * A failed attempt to dump all active popup menus, using the private fields of Display.
	 * Unfortunately, it's all nulls in the arrays. (This is due to runPopups() running
	 * before all syncExec()/asyncExec() code...)
	 */
	@InUIThread
	public static void dumpCurrentMenus() {
		Display display = Display.getDefault();
		Menu[] bars = (Menu[]) ReflectionUtils.getFieldValue(display, "bars");
		Menu[] popups = (Menu[]) ReflectionUtils.getFieldValue(display, "popups");
		MenuItem[] items = (MenuItem[]) ReflectionUtils.getFieldValue(display, "items");
		if (bars == null) bars = new Menu[0];
		if (popups == null) popups = new Menu[0];
		if (items == null) items = new MenuItem[0];

		System.out.println(bars.length + " menubars, "+popups.length + " popup menus, " + items.length + " menu items");
		for (Menu bar : bars) {
			System.out.println("  menubar: " + bar);
			if (bar != null)
				for (MenuItem item : bar.getItems())
					System.out.println("    " + item.toString());
		}
		for (Menu popup : popups) {
			System.out.println("  popup: " + popup);
			if (popup != null)
				for (MenuItem item : popup.getItems())
					System.out.println("    " + item.toString());
		}
	}

	/**
	 * Dumps all widgets of all known shells. Note: active popup menus are
	 * sadly not listed (not returned by Display.getShells()).
	 */
	@InUIThread
	public static void dumpWidgetHierarchy() {
		System.out.println("display-root");
		for (Shell shell : Display.getDefault().getShells())
			dumpWidgetHierarchy(shell, 1);
	}

	@InUIThread
	public static void dumpWidgetHierarchy(Control control) {
		dumpWidgetHierarchy(control, 0);
	}

	protected static void dumpWidgetHierarchy(Control control, int level) {
		System.out.println(StringUtils.repeat("  ", level) + control.toString());
		
		if (control.getMenu() != null)
			dumpMenu(control.getMenu(), level);

		if (control instanceof Composite)
			for (Control child : ((Composite)control).getChildren())
				dumpWidgetHierarchy(child, level+1);
	}

	protected static void dumpMenu(Menu menu, int level) {
		for (MenuItem menuItem : menu.getItems()) {
			System.out.println(StringUtils.repeat("  ", level) + menuItem.getText());

			if (menuItem.getMenu() != null)
				dumpMenu(menuItem.getMenu(), level + 1);
		}
	}
}
