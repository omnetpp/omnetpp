package org.omnetpp.scave.editors.datatable;

import org.eclipse.jface.action.IMenuManager;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Event;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engineext.ResultFileManagerEx;


/**
 * Common interface for DataTree and DataTable, the controls that display
 * simulation results, using a ResultFileManager and an IDList as input.
 */
public interface IDataControl {
    /* content */

    public ResultFileManagerEx getResultFileManager();

    public void setResultFileManager(ResultFileManagerEx manager);

    public void setNumericPrecision(int numDigitsShown);

    public int getNumericPrecision();

    public IDList getIDList();

    public void setIDList(IDList list);

    public void setLayoutData(Object layoutData);

    public int getItemCount();

    /* control */

    public IMenuManager getContextMenuManager();

    public boolean setFocus();

    public boolean isDisposed();

    /* selection */

    public int getSelectionCount();

    public ResultItem getSelectedItem();

    public ResultItem[] getSelectedItems();

    public String getSelectedField();

    public IDList getSelectedIDs();

    public void selectAll();

    public void setSelectedID(long id);

    public void setSelectedIDs(IDList idList);

    public void copySelectionToClipboard();

    /* listeners */

    public void notifyListeners(int eventType, Event event);

    public void addDataListener(IDataListener dataListener);

    public void removeDataListener(IDataListener listener);

    public void addMouseListener(MouseListener mouseListener);

    public void removeMouseListener(MouseListener mouseListener);

    public void addSelectionListener(SelectionListener listener);

    public void removeSelectionListener(SelectionListener listener);
}
