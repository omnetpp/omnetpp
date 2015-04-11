/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.panel;

import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;

/**
 * ScalarPanelComposite and VectorPanelComposite inherit from this class;
 * the purpose is that they can share the same controller, FilterPanel.
 */
public abstract class FilterPanelComposite extends Composite {

    public FilterPanelComposite(Composite parent, int style) {
        super(parent, style);
    }

    public abstract Button getFilterButton();

    public abstract CCombo getModuleNameCombo();

    public abstract CCombo getRunNameCombo();

    public abstract CCombo getNameCombo();

    public abstract Table getTable();

}