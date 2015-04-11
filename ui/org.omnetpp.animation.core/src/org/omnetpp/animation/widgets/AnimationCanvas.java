/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.widgets;

import java.util.ArrayList;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.IFigure;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.controller.IAnimationListener;
import org.omnetpp.animation.editors.AnimationContributor;
import org.omnetpp.animation.figures.AnimationCompoundModuleFigure;
import org.omnetpp.common.canvas.EditableFigureCanvas;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.EventLogSelection;
import org.omnetpp.common.eventlog.EventNumberRangeSet;
import org.omnetpp.common.eventlog.IEventLogChangeListener;
import org.omnetpp.common.eventlog.IEventLogProvider;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.figures.misc.ISelectableFigure;

/**
 * Animation widget.
 *
 * @author levy
 */
public class AnimationCanvas extends EditableFigureCanvas implements ISelectionProvider, IEventLogChangeListener, IEventLogProvider, IAnimationListener
{
    private IEventLog eventLog; // the C++ wrapper for the data to be displayed

    private EventLogInput eventLogInput; // the Java input object

    private AnimationContributor animationContributor;

    private AnimationController animationController;

    private IWorkbenchPart workbenchPart;

    private ArrayList<Integer> shownCompoundModuleIds;

    private ListenerList selectionChangedListeners = new ListenerList(); // list of selection change listeners (type ISelectionChangedListener).

    private ISelectableFigure selectedFigure;

    private boolean isSelectionChangeInProgress;

	public AnimationCanvas(Composite parent, int style) {
		super(parent, style);
		this.shownCompoundModuleIds = new ArrayList<Integer>();
		addMouseListener(new MouseAdapter() {
		    @Override
		    public void mouseUp(MouseEvent e) {
		        setSelectedElement(null, null);
		    }
		});
	}

    public IWorkbenchPart getWorkbenchPart() {
        return workbenchPart;
    }

    public void setWorkbenchPart(IWorkbenchPart workbenchPart) {
        this.workbenchPart = workbenchPart;
    }

    public AnimationController getAnimationController() {
        return animationController;
    }

    public AnimationContributor getAnimationContributor() {
        return animationContributor;
    }

    public void setAnimationContributor(AnimationContributor animationContributor) {
        this.animationContributor = animationContributor;
        MenuManager menuManager = new MenuManager();
        animationContributor.contributeToPopupMenu(menuManager);
        Menu menu = menuManager.createContextMenu(this);
        figureCanvas.setMenu(menu);
        setMenu(menu);
    }

    public Integer[] getShownCompoundModules() {
        return shownCompoundModuleIds.toArray(new Integer[0]);
    }

    public boolean showsCompoundModule(int moduleId) {
        return shownCompoundModuleIds.contains(moduleId);
    }

    public void addShownCompoundModule(int moduleId) {
        Assert.isTrue(!shownCompoundModuleIds.contains(moduleId));
        shownCompoundModuleIds.add(moduleId);
    }

    public void removeShownCompoundModule(int moduleId) {
        Assert.isTrue(shownCompoundModuleIds.contains(moduleId));
        shownCompoundModuleIds.remove((Integer)moduleId);
    }

    public AnimationCompoundModuleFigure findAnimationCompoundModuleFigure(int moduleId) {
        for (Object figure : getEditableFigures().toArray(new Object[0])) {
            if (figure instanceof AnimationCompoundModuleFigure) {
                AnimationCompoundModuleFigure animationCompoundModuleFigure = (AnimationCompoundModuleFigure)figure;
                if (animationCompoundModuleFigure.getModuleId() == moduleId)
                    return animationCompoundModuleFigure;
            }
        }

        return null;
    }

    public void removeDecorationFigures() {
        for (IFigure figure : getEditableFigures().toArray(new IFigure[0]))
//            if (!(figure instanceof AnimationCompoundModuleFigure))
                getRootFigure().remove(figure);
    }

    /*************************************************************************************
     * ANIMATION
     */

    public void animationStarted() {
    }

    public void animationStopped() {
    }

    public void animationPositionChanged(AnimationPosition animationPosition) {
        fireSelectionChanged();
    }

    /*************************************************************************************
     * SELECTION
     */

