package org.omnetpp.ned.editor.graph.misc;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.ui.parts.ScrollingGraphicalViewer;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.NedEditPartFactory;
import org.omnetpp.ned.editor.graph.edit.NedFileEditPart;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;

import de.unikassel.imageexport.providers.AbstractFigureProvider;

/**
 * Figure provider for image export plugins, returns all compound module figures
 * in the given file, along with their names.
 *
 * @author rhornig
 */
public class NedFigureProvider extends AbstractFigureProvider {

    public Map<IFigure, String> provideExportImageFigures(IFile diagramFile) {
        Map<IFigure, String> result =new HashMap<IFigure, String>();
        INEDElement modelRoot = NEDResourcesPlugin.getNEDResources().getNedFileElement(diagramFile);
        ScrollingGraphicalViewer viewer = new ScrollingGraphicalViewer();
        viewer.setEditPartFactory(new NedEditPartFactory());
        viewer.setContents(modelRoot);
        NedFileEditPart nedFilePart = (NedFileEditPart)viewer.getEditPartRegistry().get(modelRoot);
        if (nedFilePart == null) 
            throw new IllegalArgumentException("Invalid NED file.");
        // root figure is not added to the viewer because of off screen rendering
        // we have to pretend the addition otherwise add notification will not be sent to children
        nedFilePart.getFigure().addNotify();
        nedFilePart.getFigure().setBounds(new Rectangle(0,0,Integer.MAX_VALUE, Integer.MAX_VALUE));
        nedFilePart.getFigure().setFont(Display.getDefault().getSystemFont());
        nedFilePart.getFigure().validate();
        
        // count the number of compound modules. if only a single compound module
        // present ant its name is the same as the filename, we will use only that name
        // otherwise filename_modulename is the syntax
        int numberOfCompoundModules = 0;
        for (INEDElement c : modelRoot) {
            if (c instanceof CompoundModuleElementEx) 
                numberOfCompoundModules++;
        }
        
        for (INEDElement child : modelRoot) {
            if (child instanceof CompoundModuleElementEx) {
                CompoundModuleElementEx cmodule = (CompoundModuleElementEx)child;
                CompoundModuleEditPart cmep = (CompoundModuleEditPart)viewer.getEditPartRegistry().get(cmodule);
                String filebasename = StringUtils.chomp(diagramFile.getName(), "."+diagramFile.getFileExtension());
                String imageName = numberOfCompoundModules == 1 && cmodule.getName().endsWith(filebasename) ?
                                        cmodule.getName() : filebasename+"_"+cmodule.getName(); 
                result.put(cmep.getFigure(),  imageName);
            }
        }
        return result;
    }

}
