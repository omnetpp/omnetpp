/**
 * 
 */
package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.widgets.TableItem;

public class LongVirtualTableItemProvider extends LabelProvider implements IVirtualTableItemProvider {
	public void fillTableItem(TableItem item, Object element) {
		item.setText(0, String.valueOf(element));
	}
}