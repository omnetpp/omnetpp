/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ui;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.GateElement;
import org.omnetpp.ned.model.pojo.GatesElement;
import org.omnetpp.ned.model.pojo.NedFileElement;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.pojo.ParametersElement;
import org.omnetpp.ned.model.pojo.PropertyElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;
import org.omnetpp.ned.model.pojo.SubmodulesElement;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * A content provider that gives a basic overview structure for a ned model tree.
 *
 * @author rhornig
 */
public class NedModelContentProvider implements ITreeContentProvider {
    private int perGroupLimit = 0;

    public NedModelContentProvider() {
    }

    public NedModelContentProvider(int perGroupLimit) {
        this.perGroupLimit = perGroupLimit;
    }

    public void inputChanged(Viewer v, Object oldInput, Object newInput) {
    }

    public void dispose() {
    }

    public Object[] getElements(Object parent) {
        return getChildren(parent);
    }

    public Object getParent(Object child) {
        return ((INedElement)child).getParent();
    }

    @SuppressWarnings({ "unchecked", "rawtypes" })
    public Object[] getChildren(Object parent) {
        List result = new ArrayList();
        INedElement currElem = ((INedElement)parent);

        // if this is a channel spec we will give back the parameters subnode's children
        if (currElem == null)
            return result.toArray();

        for (INedElement child : currElem) {
            // display only the following classes
            if ((child instanceof NedFileElement) ||
                    (child instanceof PropertyElement && parent instanceof ParametersElement && !((PropertyElement)child).getIsImplicit()) ||
                    (child instanceof INedTypeElement) ||
                    (child instanceof SubmoduleElement) ||
                    (child instanceof GateElement) ||
                    (child instanceof ParamElement) ||
                    (child instanceof ConnectionElement) ||
                    (child instanceof ConnectionGroupElement))
                result.add(child);
            // flatten the following nodes to the first level
            if ((child instanceof ParametersElement) ||
                    (child instanceof GatesElement) ||
                    (child instanceof ConnectionsElement) ||
                    (child instanceof SubmodulesElement) ||
                    (child instanceof TypesElement)) {
                Object children[] = getChildren(child);
                List<Object> childList = Arrays.asList(children);
                if (children.length > perGroupLimit && perGroupLimit != 0) {
                    result.addAll(childList.subList(0, perGroupLimit));
                    result.add("And "+(children.length - perGroupLimit)+" more...");
                }
                else
                    result.addAll(childList);
            }
        }
        return result.toArray();
    }

    public boolean hasChildren(Object parent) {
        return getChildren(parent).length > 0;
    }
}
