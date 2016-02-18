/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.export;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.figures.misc.FigureUtils;

class ExportDiagramPreview implements PaintListener
{
    private Canvas previewCanvas;
    private Image previewImage;
    private List<GraphicalEditPart> selectedEditParts;
    private Dimension maxImageSize;
    private Button selectionOnlyCheckButton;
    private Button antialiasCheckbox;

    public ExportDiagramPreview(List<GraphicalEditPart> selectedEditParts)
    {
        this.selectedEditParts = selectedEditParts;
    }

    public boolean isShowSelectionOnly()
    {
        return selectionOnlyCheckButton.getSelection();
    }

    public boolean isAntialias()
    {
        return antialiasCheckbox.getSelection();
    }

    public void setAntialias(boolean antialias)
    {
        antialiasCheckbox.setSelection(antialias);
    }

    public Control createControl(Composite parent)
    {
        Composite composite = new Composite(parent, SWT.NULL);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        GridLayout layout = new GridLayout();
        layout.numColumns = 1;
        layout.verticalSpacing = 4;
        composite.setLayout(layout);

        Label previewLabel = new Label(composite, SWT.NULL);
        previewLabel.setText("Preview");
        previewLabel.setToolTipText("Preview of the generated image.");

        maxImageSize = new Dimension(composite.getDisplay().getClientArea().width, composite.getDisplay().getClientArea().height);
        previewCanvas = new Canvas(composite, SWT.BORDER);
        previewCanvas.setToolTipText("Preview of the generated image.");
        previewCanvas.addPaintListener(this);
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true, layout.numColumns, 1);
        gridData.minimumWidth = 100;
        gridData.minimumHeight = 100;
        previewCanvas.setLayoutData(gridData);

        selectionOnlyCheckButton = new Button(composite, SWT.CHECK);
        selectionOnlyCheckButton.setText("&Selection only");
        selectionOnlyCheckButton.setToolTipText("Export only the current selection.");
        selectionOnlyCheckButton.setSelection(!NedFigureProvider.isWholeDiagramSelected(selectedEditParts));
        selectionOnlyCheckButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e)
            {
                update();
            }
        });
        selectionOnlyCheckButton.setEnabled(!NedFigureProvider.isWholeDiagramSelected(selectedEditParts));

        antialiasCheckbox = new Button(composite, SWT.CHECK | SWT.LEFT);
        antialiasCheckbox.setText("&Antialias");
        antialiasCheckbox.setToolTipText("When checked, use antialias when creating the output.");
        antialiasCheckbox.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e)
            {
                update();
            }
        });

        update();

        return composite;
    }

    public List<GraphicalEditPart> getEditPartsToExport()
    {
        if (isShowSelectionOnly())
            return selectedEditParts;
        else
            return Collections.singletonList((GraphicalEditPart) selectedEditParts.get(0).getRoot().getContents());
    }

    public void update()
    {
        previewImage = generatePreviewImage(maxImageSize);
        previewCanvas.redraw();
    }

    public void paintControl(PaintEvent e) {
        Dimension imageSize = new Rectangle(previewImage.getBounds()).getSize();
        Dimension canvasSize = new Dimension(previewCanvas.getSize());
        Dimension size;
        if (canvasSize.contains(imageSize))
            size = imageSize;
        else
            size = createLargestFittingSize(imageSize, canvasSize);
        int x = (canvasSize.width - size.width) / 2;
        int y = (canvasSize.height - size.height) / 2;
        e.gc.drawImage(previewImage, 0, 0, previewImage.getBounds().width, previewImage.getBounds().height, x, y, size.width, size.height);
    }

    protected Dimension createLargestFittingSize(Dimension sourceSize, Dimension targetSize)
    {
        double scale = Math.min((1.0d * targetSize.width)/sourceSize.width, (1.0d * targetSize.height)/sourceSize.height);
        Dimension size = sourceSize.getScaled(scale);
        return size;
    }

    private Image generatePreviewImage(Dimension maxSize)
    {
        List<IFigure> previewedFigures = new ArrayList<IFigure>();
        Rectangle selectionBounds= null;

        if (!isShowSelectionOnly()) {
            GraphicalEditPart diagramEditPart = selectedEditParts.get(0);
            IFigure rootFigure = NedFigureProvider.getExportFigure((GraphicalViewer) diagramEditPart.getRoot().getViewer());
            previewedFigures.add(rootFigure);
            selectionBounds = FigureUtils.getBounds(rootFigure);
        }
        else {
            for (GraphicalEditPart editPart : selectedEditParts)
                previewedFigures.add(editPart.getFigure());
            selectionBounds = FigureUtils.getBounds(previewedFigures);
        }

        Dimension imageSize = selectionBounds.getSize();
        if (selectionBounds.width > maxSize.width || selectionBounds.height > maxSize.height) {
            imageSize = maxSize;
            // Downscaling turned off due to Cairo crashes. That means the preview image does not show the whole image that will be exported
        }

        // FIXME: there is an error further up that allows the wizard to be called with an empty selection
        Image image = new Image(Display.getDefault(), imageSize.width, imageSize.height);

        for (IFigure figure: previewedFigures) {
            GC gc = new GC(image);
            gc.setAntialias(isAntialias() ? SWT.ON : SWT.OFF);
            gc.setTextAntialias(isAntialias() ? SWT.ON : SWT.OFF);
            Graphics graphics = new SWTGraphics(gc);
            graphics.translate(selectionBounds.getLocation().negate());
            // FIXME: graphics.scale(scale); Cairo crashes
            figure.paint(graphics);
            graphics.dispose();
            gc.dispose ();
        }

        return image;
    }
}