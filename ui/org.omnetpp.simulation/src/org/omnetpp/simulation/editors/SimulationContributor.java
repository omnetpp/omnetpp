/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.editors;

import org.eclipse.ui.IEditorPart;
import org.omnetpp.animation.editors.AnimationContributorBase;

/**
 * Contributes context menu and toolbar items to the platform.
 *
 * @author andras
 */
public class SimulationContributor extends AnimationContributorBase {
    @Override
    public void setActiveEditor(IEditorPart targetEditor) {
        if (targetEditor instanceof SimulationEditor) {
            if (animationCanvas != null) {
                getPage().removePartListener(this);
                animationCanvas.removeSelectionChangedListener(this);
                animationCanvas.getAnimationController().removeAnimationListener(this);
            }
            animationCanvas = ((SimulationEditor)targetEditor).getAnimationCanvas();
            getPage().addPartListener(this);
            animationCanvas.addSelectionChangedListener(this);
            animationCanvas.getAnimationController().addAnimationListener(this);
            animationPositionContribution.configureSlider();
            update();
        }
        else
            super.setActiveEditor(targetEditor);
    }
    
}
