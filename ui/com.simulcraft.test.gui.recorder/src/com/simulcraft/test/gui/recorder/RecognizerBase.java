package com.simulcraft.test.gui.recorder;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.StringEscapeUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IViewReference;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;
import static org.omnetpp.common.util.Predicate.instanceOf;

/**
 * @author Andras
 */
public class RecognizerBase  {
    protected GUIRecorder recorder;
    
    public RecognizerBase(GUIRecorder recorder) {
        this.recorder = recorder;
    }

    public Object uiObject(Event e) {
        return recorder.resolveUIObject(e);
    }

    public JavaExpr expr(String javaCode, double quality, Object resultUIObject) {
        return new JavaExpr(javaCode, quality, resultUIObject, recorder.getVariableTypeFor(resultUIObject));
    }

    public JavaExpr expr(String javaCode, double quality, Object resultUIObject, String suggestedVariableName) {
        return new JavaExpr(javaCode, quality, resultUIObject, recorder.getVariableTypeFor(resultUIObject), suggestedVariableName);
    }

    // expr is standalone expression 
    public JavaSequence makeStatement(JavaExpr expr) {
        return appendStatement(new JavaSequence(), expr);
    }

    // expr is a method, called on uiObject 
    public JavaSequence makeMethodCall(Object uiObject, JavaExpr methodExpr) {
        return appendMethodCall(new JavaSequence(), uiObject, methodExpr);
    }

    // expr is standalone expression 
    public JavaSequence appendStatement(JavaSequence base, JavaExpr expr) {
        base.add(expr);
        return base;
    }

    // expr is a method, called on uiObject 
    public JavaSequence appendMethodCall(JavaSequence base, Object uiObject, JavaExpr methodExpr) {
        Assert.isTrue(!(uiObject instanceof Event) && !(uiObject instanceof JavaExpr) && !(uiObject instanceof JavaSequence));

        // first, check if uiObject was already identified somewhere
        JavaExpr uiObjectExpr = recorder.lookup(uiObject);
        if (uiObjectExpr == null)
            uiObjectExpr = base.lookupUIObject(uiObject);
        if (uiObjectExpr == null) {
            // no, generate code to identify it here
            JavaSequence seq = recorder.identifyObject(uiObject);
            if (seq != null) {
                base.merge(seq);
                uiObjectExpr = base.lookupUIObject(uiObject);
            }
        }

        // add the new expression to the code sequence
        if (uiObjectExpr == null) {
            if (Activator.DEBUG)
            	System.out.println("could not find a way to identify UI object: " + uiObject);
            return null; // could not find a way to identify uiObject
        }
        else {
            methodExpr.setCalledOn(uiObjectExpr);
            base.add(methodExpr);
            return base;
        }
    }

    public static String quoteRegexText(String text) {
        return toJavaLiteral(toRegex(text));
    }

    public static String quoteMenuPath(String text) {
        return toJavaLiteral(toRegex(text.replace("&", "")).replace("\\|", "|").replace("\\.\\.\\." ,".*"));
    }

    public static String quoteLabel(String text) {
        return toJavaLiteral(toRegex(text.replace("&", "")).replace("\\.\\.\\." ,".*"));
    }

    public static String toRegex(String text) {
        for (char ch : "\\*?+.^$()[]{}|".toCharArray())
            text = text.replace(""+ch, "\\"+ch);
        return text;
    }

    public static String toJavaLiteral(String text) {
        return "\"" + StringEscapeUtils.escapeJava(text) + "\""; 
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
        return findDescendantControl(composite, instanceOf(clazz));
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

    public static IFigure findDescendantFigure(IFigure figure, final Class<? extends IFigure> clazz) {
        return findDescendantFigure(figure, instanceOf(clazz));
    }
    
    public static IFigure findDescendantFigure(IFigure figure, IPredicate predicate) {
        return (IFigure) theOnlyObject(collectDescendantFigures(figure, predicate));
    }

    public static List<IFigure> collectDescendantFigures(IFigure figure, IPredicate predicate) {
        ArrayList<IFigure> figures = new ArrayList<IFigure>();
        collectDescendantFigures(figure, predicate, figures);
        return figures;
    }

    @SuppressWarnings("unchecked")
    protected static void collectDescendantFigures(IFigure figure, IPredicate predicate, List<IFigure> figures) {
        for (IFigure child : (List<IFigure>)figure.getChildren()) {
            if (predicate.matches(child))
                figures.add(child);
            collectDescendantFigures(child, predicate, figures);
        }
    }

    public static IWorkbenchPart findWorkbenchPart(boolean includeMultipageEditorPages, IPredicate predicate) {
        return (IWorkbenchPart) theOnlyObject(collectWorkbenchParts(includeMultipageEditorPages, predicate));
    }

    protected static List<IWorkbenchPart> collectWorkbenchParts(boolean includeMultipageEditorPages, IPredicate predicate) {
        ArrayList<IWorkbenchPart> result = new ArrayList<IWorkbenchPart>();
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        for (IWorkbenchPage page : workbenchWindow.getPages()) {
            for (IViewReference viewReference : page.getViewReferences()) {
                IWorkbenchPart viewPart = viewReference.getPart(false);
                if (viewPart != null && predicate.matches(viewPart))
                    result.add(viewPart);
            }
            for (IEditorReference editorReference : page.getEditorReferences()) {
                IWorkbenchPart editorPart = editorReference.getPart(false);
                if (editorPart != null && predicate.matches(editorPart))
                    result.add(editorPart);
                if (editorPart instanceof MultiPageEditorPart && includeMultipageEditorPages)
                    result.addAll(collectPageEditors((MultiPageEditorPart)editorPart, predicate));
            }
        }
        return result;
    }

    public static IEditorPart findPageEditor(MultiPageEditorPart multiPageEditor, IPredicate predicate) {
        return (IEditorPart) theOnlyObject(collectPageEditors(multiPageEditor, predicate));
    }

    protected static List<IEditorPart> collectPageEditors(MultiPageEditorPart multiPageEditor, IPredicate predicate) {
        ArrayList<IEditorPart> result = new ArrayList<IEditorPart>();
        int pages = (Integer) ReflectionUtils.invokeMethod(multiPageEditor, "getPageCount");
        for (int i=0; i<pages; i++) {
            IEditorPart pageEditor = (IEditorPart) ReflectionUtils.invokeMethod(multiPageEditor, "getEditor", i);
            if (pageEditor != null && predicate.matches(pageEditor))
                result.add(pageEditor);
        }
        return result;
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

    public static void dumpFigureHierarchy(IFigure figure) {
        dumpFigureHierarchy(figure, 0);
    }

    @SuppressWarnings("unchecked")
    protected static void dumpFigureHierarchy(IFigure figure, int level) {
        System.out.println(StringUtils.repeat("  ", level) + toString(figure));
        
        for (IFigure child : (List<IFigure>)figure.getChildren())
            dumpFigureHierarchy(child, level+1);
    }

    public static String toString(IFigure figure) {
        // most figures don't define a toString, and the default toString() is useless.
        // we can recognize the default because it starts with the class name
        if (figure.toString().startsWith(figure.getClass().getCanonicalName())) {
            return figure.getClass().getSimpleName();
        }
        else {
            return figure.toString();
        }
        
    }

    

}