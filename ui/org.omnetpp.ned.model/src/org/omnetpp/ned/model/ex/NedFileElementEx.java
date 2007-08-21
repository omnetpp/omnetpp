package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.NedFileElement;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class NedFileElementEx extends NedFileElement {
    protected NedFileElementEx() {
	}

    protected NedFileElementEx(INEDElement parent) {
		super(parent);
	}

	public List<INedTypeElement> getTopLevelTypeNodes() {
		List<INedTypeElement> result = new ArrayList<INedTypeElement>();
		for (INEDElement currChild : this)
			if (currChild instanceof INedTypeElement)
				result.add((INedTypeElement)currChild);

		return result;
	}
}
