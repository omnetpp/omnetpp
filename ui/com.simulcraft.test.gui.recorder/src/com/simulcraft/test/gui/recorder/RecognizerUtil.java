package com.simulcraft.test.gui.recorder;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Widget;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.InstanceofPredicate;
import org.omnetpp.common.util.StringUtils;

/**
 * Utility class. All methods are static.
 * @author Andras
 */
public class RecognizerUtil  {
    public static String quote(String text) {
        return "\"" + text.replace("\"", "\\\"")+ "\""; 
    }

    public static JavaSequence wrap(String expr, double quality) {
        return wrap(new JavaExpr(expr, quality));
    }

    public static JavaSequence wrap(JavaExpr expr) {
        JavaSequence result = new JavaSequence();
        result.add(expr);
        return result;
    }

    public static JavaSequence chain(JavaSequence base, String expr, double quality) {
        return chain(base, new JavaExpr(expr, quality));
    }

    public static JavaSequence chain(JavaSequence base, JavaExpr expr) {
        // if creation of either "base" or "expr" failed, return null
        if (base == null || expr == null)
            return null;
        Assert.isTrue(!base.isEmpty());
        JavaSequence result = new JavaSequence();
        result.merge(base);
        result.add(expr);
        expr.setMethodOf(base.getLast());
        return result;
    }

    public static Object findObject(List<Object> objects, IPredicate predicate) {
        return theOnlyObject(collectObjects(objects, predicate));
    }

    public static boolean hasObject(List<Object> objects, IPredicate predicate) {
        return collectObjects(objects, predicate).size() == 1;
    }

    public static List<Object> collectObjects(List<Object> objects, IPredicate predicate) {
        ArrayList<Object> resultObjects = new ArrayList<Object>();

        for (Object object : objects)
            if (predicate.matches(object))
                resultObjects.add(object);

        return resultObjects;
    }

    public static Control findDescendantControl(Composite composite, final Class<? extends Control> clazz) {
        return findDescendantControl(composite, new InstanceofPredicate(clazz));
    }

    public static Control findDescendantControl(Composite composite, IPredicate predicate) {
        return theOnlyControl(collectDescendantControls(composite, predicate));
    }

    public static boolean hasDescendantControl(Composite composite, IPredicate predicate) {
        return collectDescendantControls(composite, predicate).size() == 1;
    }

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

    protected static Object theOnlyObject(List<? extends Object> objects) {
        return objects.size() == 1 ? objects.get(0) : null;
    }

    protected static Widget theOnlyWidget(List<? extends Widget> widgets) {
        return (Widget)theOnlyObject(widgets);
    }

    protected static Control theOnlyControl(List<? extends Control> controls) {
        return (Control)theOnlyObject(controls);
    }
    
    protected static Label getPrecedingUniqueLabel(Composite composite, Control control) {
        Control[] siblings = control.getParent().getChildren();
        Label label = null;
        for (int i = 1; i < siblings.length && label == null; i++)
            if (siblings[i] == control && siblings[i-1] instanceof Label)
                label = (Label) siblings[i-1];
        if (label != null) {
            final String labelText = label.getText();
            Control theOnlySuchLabel = findDescendantControl(composite, new IPredicate() {
                public boolean matches(Object object) {
                    return object instanceof Label && ((Label)object).getText().equals(labelText);
                }});
            if (theOnlySuchLabel == label)
                return label;
        }
        return null;
    }

    /**
     * Dumps all widgets of all known shells. Note: active popup menus are
     * sadly not listed (not returned by Display.getShells()).
     */
    public static void dumpWidgetHierarchy() {
        System.out.println("display-root");
        for (Shell shell : Display.getDefault().getShells())
            dumpWidgetHierarchy(shell, 1);
    }

    public static void dumpWidgetHierarchy(Control control) {
        dumpWidgetHierarchy(control, 0);
    }

    protected static void dumpWidgetHierarchy(Control control, int level) {
        System.out.println(StringUtils.repeat("  ", level) + control.toString() + (!control.isVisible() ? " (not visible)" : ""));
        
        if (control.getMenu() != null)
            dumpMenu(control.getMenu(), level);

        if (control instanceof Composite)
            for (Control child : ((Composite)control).getChildren())
                dumpWidgetHierarchy(child, level+1);
    }

    public static void dumpMenu(Menu menu) {
        dumpMenu(menu, 0);
    }
    
    protected static void dumpMenu(Menu menu, int level) {
        for (MenuItem menuItem : menu.getItems()) {
            System.out.println(StringUtils.repeat("  ", level) + menuItem.getText());

            if (menuItem.getMenu() != null)
                dumpMenu(menuItem.getMenu(), level + 1);
        }
    }

}