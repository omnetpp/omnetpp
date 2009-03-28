package org.omnetpp.runtimeenv.views;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.virtualtable.LongVirtualTableContentProvider;
import org.omnetpp.common.virtualtable.LongVirtualTableRowRenderer;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.experimental.simkernel.swig.LogBuffer;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;

public class ModuleOutputView extends ViewPart implements ISimulationListener {
    public static final String ID = "org.omnetpp.runtimeenv.ModuleOutputView";

    protected VirtualTable<Long> table;
    
    /**
	 * This is a callback that will allow us to create the viewer and initialize
	 * it.
	 */
	public void createPartControl(Composite parent) {
	    table = new VirtualTable<Long>(parent, SWT.NONE);

	    //FIXME we don't want no columns or table header, but without the following nothing gets displayed at all
	    //FIXME and table header cannot be turned off
	    //FIXME 1-2 pixels on the left are not visible (off-canvas)
	    //FIXME scroll to the end, then resize: last element gets repeated!!! (this seems to be the problem of LongVirtualTableContentProvider ?)
	    //FIXME how to draw unselected element with different foreground (white)?
	    table.createColumn();
        TableColumn tableColumn = table.createColumn();;
        tableColumn.setWidth(800);  //XXX +Infinity?
        tableColumn.setText("Message");

	    table.setContentProvider(new LongVirtualTableContentProvider());
	    table.setRowRenderer(new LongVirtualTableRowRenderer());
	    table.setInput(new Long(1000));
	    
	    Activator.getSimulationManager().addChangeListener(this);
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
	    table.setFocus();
	}
	
	@Override
	public void changed() {
//	    LogBuffer logBuffer = Activator.getSimulationManager().getLogBuffer();
	    
	    table.refresh();
	}

	@Override
	public void dispose() {
        Activator.getSimulationManager().removeChangeListener(this);
	    super.dispose();
	}

}