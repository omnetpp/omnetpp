/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.IHasProperties;
import org.omnetpp.ned.model.interfaces.INedFileElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.pojo.ImportElement;
import org.omnetpp.ned.model.pojo.NedElementTags;
import org.omnetpp.ned.model.pojo.NedFileElement;
import org.omnetpp.ned.model.pojo.PackageElement;

/**
 * Represents a NED file
 *
 * @author rhornig
 */
public class NedFileElementEx extends NedFileElement implements IHasProperties, INedFileElement, INedTypeLookupContext {
	protected boolean readOnly;

    protected NedFileElementEx() {
	}

    protected NedFileElementEx(INedElement parent) {
		super(parent);
	}

    /**
     * Returns true if this NED file element has the readonly bit set.
     * Any change within the tree of a read-only NED file element will
     * cause an assertion error.
     */
    public boolean isReadOnly() {
    	return readOnly;
	}

    /**
     * Sets the readonly bit on this NED file element.
     */
    public void setReadOnly(boolean readOnly) {
		this.readOnly = readOnly;
		fireModelEvent(new NedModelReadOnlyEvent(this, readOnly));
	}

	/**
	 * Returns a list of the toplevel (non-inner) NED types in this file
	 */
    public List<INedTypeElement> getTopLevelTypeNodes() {
		List<INedTypeElement> result = new ArrayList<INedTypeElement>();
		for (INedElement currChild : this)
			if (currChild instanceof INedTypeElement)
				result.add((INedTypeElement)currChild);

		return result;
	}

	@Override
	public void fireModelEvent(NedModelEvent event) {
	    // note: the following assert is technically correct; it is commented out because
	    // hasSyntaxError() absolutely kills the performance while validating large models
		//XXX Assert.isTrue((!readOnly && !hasSyntaxError()) || !(event instanceof NedModelChangeEvent), "Attempted to modify the NED element tree while it is in read only mode");
		super.fireModelEvent(event);
	}

	public String getQNameAsPrefix() {
		return StringUtils.isEmpty(getPackage()) ? "" : getPackage() + ".";
	}

	/**
	 * Returns the package name (from the PackageElement child), or null
	 */
	public String getPackage() {
		PackageElement packageElement = getFirstPackageChild();
		return packageElement == null ? null : packageElement.getName();
	}

	/**
	 * Sets the package name; creates/removes PackageElement child as necessary.
	 */
	public void setPackage(String packageName) {
	    PackageElement packageElement = getFirstPackageChild();
	    if (StringUtils.isEmpty(packageName)) {
	        if (packageElement != null)
	            removeChild(packageElement);
	    }
	    else {
	        if (packageElement == null) {
	            packageElement = (PackageElement)NedElementFactoryEx.getInstance().createElement(NedElementTags.NED_PACKAGE);
	            insertChildBefore(getFirstChild(), packageElement);
	            packageElement.appendChild(NedElementUtilEx.createCommentElement("right", "\n\n"));
	        }
	        packageElement.setName(packageName);
	    }
	}

	/**
	 * Returns the imports in this file
	 */
	public List<String> getImports() {
		List<String> result = new ArrayList<String>();
		for (INedElement child : this)
			if (child instanceof ImportElement)
				result.add(((ImportElement)child).getImportSpec());
		return result;
	}

	/**
	 * Insert an import before the first import in the file.
	 * 
	 * The new import is inserted at the beginning rather than after 
	 * the last import so the trailing new lines (in the trailing comment 
	 * of the last import) will not appear between the import lines. 
	 */
   public ImportElement addImport(String importSpec) {
		ImportElement importElement = (ImportElement)NedElementFactoryEx.getInstance().createElement(NedElementTags.NED_IMPORT);
		importElement.setImportSpec(importSpec);

		INedElement firstImport = getFirstImportChild();
		INedElement insertionPoint = firstImport!=null ? firstImport :
			getFirstPackageChild()!=null ? getFirstPackageChild().getNextSibling() : getFirstChild();

		insertChildBefore(insertionPoint, importElement);
		return importElement;
	}

	/**
	 * Removes all ImportElements from this NED file.
	 */
	public void removeImports() {
		while (getFirstImportChild() != null)
			getFirstImportChild().removeFromParent();
	}

	/**
	 * Returns the first import child in this NED file, or null
	 */
	public ImportElement getFirstImportChild() {
		for (INedElement element : this)
			if (element instanceof ImportElement)
				return (ImportElement)element;
		return null;
	}

	/**
	 * Returns the last import child in this NED file, or null
	 */
	public ImportElement getLastImportChild() {
		ImportElement lastImport = null;
		for (INedElement element : this)
			if (element instanceof ImportElement)
				lastImport = (ImportElement)element;
		return lastImport;
	}

    public Map<String, Map<String, PropertyElementEx>> getProperties() {
        HashMap<String, Map<String, PropertyElementEx>> map = new HashMap<String, Map<String, PropertyElementEx>>();
		NedElementUtilEx.collectProperties(this, map);
		return map;
    }
}
