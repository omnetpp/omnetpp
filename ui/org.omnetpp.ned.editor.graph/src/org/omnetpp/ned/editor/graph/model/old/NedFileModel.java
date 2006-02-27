package org.omnetpp.ned.editor.graph.model.old;

import org.eclipse.ui.views.properties.ComboBoxPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;

public class NedFileModelX extends ContainerModel {
    public static String PROP_ROUTER = "router"; //$NON-NLS-1$
    public static Integer ROUTER_MANUAL = new Integer(0);
    public static Integer ROUTER_MANHATTAN = new Integer(1);
    public static Integer ROUTER_SHORTEST_PATH = new Integer(2);


	protected Integer connectionRouter = null;
	private boolean snapToGeometry = false;
	private boolean gridEnabled = false;
	private double zoom = 1.0;

	public Integer getConnectionRouter() {
	    if (connectionRouter == null) connectionRouter = ROUTER_MANUAL;
	    return connectionRouter;
	}

	public double getZoom() {
	    return zoom;
	}

	/**
	 * Returns <code>null</code> for this model. Returns normal descriptors
	 * for all subclasses.
	 * 
	 * @return Array of property descriptors.
	 */
	public IPropertyDescriptor[] getPropertyDescriptors() {
	    if (getClass().equals(ContainerModel.class)) {
	        ComboBoxPropertyDescriptor cbd = new ComboBoxPropertyDescriptor(PROP_ROUTER,
	                "Connection router", new String[] {
	                        "Manual", "Manhattan", "Shortest path" });
	        cbd.setLabelProvider(new ConnectionRouterLabelProvider());
	        return new IPropertyDescriptor[] { cbd };
	    }
	    return super.getPropertyDescriptors();
	}

	public boolean isGridEnabled() {
	    return gridEnabled;
	}

	public boolean isSnapToGeometryEnabled() {
	    return snapToGeometry;
	}

	public void setConnectionRouter(Integer router) {
	    Integer oldConnectionRouter = connectionRouter;
	    connectionRouter = router;
	    firePropertyChange(PROP_ROUTER, oldConnectionRouter, connectionRouter);
	}

	public void setGridEnabled(boolean isEnabled) {
	    gridEnabled = isEnabled;
	}

	public void setSnapToGeometry(boolean isEnabled) {
	    snapToGeometry = isEnabled;
	}

	public void setZoom(double zoom) {
	    this.zoom = zoom;
	}

	public Object getPropertyValue(Object propName) {
	    if (propName.equals(PROP_ROUTER)) return connectionRouter;
	    return super.getPropertyValue(propName);
	}

	public void setPropertyValue(Object id, Object value) {
	    if (PROP_ROUTER.equals(id))
	        setConnectionRouter((Integer) value);
	    else
	        super.setPropertyValue(id, value);
	}

    class ConnectionRouterLabelProvider extends org.eclipse.jface.viewers.LabelProvider {

        public ConnectionRouterLabelProvider() {
            super();
        }

        public String getText(Object element) {
            if (element instanceof Integer) {
                Integer integer = (Integer) element;
                if (ROUTER_MANUAL.intValue() == integer.intValue())
                    return MessageFactory.PropertyDescriptor_LogicDiagram_Manual;
                if (ROUTER_MANHATTAN.intValue() == integer.intValue())
                    return MessageFactory.PropertyDescriptor_LogicDiagram_Manhattan;
                if (ROUTER_SHORTEST_PATH.intValue() == integer.intValue())
                    return MessageFactory.PropertyDescriptor_LogicDiagram_ShortestPath;
            }
            return super.getText(element);
        }
    }

}
