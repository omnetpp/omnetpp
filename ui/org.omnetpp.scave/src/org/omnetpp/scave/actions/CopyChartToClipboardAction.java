package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.charting.ChartCanvas;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Copy chart contents to the clipboard.
 */
public class CopyChartToClipboardAction extends AbstractScaveAction {
	public CopyChartToClipboardAction() {
		setText("Copy to Clipboard");
		setToolTipText("Copy Chart to Clipboard");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		final ChartCanvas chart = editor.getActiveChartCanvas();
		if (chart != null) {
			BusyIndicator.showWhile(Display.getDefault(), new Runnable() {
				public void run() {
					chart.copyToClipboard();
				}
			});
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return editor.getActiveChartCanvas() != null;
	}
}
