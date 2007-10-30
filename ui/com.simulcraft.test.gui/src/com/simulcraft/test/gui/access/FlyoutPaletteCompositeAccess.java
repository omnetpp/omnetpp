package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.draw2d.Clickable;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.text.TextFlow;
import org.eclipse.gef.ui.palette.FlyoutPaletteComposite;
import org.eclipse.gef.ui.palette.PaletteViewer;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.core.UIStep;


public class FlyoutPaletteCompositeAccess extends CompositeAccess
{
	public FlyoutPaletteCompositeAccess(FlyoutPaletteComposite flyoutPaletteComposite) {
		super(flyoutPaletteComposite);
	}
	
	public FlyoutPaletteComposite getFlyoutPaletteComposite() {
		return (FlyoutPaletteComposite)widget;
	}
	
	@UIStep
	public void clickButtonFigureWithLabel(final String label) {
        findButtonFigureWithLabel(label).reveal();
        findButtonFigureWithLabel(label).click(LEFT_MOUSE_BUTTON);
	}

    @UIStep
    public void ensureButtonFigureWithLabelSelected(final String label) {
        findButtonFigureWithLabel(label).reveal();
        FigureAccess fa = findButtonFigureWithLabel(label);
        IFigure fig;
        for(fig = fa.getFigure(); !(fig instanceof Clickable) && fig != null; fig = fig.getParent());
        Assert.assertNotNull("Figure must be clickable", fig);
        
        if (!((Clickable)fig).isSelected())
            fa.click(LEFT_MOUSE_BUTTON);
    }

    @UIStep
    public FigureAccess findButtonFigureWithLabel(final String label) {
        ensurePinnedOpen();
        return new FigureAccess(findDescendantFigure(getRootFigure(getPaletteViewer()), new IPredicate() {
            public boolean matches(Object object) {
                return object instanceof TextFlow && ((TextFlow)object).getText().matches(label);
            }
        }));
    }

    @UIStep
    public boolean hasButtonFigureWithLabel(final String label) {
        ensurePinnedOpen();
        IFigure rootFigure = getRootFigure(getPaletteViewer());
        IPredicate predicate = new IPredicate() {
            public boolean matches(Object object) {
                return object instanceof TextFlow && ((TextFlow)object).getText().matches(label);
            }
        };

        return hasDescendantFigure(rootFigure, predicate) && findDescendantFigure(rootFigure, predicate).isVisible();
    }

	@UIStep
	public void ensurePinnedOpen() {
		Object stateExpanded = ReflectionUtils.getFieldValue(FlyoutPaletteComposite.class, "STATE_PINNED_OPEN");
		if (!(Boolean)ReflectionUtils.invokeMethod(getFlyoutPaletteComposite(), "isInState", stateExpanded))
			clickFlyoutControlButton();
	}

	@UIStep
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
