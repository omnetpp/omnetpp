/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Platform;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.FigureUtilities;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.TextUtilities;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.IProblemDecorationSupport;
import org.omnetpp.figures.ITooltipTextProvider;
import org.omnetpp.figures.misc.FigureUtils;
import org.omnetpp.ned.editor.graph.misc.IDirectEditSupport;
import org.omnetpp.ned.editor.graph.misc.LabelCellEditorLocator;


/**
 * Common base for ModuleTypeFigure and ConnectionTypeFigure. Provides an editable name
 * in a vertical toolbar layout so additional figures can be added if needed (i.g. compound module)
 * below the title.
 *
 * @author rhornig
 */
abstract public class NedTypeFigure extends Figure implements IDirectEditSupport,
                IProblemDecorationSupport, ITooltipTextProvider {

    // This is a global hack to increase the text size returned by the textExtents function
    // on Windows, because it is too small if anti aliased fonts are used. Once
    // textExtent works correctly it can be removed
    static {
        TextUtilities.INSTANCE = new TextUtilities() {
            public Dimension getStringExtents(String s, Font f) {
                Dimension stringExtents = FigureUtilities.getStringExtents(s, f);
                return Platform.OS_WIN32.equals(Platform.getOS()) ? stringExtents.expand(2,0) : stringExtents;
            }

            public Dimension getTextExtents(String s, Font f) {
                Dimension textExtents = FigureUtilities.getTextExtents(s, f);
                return Platform.OS_WIN32.equals(Platform.getOS()) ? textExtents.expand(2,0) : textExtents;
            }
        };
    }

    protected String tmpName;
    protected Label nameFigure = new Label();
    protected Layer titleArea = new Layer();
    protected ImageFigure problemMarkerFigure = new ImageFigure(); // FIXME create it on demand
    protected ITooltipTextProvider problemMarkerTextProvider;
    protected String tooltipText;

    private boolean isInterface;
    private boolean isInnerType;

    public NedTypeFigure() {
        // all ned types has a vertical toolbar layout. The topmost child is the
        // title figure (containing an icon,the name of the type and an error indicator figure)
        // Usually this is the only child, but CompoundModuleType contains
        // may additional children where the submodules are displayed or
        // a compartment where the inner types are present.
        ToolbarLayout tb = new ToolbarLayout();
        tb.setSpacing(2);
        tb.setStretchMinorAxis(false);
        setLayoutManager(tb);

        // name label alignment
        nameFigure.setLabelAlignment(PositionConstants.LEFT);
        nameFigure.setTextAlignment(PositionConstants.EAST);
        nameFigure.setTextPlacement(PositionConstants.MIDDLE);

        titleArea.setLayoutManager(new XYLayout());  // XXX we could use a horizontal toolbar possibly if the problem figure could be placed before the icon (instead of over)
        titleArea.add(problemMarkerFigure, new Rectangle(-1, 0, 16, 16));
        titleArea.add(nameFigure);

        add(titleArea);
    }

    @Override
    protected boolean useLocalCoordinates() {
        return true;
    }

    /**
     * Stores the display string, and updates the figure accordingly.
     * @param dps
     * @param project the icons are searched in this project
     * @param scale TODO
     */
    abstract public void setDisplayString(IDisplayString dps, IProject project, float scale);

    public boolean isInterface() {
        return isInterface;
    }

    public void setInterface(boolean isInterface) {
        this.isInterface = isInterface;
    }

    public boolean isInnerType() {
        return isInnerType;
    }

    public void setInnerType(boolean isInnerType) {
        this.isInnerType = isInnerType;
    }

    public void setTooltipText(String tttext) {
        tooltipText = tttext;
    }

    public String getTooltipText(int x, int y) {
        // if there is a problem marker and an associated tooltip text provider
        // and the cursor is over the marker, delegate to the problem marker text provider
        if (problemMarkerTextProvider != null && problemMarkerFigure != null) {
            Rectangle markerBounds = problemMarkerFigure.getBounds().getCopy();
            translateToParent(markerBounds);
            translateToAbsolute(markerBounds);
            if (markerBounds.contains(x, y)) {
                String text = problemMarkerTextProvider.getTooltipText(x, y);
                if (text != null)
                    return text;
            }
        }
        return tooltipText;
    }

    public void setProblemDecoration(int maxSeverity, ITooltipTextProvider textProvider) {
        Image image = FigureUtils.getProblemImageFor(maxSeverity);
        if (image != null)
            problemMarkerFigure.setImage(image);
        problemMarkerFigure.setVisible(image != null);

        problemMarkerTextProvider = textProvider;
        repaint();
    }

    @Override
    public String toString() {
        return getClass().getSimpleName()+" "+nameFigure.getText();
    }

    public void setName(String text) {
        nameFigure.setText(text);
    }

    // Direct edit support

    public String getName() {
        return nameFigure.getText();
    }

    public CellEditorLocator getDirectEditCellEditorLocator() {
        return new LabelCellEditorLocator(nameFigure);
    }

    public void showLabelUnderCellEditor(boolean visible) {
        // HACK to hide the text part only of the label
        if (!visible) {
            tmpName = nameFigure.getText();
            nameFigure.setText("");
        }
        else {
            if (StringUtils.isEmpty(nameFigure.getText()))
                nameFigure.setText(tmpName);
        }
        invalidate();
        validate();
    }

}