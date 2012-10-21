package org.omnetpp.simulation.ui;

import java.util.Collection;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.DecoratingStyledCellLabelProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Tree;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.model.cArray;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPacket;
import org.omnetpp.simulation.model.cQueue;

/**
 * Based on TreeViewer, it can display fields of an object in various ways.
 * Can be used in Property View and in inspectors.
 *
 * @author Andras
 */
public class ObjectFieldsViewer {
    public enum Mode { PACKET, CHILDREN, GROUPED, INHERITANCE, FLAT };
    public enum Ordering { NATURAL, ALPHABETICAL };

    private TreeViewer treeViewer;

    // mode icons
    public static final ImageDescriptor IMG_MODE_PACKET = SimulationPlugin.getImageDescriptor("icons/etool16/treemode_packet.png");
    public static final ImageDescriptor IMG_MODE_CHILDREN = SimulationPlugin.getImageDescriptor("icons/etool16/treemode_children.png");
    public static final ImageDescriptor IMG_MODE_FLAT = SimulationPlugin.getImageDescriptor("icons/etool16/treemode_flat.png");
    public static final ImageDescriptor IMG_MODE_GROUPED = SimulationPlugin.getImageDescriptor("icons/etool16/treemode_grouped.png");
    public static final ImageDescriptor IMG_MODE_INHERITANCE = SimulationPlugin.getImageDescriptor("icons/etool16/treemode_inher.png");

    /**
     * Constructor.
     */
    public ObjectFieldsViewer(Composite parent, int style) {
        treeViewer = new TreeViewer(parent, style);
        treeViewer.setLabelProvider(new DecoratingStyledCellLabelProvider(new ObjectFieldsTreeLabelProvider(), null, null));
        treeViewer.setContentProvider(new ObjectFieldsTreeContentProvider());
    }

    public void setInput(Object object) {
        treeViewer.setInput(object);
        refresh();
    }

    public Object getInput() {
        return treeViewer.getInput();
    }

    public TreeViewer getTreeViewer() {
        return treeViewer;
    }

    public Tree getTree() {
        return getTreeViewer().getTree();
    }

    public Mode getMode() {
        return getContentProvider().getMode();
    }

    public void setMode(Mode mode) {
        getContentProvider().setMode(mode);
        refresh();
    }

    public Ordering getOrdering() {
        return getContentProvider().getOrdering();
    }

    public void setOrdering(Ordering ordering) {
        getContentProvider().setOrdering(ordering);
        refresh();
    }

    protected ObjectFieldsTreeContentProvider getContentProvider() {
        return (ObjectFieldsTreeContentProvider)getTreeViewer().getContentProvider();
    }

    public void refresh() {
        treeViewer.refresh();
    }

    public ISelection getSelection() {
        return treeViewer.getSelection();
    }

    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        treeViewer.addSelectionChangedListener(listener);
    }

    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        treeViewer.removeSelectionChangedListener(listener);
    }

    @SuppressWarnings("rawtypes")
    public static Mode getPreferredMode(Object input) {
        Object referenceObject;
        if (input.getClass().isArray() && ((Object[])input).length != 0)
            referenceObject = ((Object[])input)[0];
        else if (input instanceof Collection && !((Collection)input).isEmpty())
            referenceObject = ((Collection)input).toArray()[0];
        else
            referenceObject = input;

        if (!(referenceObject instanceof cObject))
            return Mode.GROUPED; // whichever, won't make any difference

        return getPreferredMode((cObject)referenceObject);
    }

    public static Mode getPreferredMode(cObject object) {
        boolean isSubclassedFromcPacket = (object instanceof cPacket) && !object.getClassName().equals("cPacket");
        boolean isContainer = (object instanceof cModule) || (object instanceof cQueue) || (object instanceof cArray);
        Mode initialMode = isSubclassedFromcPacket ? ObjectFieldsViewer.Mode.PACKET :
            isContainer ? ObjectFieldsViewer.Mode.CHILDREN : ObjectFieldsViewer.Mode.GROUPED;
        return initialMode;
    }

    public static ImageDescriptor getImageDescriptorFor(Mode mode) {
        switch (mode) {
            case PACKET: return ObjectFieldsViewer.IMG_MODE_PACKET;
            case CHILDREN: return ObjectFieldsViewer.IMG_MODE_CHILDREN;
            case GROUPED: return ObjectFieldsViewer.IMG_MODE_GROUPED;
            case INHERITANCE: return ObjectFieldsViewer.IMG_MODE_INHERITANCE;
            case FLAT: return ObjectFieldsViewer.IMG_MODE_FLAT;
            default: return null;
        }
    }

    public static String getActionTooltipFor(Mode mode) {
        switch (mode) {
            case PACKET: return "Packet mode";
            case CHILDREN: return "Children mode";
            case GROUPED: return "Grouped mode";
            case INHERITANCE: return "Inheritance mode";
            case FLAT: return "Flat mode";
            default: return "";
        }
    }
}
