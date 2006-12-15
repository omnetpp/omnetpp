package org.omnetpp.scave.actions;

import static org.omnetpp.scave.model2.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.model2.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.model2.RunAttribute.REPLICATION;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.treeproviders.GenericTreeNode;
import org.omnetpp.scave.editors.ui.BrowseDataPage;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.RunAttribute;

/**
 * Sets the filters on the "Browse data" page with the attributes of the 
 * selected object.
 *
 * @author tomi
 */
public class SetFilterAction extends AbstractScaveAction {
	
	public SetFilterAction() {
		setText("Set filter");
		setToolTipText("Set the filter on the \"Browse data\" page from the current selection.");
	}

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		Object selected = selection.getFirstElement();
		Filter filter = new Filter();

		if (getFilterParams(filter, selected)) {
			BrowseDataPage page = scaveEditor.getBrowseDataPage();
			page.getScalarsPanel().setFilterParams(filter);
			page.getVectorsPanel().setFilterParams(filter);
			page.getHistogramsPanel().setFilterParams(filter);
			scaveEditor.showPage(page);
		}
	}
	
	@Override
	protected boolean isApplicable(ScaveEditor editor,
			IStructuredSelection selection) {
		return true;
	}
	
	/**
	 * Fills the given filter object with the attributes of the selected object and their ancestors.
	 * Returns true if the filter is to be applied on the "Browse data" page.
	 */
	protected boolean getFilterParams(Filter filter, Object object) {
		
		if (object instanceof InputFile) {
			InputFile inputFile = (InputFile)object;
			filter.setField(Filter.FIELD_FILENAME, inputFile.getName());
			return true;
		}
		else if (object instanceof GenericTreeNode) {
			GenericTreeNode node = (GenericTreeNode)object;
			Object payload = node.getPayload();
			
			// get params up to the root
			getFilterParams(filter, node.getParent());
			
			if (payload instanceof ResultFile) {
				ResultFile resultFile = (ResultFile)payload;
				filter.setField(Filter.FIELD_FILENAME, resultFile.getFilePath());
				return true;
			}
			else if (payload instanceof Run) {
				Run run = (Run)payload;
				filter.setField(Filter.FIELD_RUNNAME, run.getRunName());
				return true;
			}
			else if (payload instanceof RunAttribute) {
				RunAttribute attr = (RunAttribute)payload;
				String name = attr.getName();
				String value = attr.getValue();
				if (EXPERIMENT.equals(name))
					filter.setField(Filter.FIELD_EXPERIMENT, value);
				else if (MEASUREMENT.equals(name))
					filter.setField(Filter.FIELD_MEASUREMENT, value);
				else if (REPLICATION.equals(name))
					filter.setField(Filter.FIELD_REPLICATION, value);
				return true;
			}
		}
		
		return false;
	}
}
