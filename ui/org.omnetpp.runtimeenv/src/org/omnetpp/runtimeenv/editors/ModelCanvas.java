package org.omnetpp.runtimeenv.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.runtimeenv.animation.widgets.AnimationCanvas;

public class ModelCanvas extends EditorPart {
    public static final String EDITOR_ID = "org.omnetpp.runtimeenv.editors.ModelCanvas";
    
    private static final String ICONS_FINISH_GIF = "icons/finish.gif";
    private static final String ICONS_STOPSIM_GIF = "icons/stopsim.gif";
    private static final String ICONS_EXPRESS_GIF = "icons/express.gif";
    private static final String ICONS_FAST_GIF = "icons/fast.gif";
    private static final String ICONS_RUN_GIF = "icons/run.gif";
    private static final String ICONS_DOONEEVENT_GIF = "icons/dooneevent.gif";
    private static final String ICONS_RESTART_GIF = "icons/restart.gif";

    protected ToolBar liveToolBar;
    protected ToolItem liveStopToolItem; 
    protected ToolItem liveRestartToolItem;
    protected ToolItem liveStepToolItem;
    protected ToolItem liveRunToolItem;
    protected ToolItem liveFastToolItem;
    protected ToolItem liveExpressToolItem;
    protected ToolItem liveFinishToolItem; 

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
        Label testFigure = new Label("bubu");
        canvas.getRootFigure().add(testFigure);
        canvas.getRootFigure().setConstraint(testFigure, new Rectangle(0,0,100,100));

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
