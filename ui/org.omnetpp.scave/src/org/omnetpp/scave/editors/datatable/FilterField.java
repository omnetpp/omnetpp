/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.contentassist.ContentAssistUtil;
import org.omnetpp.scave.model2.FilterHints;

public class FilterField {

    Text text;
    FilterContentProposalProvider proposalProvider;

    public FilterField(Composite parent, int style) {
        this(new Text(parent, style));
    }

    public FilterField(Text text) {
        this.text = text;
        this.proposalProvider = new FilterContentProposalProvider();
        ContentAssistUtil.configureText(text, proposalProvider);
    }

    public Text getText() {
        return text;
    }

    public Control getLayoutControl() {
        return text;
    }

    public void setFilterHints(FilterHints hints) {
        proposalProvider.setFilterHints(hints);
    }
}
