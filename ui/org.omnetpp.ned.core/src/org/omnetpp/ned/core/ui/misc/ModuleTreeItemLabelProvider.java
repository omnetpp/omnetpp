/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core.ui.misc;

import org.eclipse.core.resources.IProject;
import org.eclipse.jface.viewers.ITableColorProvider;
import org.eclipse.jface.viewers.ITableFontProvider;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.core.NedResources;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.pojo.SimpleModuleElement;

public class ModuleTreeItemLabelProvider extends LabelProvider implements ITableLabelProvider, ITableFontProvider, ITableColorProvider {
    private IProject project;

    public ModuleTreeItemLabelProvider(IProject project) {
        this.project = project;
    }

    @Override
    public String getText(Object element) {
        ModuleTreeItem module = (ModuleTreeItem)element;
        String text = module.getModuleName();
        text += " -";
        if (module.getNedTypeName() != null)
            text += " (" + module.getNedTypeName() + ")";
        text += " " + module.getModuleFullPath();
        if (module.getModuleId() != -1)
            text += " (id = " + module.getModuleId() + ")";
        return text;
    }

    @Override
    public Image getImage(Object element) {
        ModuleTreeItem module = (ModuleTreeItem)element;
        INedTypeInfo nedTypeInfo = NedResources.getInstance().getToplevelOrInnerNedType(module.getNedTypeName(), project);
        if (nedTypeInfo != null) {
            INedTypeElement nedTypeElement = nedTypeInfo.getNedElement();
            DisplayString dps = nedTypeElement.getDisplayString();
            Image image = ImageFactory.of(project).getIconImage(dps.getAsString(IDisplayString.Prop.IMAGE));
            if (image != null)
                return image;
            else {
                if (nedTypeElement  instanceof SimpleModuleElement)
                    return ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_SIMPLEMODULE);
                else if (nedTypeElement instanceof CompoundModuleElementEx)
                    return ImageFactory.global().getImage(ImageFactory.MODEL_IMAGE_COMPOUNDMODULE);
            }
        }
        return null;
    }

    @Override
    public Color getForeground(Object element, int columnIndex) {
        return null;
    }

    @Override
    public Color getBackground(Object element, int columnIndex) {
        return null;
    }

    @Override
    public Font getFont(Object element, int columnIndex) {
        return null;
    }

    @Override
    public Image getColumnImage(Object element, int columnIndex) {
        if (columnIndex == 0)
            return getImage(element);
        else
            return null;
    }

    @Override
    public String getColumnText(Object element, int columnIndex) {
        ModuleTreeItem module = (ModuleTreeItem)element;
        switch (columnIndex) {
            case 0:
                return module.getModuleName();
            case 1:
                return String.valueOf(module.getModuleId());
            case 2:
                return module.getNedTypeName();
            case 3:
                return module.getModuleFullPath();
            default:
                throw new RuntimeException("Unknown column index");
        }
    }
}