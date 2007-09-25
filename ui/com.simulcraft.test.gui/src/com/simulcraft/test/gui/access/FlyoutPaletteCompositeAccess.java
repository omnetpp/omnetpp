package com.simulcraft.test.gui.access;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.text.TextFlow;
import org.eclipse.gef.ui.palette.FlyoutPaletteComposite;
import org.eclipse.gef.ui.palette.PaletteViewer;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.core.InUIThread;


public class FlyoutPaletteCompositeAccess extends CompositeAccess
{
	public FlyoutPaletteCompositeAccess(FlyoutPaletteComposite flyoutPaletteComposite) {
		super(flyoutPaletteComposite);
	}
	
	public FlyoutPaletteComposite getFlyoutPaletteComposite() {
		return (FlyoutPaletteComposite)widget;
	}
	
	@InUIThread
	public void clickButtonFigureWithLabel(final String label) {
		findButtonFigureWithLabel(label).click(LEFT_MOUSE_BUTTON);
	}

    @InUIThread
    public FigureAccess findButtonFigureWithLabel(final String label) {
        ensureExpanded();
        return new FigureAccess(findDescendantFigure(getRootFigure(getPaletteViewer()), new IPredicate() {
            public boolean matches(Object object) {
                return object instanceof TextFlow && ((TextFlow)object).getText().matches(label);
            }
        }));
    }

    @InUIThread
    public boolean hasButtonFigureWithLabel(final String label) {
        ensureExpanded();
        IFigure rootFigure = getRootFigure(getPaletteViewer());
        IPredicate predicate = new IPredicate() {
            public boolean matches(Object object) {
                return object instanceof TextFlow && ((TextFlow)object).getText().matches(label);
            }
        };

        return hasDescendantFigure(rootFigure, predicate) && findDescendantFigure(rootFigure, predicate).isVisible();
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
		clickAbsolute(LEFT_MOUSE_BUTTON, sash.toDisplay(getCenter(button.getBounds())));
	}

    private IFigure getRootFigure(PaletteViewer paletteViewer) {
        return (IFigure)ReflectionUtils.getFieldValue(paletteViewer, "rootFigure");
    }

    private PaletteViewer getPaletteViewer() {
        return (PaletteViewer)ReflectionUtils.getFieldValue(getFlyoutPaletteComposite(), "pViewer");
    }
}
