package org.omnetpp.scave.editors.datatable;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.common.virtualtable.IVirtualTableItemProvider;
import org.omnetpp.scave.engine.OutputVectorEntry;

/**
 * Implementation of the IVirtualTableItemProvier interface for
 * the table of vector data.
 *
 * @author tomi
 */
public class VectorResultLabelProvider extends LabelProvider implements IVirtualTableItemProvider {
	
	public void fillTableItem(TableItem item, Object element) {
		OutputVectorEntry entry = (OutputVectorEntry)element;
		item.setText(0, String.valueOf(entry.getSerial()));
		item.setText(1, String.valueOf(entry.getSimtime()));
		item.setText(2, String.valueOf(entry.getValue()));
	}
}
