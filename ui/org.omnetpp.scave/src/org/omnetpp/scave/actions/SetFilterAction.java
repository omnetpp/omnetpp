/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import static org.omnetpp.scave.engine.ResultItemField.FILE;
import static org.omnetpp.scave.engine.ResultItemField.RUN;

import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.BrowseDataPage;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterUtil;
import org.omnetpp.scave.model2.ResultFilePayload;
import org.omnetpp.scave.model2.RunAttributePayload;
import org.omnetpp.scave.model2.RunPayload;

/**
 * Sets the filters on the "Browse Data" page with the attributes of the
 * selected object.
 *
 * @author tomi
 */
public class SetFilterAction extends AbstractScaveAction {

    public SetFilterAction() {
        setText("Set Filter");
        setToolTipText("Set the filter on the \"Browse Data\" page from the current selection.");
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) {
        Object selected = asStructuredOrEmpty(selection).getFirstElement();
        FilterUtil filterUtil = getFilterParams(selected);
        if (filterUtil != null) {
            Filter filter = new Filter(filterUtil.getFilterPattern());
            BrowseDataPage page = scaveEditor.getBrowseDataPage();
            page.getAllPanel().setFilterParams(filter);
            page.getScalarsPanel().setFilterParams(filter);
            page.getVectorsPanel().setFilterParams(filter);
            page.getHistogramsPanel().setFilterParams(filter);
            scaveEditor.showPage(page);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }

    /**
     * Returns a filter object with the attributes of the selected object and their ancestors.
     * Returns non-null if the filter is to be applied on the "Browse Data" page.
     */
    protected FilterUtil getFilterParams(Object object) {

        if (object instanceof InputFile) {
            FilterUtil filterUtil = new FilterUtil();
            InputFile inputFile = (InputFile)object;
            filterUtil.setField(FILE, inputFile.getName());
            return filterUtil;
        }
        else if (object instanceof GenericTreeNode) {
            // GenericTreeNode trees are used on the Inputs page.
            GenericTreeNode node = (GenericTreeNode)object;
            Object payload = node.getPayload();

            // get params up to the root
            FilterUtil filterUtil = getFilterParams(node.getParent());
            if (filterUtil==null)
                filterUtil = new FilterUtil();

            if (payload instanceof ResultFilePayload) {
                ResultFilePayload resultFile = (ResultFilePayload)payload;
                filterUtil.setField(FILE, resultFile.getFilePath());
            }
            else if (payload instanceof RunPayload) {
                RunPayload run = (RunPayload)payload;
                filterUtil.setField(RUN, run.getRunName());
            }
            else if (payload instanceof RunAttributePayload) {
                RunAttributePayload attr = (RunAttributePayload)payload;
                String name = attr.getName();
                String value = attr.getValue();
                filterUtil.setField(name, value);
            }
            return filterUtil;
        }

        return null;
    }
}
