package org.omnetpp.simulation.figures;

import java.util.Collection;

import org.eclipse.jface.resource.ImageDescriptor;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.ui.ObjectFieldsTreeContentProvider;
import org.omnetpp.simulation.ui.ObjectFieldsTreeLabelProvider;
import org.omnetpp.simulation.ui.ObjectFieldsViewer;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Mode;
import org.omnetpp.simulation.ui.ObjectFieldsViewer.Ordering;

/**
 * TODO
 *
 * @author Tomi, Andras
 */
public class ObjectFieldsViewerFigure extends TreeFigure {
    /**
     * Constructor.
     */
    public ObjectFieldsViewerFigure() {
        setBackgroundColor(ColorFactory.WHITE);
        setLabelProvider(new ObjectFieldsTreeLabelProvider());
        setContentProvider(new ObjectFieldsTreeContentProvider());
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

    public ObjectFieldsTreeContentProvider getContentProvider() {
        return (ObjectFieldsTreeContentProvider)super.getContentProvider();
    }

    public void refresh() {
        rebuild();  //Andras
        invalidate();
        repaint();
    }

//TODO
//    public ISelection getSelection() {
//        return treeViewer.getSelection();
//    }
//
//    public void addSelectionChangedListener(ISelectionChangedListener listener) {
//        treeViewer.addSelectionChangedListener(listener);
//    }
//
//    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
//        treeViewer.removeSelectionChangedListener(listener);
//    }

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
        return Mode.ESSENTIALS;
//        //TODO ESSENTIALS
//        boolean isSubclassedFromcPacket = (object instanceof cPacket) && !object.getClassName().equals("cPacket");
//        boolean isContainer = (object instanceof cModule) || (object instanceof cQueue) || (object instanceof cArray);
//        Mode initialMode = isSubclassedFromcPacket ? ObjectFieldsViewer.Mode.PACKET :
//            isContainer ? ObjectFieldsViewer.Mode.CHILDREN : ObjectFieldsViewer.Mode.GROUPED;
//        return initialMode;
    }

    public static ImageDescriptor getImageDescriptorFor(Mode mode) {
        switch (mode) {
            case PACKET: return ObjectFieldsViewer.IMG_MODE_PACKET;
            case ESSENTIALS: return ObjectFieldsViewer.IMG_MODE_ESSENTIALS;
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
            case ESSENTIALS: return "Essentials mode";
            case CHILDREN: return "Children mode";
            case GROUPED: return "Grouped mode";
            case INHERITANCE: return "Inheritance mode";
            case FLAT: return "Flat mode";
            default: return "";
        }
    }
}
