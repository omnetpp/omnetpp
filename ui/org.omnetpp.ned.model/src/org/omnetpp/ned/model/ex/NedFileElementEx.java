package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.INedFileElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.notification.NEDModelChangeEvent;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.ImportElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.NedFileElement;
import org.omnetpp.ned.model.pojo.PackageElement;

/**
 * Represents a NED file
 *
 * @author rhornig
 */
public class NedFileElementEx extends NedFileElement implements INedFileElement, INedTypeLookupContext {
	protected boolean readOnly;

    protected NedFileElementEx() {
	}

    protected NedFileElementEx(INEDElement parent) {
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
		fireModelEvent(new NEDModelReadOnlyEvent(this, readOnly));
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
	    // note: the following assert is technically correct; it is commented out because 
	    // hasSyntaxError() absolutely kills the performance while validating large models
		Assert.isTrue((!readOnly && !hasSyntaxError()) || !(event instanceof NEDModelChangeEvent), "Attempted to modify the NED element tree while it is in read only mode");
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
	            packageElement = (PackageElement)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_PACKAGE);
	            insertChildBefore(getFirstChild(), packageElement);
	            packageElement.appendChild(NEDElementUtilEx.createCommentElement("right", "\n\n"));
	        }
	        packageElement.setName(packageName);
	    }
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

	/**
	 * Insert an import after the last import in the file
	 */
   public ImportElement addImport(String importSpec) {
		ImportElement importElement = (ImportElement)NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_IMPORT);
		importElement.setImportSpec(importSpec);

		INEDElement lastImport = getLastImportChild(); 
		INEDElement insertionPoint = lastImport!=null ? lastImport.getNextSibling() : 
			getFirstPackageChild()!=null ? getFirstPackageChild().getNextSibling() :
				getFirstChild();
		
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
	 * Returns the last import child in this NED file, or null
	 */
	public ImportElement getLastImportChild() {
		ImportElement lastImport = null;
		for (INEDElement element : this)
			if (element instanceof ImportElement)
				lastImport = (ImportElement)element;
		return lastImport;
	}
}
