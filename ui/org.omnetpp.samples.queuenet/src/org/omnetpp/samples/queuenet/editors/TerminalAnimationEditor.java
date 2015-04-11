/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.samples.queuenet.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.animation.controller.EventLogAnimationCoordinateSystem;
import org.omnetpp.animation.controller.IAnimationCoordinateSystem;
import org.omnetpp.animation.editors.AnimationEditor;
import org.omnetpp.animation.providers.IAnimationPrimitiveProvider;
import org.omnetpp.animation.widgets.AnimationCanvas;
import org.omnetpp.samples.queuenet.animation.providers.TerminalAnimationPrimitiveProvider;
import org.omnetpp.samples.queuenet.widgets.TerminalAnimationCanvas;

public class TerminalAnimationEditor extends AnimationEditor
{
    @Override
    protected IAnimationCoordinateSystem createAnimationCoordinateSystem() {
        return new EventLogAnimationCoordinateSystem(eventLogInput);
    }

    @Override
    protected AnimationCanvas createAnimationCanvas(Composite parent) {
        return new TerminalAnimationCanvas(parent, SWT.DOUBLE_BUFFERED);
    }

    @Override
    protected IAnimationPrimitiveProvider createAnimationPrimitiveProvider() {
        return new TerminalAnimationPrimitiveProvider();
    }
}