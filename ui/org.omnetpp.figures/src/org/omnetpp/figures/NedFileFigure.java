package org.omnetpp.figures;

import org.eclipse.draw2d.CompoundBorder;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.color.ColorFactory;

/**
 * Top level figure corresponding to a NED file
 *
 * @author rhornig
 */
public class NedFileFigure extends Layer {
	// global switch for antialiasing
	public static int antialias = SWT.ON;

	protected String problemMessage;

	private Label problemLabel;

	public NedFileFigure() {
		super();
		ToolbarLayout fl = new ToolbarLayout();
		fl.setStretchMinorAxis(false);
		fl.setSpacing(20);
		setLayoutManager(fl);
		setProblemMessage(null);
	}

	public void setProblemMessage(String problemMessage) {
		this.problemMessage = problemMessage;

		if (problemMessage != null) {
			setBorder(new CompoundBorder(
					new MarginBorder(3),
					new CompoundBorder(
							new LineBorder(ColorFactory.RED, 3),
							new MarginBorder(4))));

			if (problemLabel == null) {
				problemLabel = new Label();
				Image errorImage = PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_OBJS_ERROR_TSK);
				problemLabel.setIcon(errorImage);
				add(problemLabel, 0);
			}

			problemLabel.setText(problemMessage);
		}
		else {
			setBorder(new MarginBorder(10));
			if (problemLabel != null) {
				remove(problemLabel);
				problemLabel = null;
			}
		}
		invalidate();
	}
}
