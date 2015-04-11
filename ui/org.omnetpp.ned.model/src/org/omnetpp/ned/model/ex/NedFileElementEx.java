/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.IHasProperties;
import org.omnetpp.ned.model.interfaces.INedFileElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
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
    private INedTypeResolver resolver;
    protected boolean readOnly;

    protected NedFileElementEx(INedTypeResolver resolver) {
        init(resolver);
    }

    protected NedFileElementEx(INedTypeResolver resolver, INedElement parent) {
        super(parent);
        init(resolver);
    }

    private void init(INedTypeResolver resolver) {
        Assert.isNotNull(resolver, "This NED element type needs a resolver");
        this.resolver = resolver;
    }

    public INedTypeResolver getResolver() {
        return resolver;
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
        // Assert.isTrue((!readOnly && !hasSyntaxError()) || !(event instanceof NedModelChangeEvent), "Attempted to modify the NED element tree while it is in read only mode");
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
                packageElement = (PackageElement)NedElementFactoryEx.getInstance().createElement(getResolver(), NedElementTags.NED_PACKAGE);
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
     * Insert an import into the file. May add newlines to the import as trailing comment.
     */
    public ImportElement addImport(String importSpec) {
        ImportElement importElement = (ImportElement)NedElementFactoryEx.getInstance().createElement(getResolver(), NedElementTags.NED_IMPORT);
        importElement.setImportSpec(importSpec);
        insertImport(importElement);
        return importElement;
    }

    /**
     * Inserts the given import into the file. May add newlines to the import as trailing comment.
     */
    public void insertImport(ImportElement importElement) {
        insertChildBefore(NedElementUtilEx.findInsertionPointForNewImport(this, importElement), importElement);

        // if this is the last import element, add some additional new lines as trailing comment
        if (importElement.getNextImportSibling() == null) {
            importElement.appendChild(NedElementUtilEx.createCommentElement("right", "\n\n\n"));
            INedElement prevSibling = importElement.getPrevSibling();
            // remove comments from the previous import element
            if (prevSibling instanceof ImportElement)
                prevSibling.removeAllChildren();
        }
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

    @Override
    public String toString() {
        return super.toString() + getFilename();
    }

}
