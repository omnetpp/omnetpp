package org.omnetpp.scave.actions;

import static org.omnetpp.common.image.ImageFactory.TOOLBAR_IMAGE_REFRESH;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.editors.ui.ChartSheetPage;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;

/**
 * Refreshes the charts of the active chart page or 
 * chart sheet page of the active Scave editor.
 *
 * @author tomi
 */
public class RefreshChartAction extends AbstractScaveAction {
	
	public RefreshChartAction() {
		setText("Refresh");
		setDescription("Refresh the content of the current chart");
		setImageDescriptor(ImageFactory.getDescriptor(TOOLBAR_IMAGE_REFRESH));
	}

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		ScaveEditorPage page = scaveEditor.getActiveEditorPage();
		if (page instanceof ChartPage)
			((ChartPage)page).updateChart();
		else if (page instanceof ChartSheetPage)
			((ChartSheetPage)page).updateCharts();
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		ScaveEditorPage page = editor.getActiveEditorPage();
		return page instanceof ChartPage || page instanceof ChartSheetPage;
	}
}
