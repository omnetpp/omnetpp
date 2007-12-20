package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.charting.ChartCanvas;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Zooms in/out the chart of the active chart page in the active Scave editor.
 *
 * @author tomi
 */
public class ZoomChartAction extends AbstractScaveAction {
	
	private double zoomFactor;
	private boolean horizontally;
	private boolean vertically;

	public ZoomChartAction(boolean horizontally, boolean vertically, double zoomFactor) {
		this.horizontally = horizontally;
		this.vertically = vertically;
		this.zoomFactor = zoomFactor;

		String inout = (zoomFactor == 0.0 ? "To Fit" :
						zoomFactor > 1.0 ? "In" : "Out");
		String dir = (horizontally && vertically) ? "" : (horizontally ? " X" : " Y");
		String dir2 = (horizontally && vertically) ? "" : (horizontally ? " Horizontally" : " Vertically");
		setText("Zoom " +  inout + dir);
		setDescription("Zoom " + inout.toLowerCase() + " chart " + dir2.toLowerCase());
		String imageId =
			zoomFactor == 0.0 ? ImageFactory.TOOLBAR_IMAGE_ZOOMTOFIT :
			zoomFactor > 1.0 ? ImageFactory.TOOLBAR_IMAGE_ZOOMPLUS : ImageFactory.TOOLBAR_IMAGE_ZOOMMINUS;
		setImageDescriptor(ImageFactory.getDescriptor(imageId));
	}

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		ChartCanvas canvas = scaveEditor.getActiveChartCanvas();
		if (canvas != null) {
			if (horizontally) {
				if (zoomFactor == 0.0)
					canvas.zoomToFitX();
				else
					canvas.zoomXBy(zoomFactor);
			}
			if (vertically) {
				if (zoomFactor == 0.0)
					canvas.zoomToFitY();
				else
					canvas.zoomYBy(zoomFactor);
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		ChartCanvas canvas = editor.getActiveChartCanvas();
		if (canvas != null) {
			return zoomFactor > 1.0 ||
				   horizontally && canvas.getMinZoomX() < canvas.getZoomX() ||
				   vertically && canvas.getMinZoomY() < canvas.getZoomY();
		}
		return false;
	}
}
