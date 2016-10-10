/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.CompoundBorder;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.editor.NedEditorPlugin;

/**
 * Top level figure corresponding to a NED file
 *
 * @author rhornig
 */
public class NedFileFigure extends Layer {
    // global switch for antialiasing
    private static int antialias = SWT.ON;

    protected String problemMessage;

    private Layer topLevelTypesLayer = new Layer();
    private Label problemLabel;
    private Label packageLabel = new Label();

    public NedFileFigure() {
        super();
        packageLabel.setIcon(NedEditorPlugin.getCachedImage("icons/obj16/package_obj.gif"));

        ToolbarLayout typeLayout = new ToolbarLayout();
        typeLayout.setStretchMinorAxis(false);
        typeLayout.setSpacing(20);
        topLevelTypesLayer.setLayoutManager(typeLayout);

        ToolbarLayout fl = new ToolbarLayout();
        fl.setStretchMinorAxis(true);
        fl.setSpacing(10);
        setLayoutManager(fl);
        setProblemMessage(null);
        setPackageName(null);
        packageLabel.setBorder(new MarginBorder(2));
        packageLabel.setOpaque(true);
        packageLabel.setLabelAlignment(PositionConstants.LEFT);
        // use only the super.add(figure,constraint,index) to add figures as a direct child
        // because the other add() methods are overridden to add the children to the topLevelTypeLayer
        super.add(packageLabel, null, -1);
        super.add(topLevelTypesLayer, null, -1);
    }


    // overriding the add/remove pair to add children to the topLevelTypesLayer
    @Override
    public void add(IFigure figure, Object constraint, int index) {
        topLevelTypesLayer.add(figure, constraint, index);
    }

    @Override
    public void remove(IFigure figure) {
        topLevelTypesLayer.remove(figure);
    }

    /**
     * The package name this file belongs to. If set to "" or null it means default package and
     * the corresponding label is removed from the top of the figure.
     */
    public void setPackageName(String packageName) {
        if (StringUtils.isEmpty(packageName)) {
            if (packageLabel.getParent() == this)
                super.remove(packageLabel);
        }
        else {
            if (packageLabel.getParent() != this)
                super.add(packageLabel, null, 0);
            packageLabel.setText("package "+packageName);
        }
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
                problemLabel.setLabelAlignment(PositionConstants.LEFT);
                super.add(problemLabel, null, 0);  // must call the non-overridden version to add as a child of this figure and not as a chile of topLevelTypes
            }

            problemLabel.setText(problemMessage);
        }
        else {
            setBorder(new MarginBorder(10));
            if (problemLabel != null) {
                super.remove(problemLabel);
                problemLabel = null;
            }
        }
        invalidate();
    }

    @Override
    public void paint(Graphics graphics) {
        // set antialiasing on content and child/derived figures
        if (NedFileFigure.antialias != SWT.DEFAULT)
            graphics.setAntialias(NedFileFigure.antialias);
        super.paint(graphics);
    }
}
