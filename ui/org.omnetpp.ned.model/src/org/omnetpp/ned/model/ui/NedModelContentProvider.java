package org.omnetpp.ned.model.ui;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.pojo.ChannelInterfaceElement;
import org.omnetpp.ned.model.pojo.ChannelElement;
import org.omnetpp.ned.model.pojo.ChannelSpecElement;
import org.omnetpp.ned.model.pojo.CompoundModuleElement;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.GateElement;
import org.omnetpp.ned.model.pojo.GatesElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.NedFileElement;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.pojo.ParametersElement;
import org.omnetpp.ned.model.pojo.PropertyElement;
import org.omnetpp.ned.model.pojo.SimpleModuleElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;
import org.omnetpp.ned.model.pojo.SubmodulesElement;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * A content provider that gives a basic overview structure for a ned model tree
 *
 * @author rhornig
 */
public class NedModelContentProvider implements ITreeContentProvider {

	public void inputChanged(Viewer v, Object oldInput, Object newInput) {
	}

	public void dispose() {
	}

	public Object[] getElements(Object parent) {
		return getChildren(parent);
	}

	public Object getParent(Object child) {
		return ((INEDElement)child).getParent();
	}

	// TODO maybe we should provide a more flatter layout (skipping the parameters, gates, suboldules, types, connections nodes)
	@SuppressWarnings("unchecked")
    public Object[] getChildren(Object parent) {
        List result = new ArrayList();
        INEDElement currElem = ((INEDElement)parent);

        // if this is a channel spec we will give back the parameters subnode's children
        if (currElem instanceof ChannelSpecElement)
            currElem = ((ChannelSpecElement)currElem).getFirstParametersChild();
        if (currElem == null)
            return result.toArray();

        for (INEDElement child : currElem) {
            // display only the following classes
            if ((child instanceof NedFileElement) ||
//                    (child instanceof ImportElement) ||
                    (child instanceof PropertyElement && parent instanceof ParametersElement) ||
                    (child instanceof ChannelElement) ||
                    (child instanceof ChannelInterfaceElement) ||
                    (child instanceof SimpleModuleElement) ||
                    (child instanceof CompoundModuleElement) ||
                    (child instanceof ModuleInterfaceElement) ||
                    (child instanceof SubmoduleElement) ||
                    (child instanceof GateElement) ||
                    (child instanceof ParamElement) ||
                    (child instanceof ConnectionElement) ||
                    (child instanceof ConnectionGroupElement) ||
                    (child instanceof ChannelSpecElement))
                result.add(child);
            // flatten the following nodes to the first level
            if ((child instanceof ParametersElement) ||
                    (child instanceof GatesElement) ||
                    (child instanceof ConnectionsElement) ||
                    (child instanceof SubmodulesElement) ||
                    (child instanceof TypesElement))
                result.addAll(Arrays.asList(getChildren(child)));
        }
        return result.toArray();
	}

	public boolean hasChildren(Object parent) {
        return getChildren(parent).length > 0;
	}
}
