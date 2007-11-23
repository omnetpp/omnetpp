package org.omnetpp.ned.editor.graph.misc;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.ui.parts.ScrollingGraphicalViewer;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.edit.NedEditPart;
import org.omnetpp.ned.editor.graph.edit.NedEditPartFactory;
import org.omnetpp.ned.editor.graph.edit.NedFileEditPart;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

import de.unikassel.imageexport.providers.AbstractFigureProvider;

/**
 * Figure provider for image export plugins, returns all compound module figures
 * in the given file, along with their names.
 *
 * @author rhornig
 */
public class NedFigureProvider extends AbstractFigureProvider {

    @SuppressWarnings("unchecked")
    public Map<IFigure, String> provideExportImageFigures(final IFile diagramFile) {
        final Map<IFigure, String>[] results = new Map[1];

        DisplayUtils.runNowOrSyncInUIThread(new java.lang.Runnable() {
            public void run() {
                Map<IFigure, String> result = new HashMap<IFigure, String>();
                NedFileElementEx modelRoot = NEDResourcesPlugin.getNEDResources().getNedFileElement(diagramFile);
                ScrollingGraphicalViewer viewer = new ScrollingGraphicalViewer();
                viewer.setEditPartFactory(new NedEditPartFactory());
                viewer.setContents(modelRoot);
                NedFileEditPart nedFilePart = (NedFileEditPart)viewer.getEditPartRegistry().get(modelRoot);
                if (nedFilePart == null) 
                    throw new IllegalArgumentException("Invalid NED file.");
                // root figure is not added to the viewer because of off screen rendering
                // we have to pretend the addition otherwise add notification will not be sent to children
                nedFilePart.getFigure().addNotify();
                nedFilePart.getFigure().setBounds(new Rectangle(0, 0, Integer.MAX_VALUE, Integer.MAX_VALUE));
                nedFilePart.getFigure().setFont(Display.getDefault().getSystemFont());
                nedFilePart.getFigure().validate();
                
                // count the number of type. if only a single type
                // present and its name is the same as the filename, we will use only that name
                // otherwise filename_modulename is the syntax
                List<INedTypeElement> typeElements = modelRoot.getTopLevelTypeNodes();
        
                for (INedTypeElement typeElement : typeElements) {
                    NedEditPart editPart = (NedEditPart)viewer.getEditPartRegistry().get(typeElement);
                    String filebasename = StringUtils.chomp(diagramFile.getName(), "."+diagramFile.getFileExtension());
                    String imageName = typeElements.size() == 1 && typeElement.getName().endsWith(filebasename) ?
                            typeElement.getName() : filebasename+"_"+typeElement.getName(); 
                    result.put(editPart.getFigure(),  imageName);
                }
                
                results[0] = result;
            }
        });
        
        return results[0];
    }
}
