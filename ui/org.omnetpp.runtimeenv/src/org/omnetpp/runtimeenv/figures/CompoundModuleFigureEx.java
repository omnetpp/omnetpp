package org.omnetpp.runtimeenv.figures;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.LayoutListener;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.geometry.Dimension;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.layout.CompoundModuleLayout;
import org.omnetpp.runtimeenv.editors.IInspectorFigure;
import org.omnetpp.runtimeenv.editors.IInspectorPart;

//FIXME draw proper resize handles, and make the mouse listener recognize it
public class CompoundModuleFigureEx extends ScrollPane implements IInspectorFigure {
	protected CompoundModuleFigure moduleFigure;
    protected IInspectorPart inspectorPart;

    public CompoundModuleFigureEx() {
    	moduleFigure = new CompoundModuleFigure();
    	setContents(moduleFigure);

        setMinimumSize(new Dimension(20,20));
        //setBorder(new LineBorder(2));

        // fix crappy scrollbar behavior of ScrollPane
        //FIXME needed?
        addLayoutListener(new LayoutListener.Stub() {
			public void postLayout(IFigure container) {
				setHorizontalScrollBarVisibility(getSize().width >= getContents().getSize().width ? ScrollPane.NEVER : ScrollPane.ALWAYS);
				setVerticalScrollBarVisibility(getSize().height >= getContents().getSize().height ? ScrollPane.NEVER : ScrollPane.ALWAYS);
			}
        });
    }

    public IInspectorPart getInspectorPart() {
        return inspectorPart;
    }

    public void setInspectorPart(IInspectorPart inspectorPart) {
        this.inspectorPart = inspectorPart;
    }

    //@Override
    public int getDragOperation(int x, int y) {
    	return FigureUtils.getBorderMoveResizeDragOperation(x, y, getBounds());
    }

    public CompoundModuleFigure getRealModuleFigure() {
		return moduleFigure;
	}

	public Layer getSubmoduleLayer() {
		return moduleFigure.getSubmoduleLayer();
	}

	public Layer getConnectionLayer() {
		return moduleFigure.getConnectionLayer();
	}

	public CompoundModuleLayout getSubmoduleLayouter() {
		return (CompoundModuleLayout)moduleFigure.getSubmoduleLayer().getLayoutManager();
	}

    public void setDisplayString(IDisplayString dps) {
		moduleFigure.setDisplayString(dps);
	}

    @Override
    public void setMaximumSize(Dimension d) {
    	throw new UnsupportedOperationException();
    }

	@Override
	public Dimension getMaximumSize() {
		return getContents().getSize();
	}

	//@Override
    public void setSelectionBorder(boolean isSelected) {
        setBorder(isSelected ? new SelectionBorder() : null); //XXX for now
    }

	public boolean getSelectionBorderShown() {
        return getBorder() != null; //XXX for now
    }
}
