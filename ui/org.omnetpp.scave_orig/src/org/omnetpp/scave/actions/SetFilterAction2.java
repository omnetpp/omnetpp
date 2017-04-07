/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.concurrent.Callable;

import org.eclipse.jface.action.Action;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.datatable.IDataControl;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterUtil;

/**
 * Sets the filter of a filtered data panel.
 * The filter is determined by the selected cell of the panel's control.
 *
 * @author tomi
 */
public class SetFilterAction2 extends Action
{
    FilteredDataPanel panel;
    String fieldName;
    String fieldValue;

    public SetFilterAction2() {
        setDescription("Sets the filter according to the clicked cell.");
        update(null);
    }

    /**
     * This method is called when the active panel or
     * the selected cell is changed.
     * It updates the parameters of the action.
     */
    public void update(final FilteredDataPanel panel) {
        this.panel = panel;
        if (panel != null) {
            ResultFileManager.callWithReadLock(panel.getResultFileManager(), new Callable<Object>() {
                public Object call() {
                    IDataControl control = panel.getDataControl();
                    ResultItem item = control.getSelectedItem();
                    if (item != null && control.getSelectedField() != null) {
                        ResultItemField field = new ResultItemField(control.getSelectedField());
                        fieldName = field.getName();
                        fieldValue = field.getFieldValue(item);

                        if (fieldValue != null) {
                            setText(String.format("Set filter: %s=%s", field.getName(), fieldValue));
                            setEnabled(true);
                            return null;
                        }
                    }
                    setText("Set filter");
                    setEnabled(false);
                    return null;
                }
            });

        }
        setText("Set filter");
        setEnabled(false);
    }

    @Override
    public void run() {
        if (panel != null && fieldName != null && fieldValue != null) {
            FilterUtil filter = new FilterUtil();
            filter.setField(fieldName, fieldValue);
            panel.setFilterParams(new Filter(filter.getFilterPattern()));
        }
    }
}
