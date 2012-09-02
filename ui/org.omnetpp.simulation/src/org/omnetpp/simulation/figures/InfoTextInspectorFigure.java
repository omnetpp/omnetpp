package org.omnetpp.simulation.figures;

import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.RoundedRectangle;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.jface.resource.JFaceResources;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.simulation.inspectors.IInspectorFigure;

/**
 * Experimental inspector figure, for InfoTextInspectorPart
 * @author Andras
 */
public class InfoTextInspectorFigure extends RoundedRectangle implements IInspectorFigure {
    protected Label nameLabel;
    protected Label infoStringLabel;
    protected Insets insets = new Insets(4, 8, 4, 8);

    public InfoTextInspectorFigure() {
        setBackgroundColor(ColorFactory.LIGHT_BLUE2);
        setLayoutManager(new ToolbarLayout());

        nameLabel = new Label();
        nameLabel.setForegroundColor(ColorFactory.GREY25);
        add(nameLabel);

        infoStringLabel = new Label();
        infoStringLabel.setFont(JFaceResources.getBannerFont()); //XXX
        add(infoStringLabel);
    }

    @Override
    public Insets getInsets() {
        return insets;
    }

    public void setTexts(String name, String info) {
        nameLabel.setText(name);
        infoStringLabel.setText(info);
        //XXX re-layout if size changed?
    }
}
