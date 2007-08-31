package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.notification.NEDModelChangeEvent;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.ImportElement;
import org.omnetpp.ned.model.pojo.NedFileElement;

/**
 * Represents a NED file
 *
 * @author rhornig
 */
public class NedFileElementEx extends NedFileElement implements INedTypeLookupContext {
	protected boolean readOnly;

    protected NedFileElementEx() {
	}

    protected NedFileElementEx(INEDElement parent) {
		super(parent);
	}
    
    public boolean isReadOnly() {
    	return readOnly;
	}
    
    public void setReadOnly(boolean readOnly) {
		this.readOnly = readOnly;
		fireModelEvent(new NEDModelReadOnlyEvent(this, readOnly));
	}
    
    public boolean hasSyntaxError() {
    	return getSyntaxProblemMaxCumulatedSeverity() == IMarker.SEVERITY_ERROR;
	}

	/**
	 * Returns a list of the toplevel (non-inner) NED types in this file
	 */
    public List<INedTypeElement> getTopLevelTypeNodes() {
		List<INedTypeElement> result = new ArrayList<INedTypeElement>();
		for (INEDElement currChild : this)
			if (currChild instanceof INedTypeElement)
				result.add((INedTypeElement)currChild);

		return result;
	}
	
	@Override
	public void fireModelEvent(NEDModelEvent event) {
		Assert.isTrue((!readOnly && !hasSyntaxError()) || !(event instanceof NEDModelChangeEvent), "Attempted to modify the NED element tree while it is in read only mode");
		super.fireModelEvent(event);
	}

	public String getQNameAsPrefix() {
		return StringUtils.isEmpty(getPackage()) ? "" : getPackage() + ".";
	}
	
	/**
	 * Returns the imports in this file
	 */
	public List<String> getImports() {
		List<String> result = new ArrayList<String>();
		for (INEDElement child : this)
			if (child instanceof ImportElement)
				result.add(((ImportElement)child).getImportSpec());
		return result;
	}
}
