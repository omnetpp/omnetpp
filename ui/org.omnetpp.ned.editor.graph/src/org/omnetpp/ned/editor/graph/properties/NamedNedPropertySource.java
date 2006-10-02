package org.omnetpp.ned.editor.graph.properties;

import org.omnetpp.common.properties.Property;
import org.omnetpp.ned2.model.INamed;
import org.omnetpp.ned2.model.NEDElement;

/**
 * @author rhornig
 * A Property source for NED elements who has a Name attribute
 */
public class NamedNedPropertySource extends NedPropertySource {

	INamed model;
	
	public NamedNedPropertySource(INamed model) {
		super((NEDElement)model);
		this.model = model;
	}

	@Property(displayName="Name")
	public String getName() { return model.getName(); }
	public void setName(String name) { model.setName(name); }

}
