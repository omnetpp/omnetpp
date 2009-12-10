/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import org.omnetpp.ned.model.INEDElement;

/**
 * Interface to allow selection of a toplevel component or a line range from
 * outside of a graphical editor
 *
 * @author rhornig
 */
public interface IGotoNedElement {
        enum Mode {AUTOMATIC, GRAPHICAL, TEXT};

        /**
         * Opens the editor and shown the specified INEDElement
         * @param model Th ned element to be shown
         * @param mode Which editor should be used
         */
        public void showInEditor(INEDElement model, Mode mode);
}
