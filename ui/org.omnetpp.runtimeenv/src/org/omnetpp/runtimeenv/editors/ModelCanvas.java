package org.omnetpp.runtimeenv.editors;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.runtimeenv.animation.widgets.AnimationCanvas;

//FIXME we require the GEF plugin only because SubmoduleFigure implements HandleBounds!!! Remove that! 
public class ModelCanvas extends EditorPart {
    public static final String EDITOR_ID = "org.omnetpp.runtimeenv.editors.ModelCanvas";

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        if (!(input instanceof ModuleIDEditorInput))
            throw new PartInitException("Invalid input: it must be a module in the simulation");
        setSite(site);
        setInput(input);
        setPartName(input.getName());
    }

    @Override
    public void createPartControl(Composite parent) {
//        parent.setLayout(new FormLayout());
//        parent.setBackground(new Color(null, 228, 228, 228));
//        
//        createCoolbar();
//        createSimulationToolbar();
//        createNavigationToolbar();
//        createTimeGauges();
//        createSpeedSlider();
//
//        coolBar.setWrapIndices(new int[] {2,3});
        
        AnimationCanvas canvas = new AnimationCanvas(parent, SWT.DOUBLE_BUFFERED);

        int moduleID = ((ModuleIDEditorInput)getEditorInput()).getModuleID();
        cModule module = cSimulation.getActiveSimulation().getModule(moduleID);
        final CompoundModuleFigure moduleFigure = new CompoundModuleFigure();
        canvas.getRootFigure().add(moduleFigure);
        moduleFigure.setDisplayString(module.getDisplayString());
        canvas.getRootFigure().setConstraint(moduleFigure, new Rectangle(0,0,500,500));
        
        cModule submodule = module.getSubmodule("server");
        SubmoduleFigure submoduleFigure = new SubmoduleFigure();
        submoduleFigure.setDisplayString(submodule.getDisplayString());
        submoduleFigure.setName(submodule.getFullName());
        submoduleFigure.setPinDecoration(false);
        moduleFigure.addSubmoduleFigure(submoduleFigure);

        SubmoduleConstraint constraint = new SubmoduleConstraint();
        constraint.setLocation(submoduleFigure.getPreferredLocation());
        constraint.setSize(submoduleFigure.getPreferredSize());
        Assert.isTrue(constraint.height != -1 && constraint.width != -1);
        moduleFigure.getSubmoduleContainer().setConstraint(submoduleFigure, constraint);

        for (int i=0; i<5; i++) {            
            submodule = module.getSubmodule("host", i);
            submoduleFigure = new SubmoduleFigure();
            submoduleFigure.setDisplayString(submodule.getDisplayString());
            submoduleFigure.setName(submodule.getFullName());
            submoduleFigure.setPinDecoration(false);
            moduleFigure.addSubmoduleFigure(submoduleFigure);

            constraint = new SubmoduleConstraint();
            constraint.setLocation(submoduleFigure.getPreferredLocation());
            constraint.setSize(submoduleFigure.getPreferredSize());
            Assert.isTrue(constraint.height != -1 && constraint.width != -1);
            moduleFigure.getSubmoduleContainer().setConstraint(submoduleFigure, constraint);
        }

        moduleFigure.addMouseListener(new MouseListener() {
            @Override
            public void mouseDoubleClicked(MouseEvent me) {}

            @Override
            public void mousePressed(MouseEvent me) {
                IFigure target = moduleFigure.findFigureAt(me.x, me.y);
                System.out.println(target);
            }

            @Override
            public void mouseReleased(MouseEvent me) {}
        });

    }

    @Override
    public void setFocus() {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void doSave(IProgressMonitor monitor) {
        // Nothing
    }

    @Override
    public void doSaveAs() {
        // Nothing
    }

    @Override
    public boolean isDirty() {
        return false;
    }

    @Override
    public boolean isSaveAsAllowed() {
        return false;
    }
}



////////////////
//
//public boolean containsProperty(Prop prop) {
//    //FIXME getTagArg to return null instead of "" to Java!
//    return !getTagArg(prop.getTagName(), prop.getPos()).equals("");
//}
//
//public boolean containsTag(Tag tagName) {
//    return containsTag(tagName.name());
//}
//
//public float getAsFloat(Prop prop, float defValue) {
//    try {
//        String propValue = getTagArg(prop.getTagName(), prop.getPos());
//        return StringUtils.isEmpty(propValue) ? defValue : Float.valueOf(propValue);
//    } catch (NumberFormatException e) { }
//    return defValue;
//}
//
//public int getAsInt(Prop prop, int defValue) {
//    try {
//        String propValue = getTagArg(prop.getTagName(), prop.getPos());
//        return StringUtils.isEmpty(propValue) ? defValue : Integer.valueOf(propValue);
//    } catch (NumberFormatException e) { }
//    return defValue;
//}
//
//public String getAsString(Prop prop) {
//    return getTagArg(prop.getTagName(), prop.getPos());
//}
//
//public Dimension getCompoundSize(Float scale) {
//    int width = unit2pixel(getAsFloat(Prop.MODULE_WIDTH, -1.0f), scale);
//    width = width > 0 ? width : -1;
//    int height = unit2pixel(getAsFloat(Prop.MODULE_HEIGHT, -1.0f), scale);
//    height = height > 0 ? height : -1;
//
//    return new Dimension(width, height);
//}
//
//public Point getLocation(Float scale) {
//    // return NaN to signal that the property is missing
//    Float x = getAsFloat(Prop.X, Float.NaN);
//    Float y = getAsFloat(Prop.Y, Float.NaN);
//    // if it's unspecified in any direction we should return a NULL constraint
//    if (x.equals(Float.NaN) || y.equals(Float.NaN))
//        return null;
//
//    return new Point (unit2pixel(x, scale), unit2pixel(y, scale));
//}
//
//public int getRange(Float scale) {
//    float floatvalue = getAsFloat(IDisplayString.Prop.RANGE, -1.0f);
//    return (floatvalue <= 0) ? -1 : unit2pixel(floatvalue, scale);
//}
//
//public float getScale() {
//    return getAsFloat(Prop.MODULE_SCALE, 1.0f);
//}
//
//public Dimension getSize(Float scale) {
//    int width = unit2pixel(getAsFloat(Prop.WIDTH, -1.0f), scale);
//    width = width > 0 ? width : -1;
//    int height = unit2pixel(getAsFloat(Prop.HEIGHT, -1.0f), scale);
//    height = height > 0 ? height : -1;
//    return new Dimension(width, height);
//}
//
//public final float pixel2unit(int pixel, Float overrideScale) {
//    if (overrideScale != null)
//        return pixel / overrideScale;
//    return  pixel / getScale();
//}
//
//public final int unit2pixel(float unit, Float overrideScale) {
//    if (overrideScale != null)
//        return (int)(unit * overrideScale);
//    return (int)(unit * getScale());
//}
