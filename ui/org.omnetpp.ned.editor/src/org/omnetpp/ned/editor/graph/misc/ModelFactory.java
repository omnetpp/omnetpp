package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.requests.CreationFactory;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IHasType;


/**
 * A factory used to create new model elements corresponding to graphical editor elements
 * Type and name must ba provided.
 *
 * @author rhornig
 */
public class ModelFactory implements CreationFactory {
    protected String objectType;
    protected String name;
	protected String type;

    /**
     * @param objectType The class identifier of the INEDElement
     */
    public ModelFactory(String objectType) {
        this(objectType, null, null);
    }
    /**
     * @param objectType The class identifier of the INEDElement
     * @param name The optional name of the new element
     */
    public ModelFactory(String objectType, String name) {
        this(objectType, name, null);
    }
	/**
	 * @param objectType The class identifier of the INEDElement
	 * @param name The optional name of the new element
	 * @param type The optional type of the new element (for submodules and connections)
	 */
	public ModelFactory(String objectType, String name, String type) {
        this.objectType = objectType;
        this.name = name;
		this.type = type;
	}

	public Object getNewObject() {
		INEDElement element = NEDElementFactoryEx.getInstance().createElement(objectType);
        if (element instanceof IHasName)
            ((IHasName)element).setName(name);

        if (element instanceof IHasType)
            ((IHasType)element).setType(type);

        return element;
	}

	public Object getObjectType() {
		return objectType;
	}

}

