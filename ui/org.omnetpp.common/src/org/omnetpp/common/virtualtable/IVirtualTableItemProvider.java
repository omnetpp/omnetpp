package org.omnetpp.common.virtualtable;

import org.eclipse.jface.viewers.IBaseLabelProvider;
import org.eclipse.swt.widgets.TableItem;

public interface IVirtualTableItemProvider extends IBaseLabelProvider {
	public void fillTableItem(TableItem item, Object element);
}