    /**
     * Add a selection change listener.
     */
    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.add(listener);
    }

    /**
     * Remove a selection change listener.
     */
    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.remove(listener);
    }

    /**
     * Notifies any selection changed listeners that the viewer's selection has changed.
     * Only listeners registered at the time this method is called are notified.
     */
    private void fireSelectionChanged() {
        fireSelectionChanged(getSelection());
    }

    private void fireSelectionChanged(ISelection selection) {
        Object[] listeners = selectionChangedListeners.getListeners();
        final SelectionChangedEvent event = new SelectionChangedEvent(this, selection);
        for (int i = 0; i < listeners.length; ++i) {
            final ISelectionChangedListener l = (ISelectionChangedListener) listeners[i];
            SafeRunnable.run(new SafeRunnable() {
                public void run() {
                    l.selectionChanged(event);
                }
            });
        }
    }

    public ISelection getSelection() {
        if (eventLogInput == null)
            return null;
        else {
            EventNumberRangeSet eventNumbers = new EventNumberRangeSet();
            ArrayList<BigDecimal> simulationTimes = new ArrayList<BigDecimal>();
            if (animationController.getCurrentAnimationPosition().isCompletelySpecified()) {
                eventNumbers.add(animationController.getCurrentEventNumber());
                simulationTimes.add(animationController.getCurrentSimulationTime());
            }
            return new EventLogSelection(eventLogInput, eventNumbers, simulationTimes);
        }
    }

    public void setSelection(ISelection selection) {
        if (selection instanceof IEventLogSelection) {
            IEventLogSelection eventLogSelection = (IEventLogSelection)selection;
            EventLogInput selectionEventLogInput = eventLogSelection.getEventLogInput();
            if (eventLogInput != selectionEventLogInput)
                setInput(selectionEventLogInput);
            BigDecimal firstSimulationTime = eventLogSelection.getFirstSimulationTime();
            Long firstEventNumber = eventLogSelection.getFirstEventNumber();
            boolean isSelectionReallyChanged =
                ((firstSimulationTime != null && !firstSimulationTime.equals(animationController.getCurrentSimulationTime())) ||
                 (firstEventNumber != null) && !firstEventNumber.equals(animationController.getCurrentEventNumber()));
            if (isSelectionReallyChanged && !isSelectionChangeInProgress) {
                try {
                    isSelectionChangeInProgress = true;
                    if (firstSimulationTime != null)
                        animationController.gotoSimulationTime(eventLogSelection.getFirstSimulationTime());
                    else if (firstEventNumber != null)
                        animationController.gotoEventNumber(eventLogSelection.getFirstEventNumber());
                    fireSelectionChanged();
                }
                finally {
                    isSelectionChangeInProgress = false;
                }
            }
        }
    }

    public void setSelectedElement(IFigure figure, Object element) {
        if (figure == null || figure instanceof ISelectableFigure) {
            ISelectableFigure selectableFigure = ((ISelectableFigure)figure);
            if (selectedFigure != null)
                selectedFigure.setSelected(false);
            if (selectableFigure != null)
                selectableFigure.setSelected(true);
            selectedFigure = selectableFigure;
            ArrayList<Object> elements = new ArrayList<Object>();
            if (element != null)
                elements.add(element);
            fireSelectionChanged(new EventLogSelection(eventLogInput, elements));
        }
    }

    /*************************************************************************************
     * INPUT HANDLING
     */

    /**
     * Returns the eventlog (data) to be displayed as an animation.
     */
    public IEventLog getEventLog() {
        return eventLog;
    }

    /**
     * Returns the currently displayed EventLogInput object.
     */
    public EventLogInput getInput() {
        return eventLogInput;
    }

    public void setAnimationController(AnimationController animationController) {
        this.animationController = animationController;
    }

    /**
     * Sets a new EventLogInput to be displayed.
     */
    public void setInput(final EventLogInput input) {
        if (input != eventLogInput) {
            // store current settings
            if (eventLogInput != null) {
                eventLogInput.runWithProgressMonitor(new Runnable() {
                    public void run() {
                        eventLogInput.removeEventLogChangedListener(AnimationCanvas.this);
                    }
                });
            }
            // remember input
            eventLogInput = input;
            eventLog = eventLogInput == null ? null : eventLogInput.getEventLog();
            shownCompoundModuleIds.clear();
            if (animationController != null)
                animationController.addAnimationListener(this);
            if (eventLogInput != null) {
                eventLogInput.runWithProgressMonitor(new Runnable() {
                    public void run() {
                        // restore last known settings
                        if (eventLogInput != null) {
                            eventLogInput.addEventLogChangedListener(AnimationCanvas.this);
                        }
                    }
                });
            }
        }
    }

    /*************************************************************************************
     * EVENTLOG EVENT HANDLING
     */

    public void eventLogAppended() {
        // TODO:
    }

    public void eventLogFilterRemoved() {
        // TODO:
    }

    public void eventLogFiltered() {
        // TODO:
    }

    public void eventLogLongOperationStarted() {
        // TODO:
    }

    public void eventLogLongOperationEnded() {
        // TODO:
    }

    public void eventLogOverwritten() {
        // TODO:
    }

    public void eventLogProgress() {
        // TODO:
    }
}
