/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.ui;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.text.AbstractInformationControl;
import org.eclipse.jface.text.AbstractReusableInformationControlCreator;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.jface.text.IInformationControlExtension2;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.ToolBar;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IUpdateableAction;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Mode;

/**
 *
 * @author andras
 */
public class ObjectTreeInformationControl extends AbstractInformationControl implements IInformationControlExtension2 {
    private ObjectFieldsViewer viewer;
    private ToolBar toolbar;
    private List<IUpdateableAction> actions = new ArrayList<IUpdateableAction>();

    public ObjectTreeInformationControl(Shell parentShell, boolean isResizable) {
        super(parentShell, isResizable);
        create();
    }

    public ObjectTreeInformationControl(Shell parentShell, String statusFieldText) {
        super(parentShell, statusFieldText);
        create();
    }

    @Override
    public Point computeSizeHint() {
        Point size = viewer.getTree().computeSize(-1, -1);
        size.x = Math.min(Math.max(size.x, 300), 800);
        size.y = Math.min(Math.max(size.y, 100), 300);
        // KLUGE: added some space for the status line ('Press F2') and the toolbar. The problem is that
        // we are not really called again when the sticky hover is created, so we better add some space
        // for the toolbar up front
        int affordanceTextHeight = JFaceResources.getDefaultFont().getFontData()[0].getHeight() * Display.getCurrent().getDPI().y / 72;
        int extraSpaceForToolbar = 10; // standard icon size is 32 pixels, but tree already adds some slack
        int extraHeight = 10;  // borders, etc.
        return new Point(size.x, size.y + affordanceTextHeight + extraSpaceForToolbar + extraHeight);
    }

    @Override
    public boolean hasContents() {
        return viewer.getInput() != null;
    }

    @Override
    protected void createContent(Composite parent) {
        viewer = new ObjectFieldsViewer(parent, SWT.NONE);

        if (isResizable()) {
            // add toolbar with actions to change the tree mode
            GridLayout layout = new GridLayout(1,true);
            layout.marginWidth = layout.marginHeight = layout.horizontalSpacing = layout.verticalSpacing = 0;
            parent.setLayout(layout);

            viewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

            toolbar = new ToolBar(parent, SWT.HORIZONTAL);
            toolbar.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
            addActionsTo(toolbar);
        }
    }

    @Override
    public void setInformation(String information) {
        throw new UnsupportedOperationException();  // call setInput instead()
    }

    @Override
    @SuppressWarnings("rawtypes")
    public void setInput(Object input) {
        viewer.setInput(input);
        viewer.setMode(ObjectFieldsViewer.getPreferredMode(input));
        boolean isSingleElementList = (input.getClass().isArray() && ((Object[])input).length == 1)
                || (input instanceof Collection && ((Collection)input).size() == 1);
        if (isSingleElementList)
            viewer.getTreeViewer().expandToLevel(2);
        updateActions(); // due to mode change
    }

    protected class SetModeAction extends SetObjectViewerModeAction {
        public SetModeAction(ObjectFieldsViewer viewer, Mode mode) {
            super(viewer, mode);
        }

        @Override
        public void run() {
            super.run();
            updateActions();
        }
    }

    protected void addActionsTo(ToolBar toolbar) {
        IToolBarManager manager = new ToolBarManager(toolbar);
        //TODO manager.add(my(new SortAction()));
        manager.add(my(new SetModeAction(viewer, Mode.PACKET)));
        manager.add(my(new SetModeAction(viewer, Mode.CHILDREN)));
        manager.add(my(new SetModeAction(viewer, Mode.GROUPED)));
        manager.add(my(new SetModeAction(viewer, Mode.INHERITANCE)));
        manager.add(my(new SetModeAction(viewer, Mode.FLAT)));
        manager.update(true);
        updateActions();
    }

    protected IUpdateableAction my(IUpdateableAction action) {
        actions.add(action);
        return action;
    }

    protected void updateActions() {
        for (IUpdateableAction action : actions)
            action.update();
    }

    /**
     * Utility method: returns a creator that creates a (transient) ObjectTreeInformationControl.
     */
    public static IInformationControlCreator getCreator() {
        return new AbstractReusableInformationControlCreator() {
            @Override
            public IInformationControl doCreateInformationControl(Shell shell) {
                return new ObjectTreeInformationControl(shell, HoverSupport.AFFORDANCE);
            }
        };
    }

    /**
     * Utility method: returns a creator that creates a (sticky) ObjectTreeInformationControl.
     */
    public static IInformationControlCreator getStickyCreator() {
        return new AbstractReusableInformationControlCreator() {
            @Override
            public IInformationControl doCreateInformationControl(Shell shell) {
                return new ObjectTreeInformationControl(shell, true);
            }
        };
    }

    /**
     * Implements IInformationControlExtension5.getInformationPresenterControlCreator().
     */
    @Override
    public IInformationControlCreator getInformationPresenterControlCreator() {
        return getStickyCreator();
    }
}