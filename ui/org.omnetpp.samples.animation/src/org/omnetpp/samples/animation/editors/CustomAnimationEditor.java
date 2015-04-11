/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.samples.animation.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.animation.controller.IAnimationCoordinateSystem;
import org.omnetpp.animation.editors.AnimationEditor;
import org.omnetpp.animation.providers.IAnimationPrimitiveProvider;
import org.omnetpp.animation.widgets.AnimationCanvas;
import org.omnetpp.samples.animation.controller.CustomAnimationCoordinateSystem;
import org.omnetpp.samples.animation.providers.CustomAnimationPrimitiveProvider;
import org.omnetpp.samples.animation.widgets.CustomAnimationCanvas;

public class CustomAnimationEditor extends AnimationEditor
{
    @Override
    protected IAnimationCoordinateSystem createAnimationCoordinateSystem() {
        return new CustomAnimationCoordinateSystem();
    }

    @Override
    protected AnimationCanvas createAnimationCanvas(Composite parent) {
        return new CustomAnimationCanvas(parent, SWT.DOUBLE_BUFFERED);
    }

    @Override
    protected IAnimationPrimitiveProvider createAnimationPrimitiveProvider() {
        return new CustomAnimationPrimitiveProvider(getFile());
    }
}