package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.requests.CreationFactory;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IHasType;


/**
 * A factory used to create new model elements corresponding to graphical editor elements.
 *
 * @author rhornig
 */
public class ModelFactory implements CreationFactory {
    protected int tagCode;
    protected String name;
	protected String type;
	protected boolean useLike = false;

    /**
     * @param objectType The class identifier of the INEDElement passed NEDElementFactoryEx.getInstance().createElement
     */
    public ModelFactory(int tagCode) {
        this(tagCode, null);
    }
    /**
     * @param objectType The class identifier of the INEDElement
     * @param name The optional name of the new element
     */
    public ModelFactory(int tagCode, String name) {
        this(tagCode, name, null, false);
    }

    /**
     * @param objectType The class identifier of the INEDElement
     * @param name The optional name of the new element
     * @param type The optional type of the new element (for submodules and connections)
     */
    public ModelFactory(int tagCode, String name, String type) {
        this(tagCode, name, type, false);
    }

    /**
	 * @param objectType The class identifier of the INEDElement
	 * @param name The optional name of the new element
	 * @param type The optional type of the new element (for submodules and connections)
	 * @param useLike The provided type will be used as an interface type t
	 */
	public ModelFactory(int tagCode, String name, String type, boolean useLike) {
        this.tagCode = tagCode;
        this.name = name;
		this.type = type;
		this.useLike = useLike;
	}

	public Object getNewObject() {
		INEDElement element = NEDElementFactoryEx.getInstance().createElement(tagCode);
        if (element instanceof IHasName)
            ((IHasName)element).setName(name);

        if (element instanceof IHasType) {
            if (!useLike)
                ((IHasType)element).setType(type);
            else {
                ((IHasType)element).setLikeType(type);
                // TODO maybe we could ask the user to specify the param name in a dialog box
                ((IHasType)element).setLikeParam("paramName");
            }
        }

        return element;
	}

	public Object getObjectType() {
		return tagCode;
	}

}

