/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.swt.SWT;

/**
 * Preferred values must be higher than the minimum values. SWT.DEFAULT means value is not provided.
 *
 * @author levy
 */
public class SizeConstraint {
    public int minimumWidth = SWT.DEFAULT;

    public int minimumHeight = SWT.DEFAULT;

    public int maximumWidth = SWT.DEFAULT;

    public int maximumHeight = SWT.DEFAULT;

    public int preferredWidth = SWT.DEFAULT;

    public int preferredHeight = SWT.DEFAULT;
}
