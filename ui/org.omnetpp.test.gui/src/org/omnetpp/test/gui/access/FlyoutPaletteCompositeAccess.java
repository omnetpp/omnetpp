package org.omnetpp.test.gui.access;

import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.text.TextFlow;
import org.eclipse.gef.ui.palette.FlyoutPaletteComposite;
import org.eclipse.gef.ui.palette.PaletteViewer;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.test.gui.core.InUIThread;

public class FlyoutPaletteCompositeAccess extends CompositeAccess
{
	public FlyoutPaletteCompositeAccess(FlyoutPaletteComposite flyoutPaletteComposite) {
		super(flyoutPaletteComposite);
	}
	
	public FlyoutPaletteComposite getFlyoutPaletteComposite() {
		return (FlyoutPaletteComposite)widget;
	}
	
	@InUIThread
	public void clickButtonWithLabel(final String label) {
		ensureExpanded();
		PaletteViewer paletteViewer = (PaletteViewer)ReflectionUtils.getFieldValue(getFlyoutPaletteComposite(), "pViewer");
		IFigure rootFigure = (IFigure)ReflectionUtils.getFieldValue(paletteViewer, "rootFigure");
		IFigure figure = findDescendantFigure(rootFigure, new IPredicate() {
			public boolean matches(Object object) {
				return object instanceof TextFlow && ((TextFlow)object).getText().matches(label);
			}
		});
		
		Composite paletteContainer = (Composite)ReflectionUtils.getFieldValue(getFlyoutPaletteComposite(), "paletteContainer");
		Control figureCanvas = findDescendantControl(paletteContainer, FigureCanvas.class);
		new FigureAccess(figure).click(LEFT_MOUSE_BUTTON, figureCanvas);
	}

	@InUIThread
	public void ensureExpanded() {
		Object stateExpanded = ReflectionUtils.getFieldValue(FlyoutPaletteComposite.class, "STATE_EXPANDED");
		if (!(Boolean)ReflectionUtils.invokeMethod(getFlyoutPaletteComposite(), "isInState", stateExpanded))
			clickFlyoutControlButton();
	}

	@InUIThread
	public void clickFlyoutControlButton() {
		Control sash = (Control)ReflectionUtils.getFieldValue(getFlyoutPaletteComposite(), "sash");
		Canvas button = (Canvas)ReflectionUtils.getFieldValue(sash, "button");
		click(LEFT_MOUSE_BUTTON, sash.toDisplay(getCenter(button.getBounds())));
	}
}
