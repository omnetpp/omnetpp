/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import org.omnetpp.ned.model.INedElement;

/**
 * Interface to allow selection of a toplevel component or a line range from
 * outside of a graphical editor
 *
 * @author rhornig
 */
public interface IGotoNedElement {
        enum Mode {AUTOMATIC, GRAPHICAL, TEXT};

        /**
         * Opens the editor and shown the specified INedElement
         * @param model Th ned element to be shown
         * @param mode Which editor should be used
         */
        public void showInEditor(INedElement model, Mode mode);
}
