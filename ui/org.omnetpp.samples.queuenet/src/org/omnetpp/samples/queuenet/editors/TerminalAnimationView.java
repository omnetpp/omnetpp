/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.samples.queuenet.editors;

import org.omnetpp.animation.editors.AnimationView;
import org.omnetpp.animation.providers.IAnimationPrimitiveProvider;
import org.omnetpp.animation.providers.TerminalAnimationPrimitiveProvider;

public class TerminalAnimationView extends AnimationView {
    @Override
    protected IAnimationPrimitiveProvider createAnimationPrimitiveProvider() {
        return new TerminalAnimationPrimitiveProvider();
    }
}
