/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.export;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.editparts.LayerManager;
import org.eclipse.gef.ui.parts.ScrollingGraphicalViewer;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.editor.graph.parts.NedEditPart;
import org.omnetpp.ned.editor.graph.parts.NedEditPartFactory;
import org.omnetpp.ned.editor.graph.parts.NedFileEditPart;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

public class NedFigureProvider
{
    public List<IFigure> getDiagramFigures(List<GraphicalEditPart> editParts)
    {
        List<IFigure> figures = new ArrayList<IFigure>(editParts.size());
        // If only the root edit part is selected, the whole diagram is exported.
        if (isWholeDiagramSelected(editParts)) {
            GraphicalEditPart diagramEditPart = editParts.get(0);
            IFigure rootFigure = getExportFigure((GraphicalViewer) diagramEditPart.getRoot().getViewer());
            figures.add(rootFigure);
        }
         // The root edit part is not selected but any number of other edit parts.
        if (figures.isEmpty())
            for (GraphicalEditPart editPart : editParts)
                figures.add(editPart.getFigure());
        return figures;
    }

    public static boolean isWholeDiagramSelected(List<GraphicalEditPart> selectedEditParts)
    {
        if (selectedEditParts.size() == 1) {
            GraphicalEditPart editPart = selectedEditParts.get(0);
            if (editPart == editPart.getRoot().getViewer().getContents())
                return true;
        }
        return false;
    }

    public static IFigure getExportFigure(GraphicalViewer viewer)
    {
        LayerManager layers = (LayerManager) viewer.getEditPartRegistry().get(LayerManager.ID);
        IFigure diagramFigure = layers.getLayer(LayerConstants.PRINTABLE_LAYERS);
        return diagramFigure;
    }

    public static ScrollingGraphicalViewer createNedViewer(INedElement model) {
        ScrollingGraphicalViewer viewer = new ScrollingGraphicalViewer();
        viewer.setEditPartFactory(new NedEditPartFactory());
        viewer.setContents(model);
        NedFileEditPart nedFilePart = (NedFileEditPart)viewer.getEditPartRegistry().get(model);
        if (nedFilePart == null)
            throw new IllegalArgumentException("Invalid NED file.");
        // root figure is not added to the viewer because of off screen rendering
        // we have to pretend the addition otherwise add notification will not be sent to children
        IFigure rootFigure = nedFilePart.getFigure();
        rootFigure.addNotify();
        rootFigure.setBounds(new Rectangle(0, 0, Integer.MAX_VALUE, Integer.MAX_VALUE));
        rootFigure.setBorder(null);
        rootFigure.setFont(Display.getDefault().getSystemFont());
        rootFigure.validate();
        return viewer;
    }

    @SuppressWarnings("unchecked")
    public Map<IFigure, String> getDiagramFigures(final IFile diagramFile) {
        final Map<IFigure, String>[] results = new Map[1];

        DisplayUtils.runNowOrSyncInUIThread(new java.lang.Runnable() {
            public void run() {
                Map<IFigure, String> result = new HashMap<IFigure, String>();
                NedFileElementEx modelRoot = NedResourcesPlugin.getNedResources().getNedFileElement(diagramFile);
                ScrollingGraphicalViewer viewer = createNedViewer(modelRoot);

                // count the number of type. if only a single type
                // present and its name is the same as the filename, we will use only that name
                // otherwise filename_modulename is the syntax
                List<INedTypeElement> typeElements = modelRoot.getTopLevelTypeNodes();

                for (INedTypeElement typeElement : typeElements) {
                    NedEditPart editPart = (NedEditPart)viewer.getEditPartRegistry().get(typeElement);
                    String filebasename = StringUtils.chomp(diagramFile.getName(), "."+diagramFile.getFileExtension());
                    String imageName = getFigureName(typeElements, typeElement, filebasename);
                    result.put(editPart.getFigure(),  imageName);
                }

                results[0] = result;
            }
        });

        return results[0];
    }

    public static String getFigureName(List<INedTypeElement> typeElements, INedTypeElement typeElement, String fileBaseName) {
        return typeElements.size() == 1 && typeElement.getName().endsWith(fileBaseName) ?
               typeElement.getName() : fileBaseName + "_" + typeElement.getName();
    }
}
