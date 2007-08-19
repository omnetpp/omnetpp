package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.pojo.NedFileNode;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class NedFileNodeEx extends NedFileNode {
    protected NedFileNodeEx() {
	}

    protected NedFileNodeEx(INEDElement parent) {
		super(parent);
	}

	public List<INedTypeNode> getTopLevelElements() {
		List<INedTypeNode> result = new ArrayList<INedTypeNode>();
		for (INEDElement currChild : this)
			if (currChild instanceof INedTypeNode)
				result.add((INedTypeNode)currChild);

		return result;
	}
}
