package org.omnetpp.scave.actions;

import org.eclipse.jface.action.Action;
import org.omnetpp.scave.editors.datatable.DataTable;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterUtil;
import org.omnetpp.scave.model2.ResultItemFields2;

/**
 * Sets the filter of a filtered data panel.
 * The filter is determined by the selected cell of the table of the panel. 
 *
 * @author tomi
 */
public class SetFilterAction2 extends Action
{
	FilteredDataPanel panel;
	ResultItem item;
	String field;
	String value;
	
	public SetFilterAction2() {
		setDescription("Sets the filter according to the clicked table cell.");
		update(null);
	}
	
	/**
	 * This method is called when the active panel or
	 * the selected cell of the table changed.
	 * It updates the parameters of the action.
	 */
	public void update(FilteredDataPanel panel) {
		this.panel = panel;
		if (panel != null) {

			DataTable table = panel.getTable();
			item = table.getSelectedItem();
			field = table.getSelectedField();
			value = ResultItemFields2.getFieldValue(item, field);

			if (field != null && value != null) {
				setText(String.format("Set filter: %s=%s", field, value));
				setEnabled(true);
				return;
			}
		}
		setText("Set filter");
		setEnabled(false);
	}
	
	@Override
	public void run() {
		if (panel != null && field != null && value != null) {
			FilterUtil filter = new FilterUtil();
			filter.setField(field, value);
			panel.setFilterParams(new Filter(filter.getFilterPattern()));
		}
	}
}
