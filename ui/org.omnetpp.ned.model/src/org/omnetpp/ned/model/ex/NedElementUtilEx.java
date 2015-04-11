/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.editor.text.Keywords;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedElementConstants;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.IHasProperties;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.CompoundModuleElement;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.ImportElement;
import org.omnetpp.ned.model.pojo.LiteralElement;
import org.omnetpp.ned.model.pojo.NedElementTags;
import org.omnetpp.ned.model.pojo.NedFileElement;
import org.omnetpp.ned.model.pojo.PackageElement;
import org.omnetpp.ned.model.pojo.ParametersElement;
import org.omnetpp.ned.model.pojo.PropertyElement;
import org.omnetpp.ned.model.pojo.PropertyKeyElement;

/**
 * A collection of utility functions for manipulating NedElement trees
 *
 * @author rhornig, andras
 */
public class NedElementUtilEx implements NedElementTags, NedElementConstants {
    public static final String DISPLAY_PROPERTY = "display";

    //XXX move to NedElement
    public static boolean isDisplayStringUpToDate(DisplayString displayStringObject, IHasDisplayString owner) {
        // Note: cannot call owner.getDisplayString() here, as it also wants to update the fallback, etc.
        String displayStringLiteral = StringUtils.nullToEmpty(getDisplayStringLiteral(owner));
        String cachedDisplayString = displayStringObject == null ? "" : displayStringObject.toString();
        return displayStringLiteral.equals(cachedDisplayString);
    }

    /**
     * Returns the display string of the given element (submodule, connection or
     * toplevel type) in an unparsed form, from the NED tree.
     *
     * Returns null if the NED tree doesn't contain a display string.
     */
    public static String getDisplayStringLiteral(IHasDisplayString node) {
        // look for the "display" property inside the parameters section (if it exists)
        INedElement parametersNode = node.getFirstChildWithTag(NED_PARAMETERS);
        if (parametersNode != null)
            for (INedElement currElement : parametersNode)
                if (currElement instanceof PropertyElement) {
                    PropertyElement currProp = (PropertyElement)currElement;
                    if (DISPLAY_PROPERTY.equals(currProp.getName()))
                        return getPropertyValue(currProp);
                }
        return null;
    }

    /**
     * Sets the display string of a given node in the NED tree,
     * by creating or updating the LiteralElement that contains the
     * "@display" property in the tree. If the value to be set is empty,
     * the method removes the display string.
     *
     * Returns the LiteralElement which was created/updated, or null if the
     * display string was removed.
     */
    public static LiteralElement setDisplayStringLiteral(IHasDisplayString node, String displayString) {
        boolean remove = displayString.equals("");

        // look for the "parameters" block
        INedElement paramsNode = node.getFirstChildWithTag(NED_PARAMETERS);
        if (paramsNode == null) {
            if (remove)
                return null;
            paramsNode = NedElementFactoryEx.getInstance().createElement(NED_PARAMETERS);
            ((ParametersElement)paramsNode).setIsImplicit(true);
            node.appendChild(paramsNode);
        }

        // look for the first property parameter named "display"
        INedElement displayPropertyNode = paramsNode.getFirstChildWithAttribute(NED_PROPERTY, PropertyElement.ATT_NAME, DISPLAY_PROPERTY);
        if (displayPropertyNode == null) {
            if (remove)
                return null;
            displayPropertyNode = NedElementFactoryEx.getInstance().createElement(NED_PROPERTY);
            ((PropertyElement)displayPropertyNode).setName(DISPLAY_PROPERTY);
            paramsNode.appendChild(displayPropertyNode);
        }

        // if displayString was set to "" (empty), we want to delete the whole display property node
        if (remove) {
            paramsNode.removeChild(displayPropertyNode);
            if (paramsNode.getFirstChild() == null)
                node.removeChild(paramsNode); // remove empty "parameters:" section
            return null;
        }

        // look for the property key
        INedElement propertyKeyNode = displayPropertyNode.getFirstChildWithAttribute(NED_PROPERTY_KEY, PropertyKeyElement.ATT_NAME, "");
        if (propertyKeyNode == null) {
            propertyKeyNode = NedElementFactoryEx.getInstance().createElement(NED_PROPERTY_KEY);
            displayPropertyNode.appendChild(propertyKeyNode);
        }

        // look up or create the LiteralElement
        LiteralElement literalElement = (LiteralElement)propertyKeyNode.getFirstChildWithTag(NED_LITERAL);
        if (literalElement == null)
            literalElement = (LiteralElement)NedElementFactoryEx.getInstance().createElement(NED_LITERAL);

        // fill in the LiteralElement. if()'s are there to minimize the number of notifications
        if (literalElement.getType() != NED_CONST_STRING)
            literalElement.setType(NED_CONST_STRING);
        if (!displayString.equals(literalElement.getValue()))
            literalElement.setValue(displayString);
        // invalidate the text representation, so that next time the code will
        // be generated from VALUE, not from the text attribute
        if (literalElement.getText() != null)
            literalElement.setText(null);
        // if new, add it only here (also to minimize notifications)
        if (literalElement.getParent() == null)
            propertyKeyNode.appendChild(literalElement);
        return literalElement;
    }

    /**
     * Sets the value of the given boolean property on a node. If value is true adds it to the tree otherwise removes it.
     * @param value
     */
    public static void setBooleanProperty(IHasParameters node, String propertyName, boolean value) {
        // look for the "parameters" block
        INedElement paramsNode = node.getFirstChildWithTag(NED_PARAMETERS);

        if (paramsNode == null) {
            if (value == false) return; // optimize away property node for default value

            paramsNode = NedElementFactoryEx.getInstance().createElement(NED_PARAMETERS);
            ((ParametersElement)paramsNode).setIsImplicit(true);
            node.appendChild(paramsNode);
        }

        // look for the first property parameter named "display"
        INedElement isBooleanPropertyNode = paramsNode.getFirstChildWithAttribute(NED_PROPERTY, PropertyElement.ATT_NAME, propertyName);

        // first we always remove the original node (so all its children will be discarded)
        if (isBooleanPropertyNode !=null)
            paramsNode.removeChild(isBooleanPropertyNode);
        // create and add a new node if needed
        if (value) {
            isBooleanPropertyNode = NedElementFactoryEx.getInstance().createElement(NED_PROPERTY);
            ((PropertyElement)isBooleanPropertyNode).setIsImplicit(node instanceof CompoundModuleElement);
            ((PropertyElement)isBooleanPropertyNode).setName(propertyName);
            paramsNode.appendChild(isBooleanPropertyNode);
        }
    }

    /**
     * Returns the name of the first "extends" node (or null if none present)
     */
    public static String getFirstExtends(INedElement node) {
        ExtendsElement extendsElement = (ExtendsElement)node.getFirstChildWithTag(NED_EXTENDS);
        return extendsElement == null ? null : extendsElement.getName();
    }

    /**
     * Sets the name of object that is extended by node. if we set it to null or "" the node will be removed
     * @param node The node which extends the provided type
     * @param ext The name of the type that is extended
     */
    public static void setFirstExtends(INedElement node, String ext) {
        ExtendsElement extendsElement = (ExtendsElement)node.getFirstChildWithTag(NED_EXTENDS);
            if (extendsElement == null && ext != null && !"".equals(ext)) {
                extendsElement = (ExtendsElement)NedElementFactoryEx.getInstance().createElement(NED_EXTENDS);
                node.appendChild(extendsElement);
            }
            else if (extendsElement != null && (ext == null || "".equals(ext))) {
                // first set the name to "" so we will generate an attribute change event
                extendsElement.setName("");
                // remove the node if we would set the name to null or empty string
                node.removeChild(extendsElement);
            }
            if (extendsElement != null)
                extendsElement.setName(ext);
    }

    /**
     * Returns the value of the given property (assigned to the first key (default value))
     */
    public static String getPropertyValue(PropertyElement prop) {
        PropertyKeyElement pkn = prop.getFirstPropertyKeyChild();
        if (pkn == null ) return null;
        LiteralElement ln = pkn.getFirstLiteralChild();
        if (ln == null ) return null;

        return ln.getValue();
    }

    /**
     * Returns the name of component but stripped any digits from the right ie: "name123" would be "name"
     */
    private static String getNameBase(String name) {
        int i = name.length()-1;
        while (i>=0 && Character.isDigit(name.charAt(i))) --i;
        // strip away the digits at the end
        return name.substring(0,i+1);
    }

    public static String getFullName(IHasName element) {
        String name = element.getName();
        if (element instanceof SubmoduleElementEx) {
            SubmoduleElementEx submoduleElement = (SubmoduleElementEx)element;
            String vectorSize = submoduleElement.getVectorSize();
            if (StringUtils.isNotEmpty(vectorSize))
                return name + "[" + vectorSize + "]";
            else
                return name;
        }
        else
            return name;
    }

    public static void setFullName(IHasName element, String name) {
        if (element instanceof SubmoduleElementEx) {
            SubmoduleElementEx submoduleElement = (SubmoduleElementEx)element;
            int openIndex = name.indexOf("[");
            int closeIndex = name.indexOf("]");
            if (openIndex != -1 && closeIndex > openIndex) {
                element.setName(name.substring(0, openIndex));
                submoduleElement.setVectorSize(name.substring(openIndex + 1, closeIndex));
            }
            else {
                element.setName(name);
                submoduleElement.setVectorSize(null);
            }
        }
        else
            element.setName(name);
    }

    /**
     * Calculates a unique name for the provided model element.
     */
    public static String getUniqueNameFor(String currentName, Set<String> contextCollection) {
        // if there is no name in the context with the same name we don't have to change the name
        if (!contextCollection.contains(currentName))
            return currentName;

        // there is an other module with the same name, so find a new name
        String baseName = getNameBase(currentName);
        int i = 1;
        while(contextCollection.contains(new String(baseName+i)))
            i++;

        // we've found a unique name
        return baseName+i;
    }

    /**
     * Calculates a unique name for the provided model element
     * @param namedElement
     * @param contextCollection A collection of strings which provides a context in which the name should be unique
     * @return The new unique name, or the original name if it was unique
     */
    public static String getUniqueNameFor(IHasName namedElement, Collection<? extends IHasName> contextCollection) {
        Set<String> nameSet = new HashSet<String>(contextCollection.size());
        for (IHasName sm : contextCollection)
            if (sm != namedElement)
                nameSet.add(sm.getName());
        return getUniqueNameFor(namedElement.getName(), nameSet);
    }

    /**
     * Selects a name for a type, ensuring that the name will be unique in the given context.
     * (i.e. in a package for top level types or inside a top-level type for inner types)
     * The context must be a {@link NedFileElementEx} for top level elements or
     * {@link CompoundModuleElementEx} for inner types
     */
    public static String getUniqueNameForType(String currentName, INedTypeLookupContext context) {
        Set<String> reservedNames;
        String currentQName;
        if (context instanceof NedFileElementEx) {
            // top level type (work with fully qualified names)
            INedTypeResolver res = context.getResolver();
            IProject project = res.getNedFile((NedFileElementEx)context).getProject();
            reservedNames = res.getReservedQNames(project);
            currentQName = context.getQNameAsPrefix() + currentName;
        } else if (context instanceof CompoundModuleElementEx) {
            // inner type (we can work with unqualified names)
            CompoundModuleElementEx containerElement = (CompoundModuleElementEx)context;
            reservedNames = containerElement.getNedTypeInfo().getInnerTypes().keySet();
            currentQName = currentName;
        } else {
            Assert.isTrue(false, "Unique name ganeration is implemented only for NedFile and CompoundModule contexts.");
            return currentName;
        }

        String uniqueQName = getUniqueNameFor(currentQName, reservedNames);
        return uniqueQName.contains(".") ? StringUtils.substringAfterLast(uniqueQName, ".") : uniqueQName;
    }

    /**
     * Checks whether the provided string is a valid identifier, i.e. it does not contain
     * illegal characters and it not a reserved keyword.
     */
    public static boolean isValidIdentifier(String str) {
        if (str == null)
            return false;
        if (!str.matches(Keywords.NED_IDENT_REGEX))
            return false;
        if (Arrays.asList(NedElementConstants.RESERVED_NED_KEYWORDS).contains(str))
            return false;
        return true;
    }

    /**
     * Checks whether the provided string is a "dotted" sequence of valid NED identifiers.
     */
    public static boolean isValidQualifiedIdentifier(String str) {
        if (str == null)
            return false;
        if (str.startsWith(".") || str.endsWith(".") || str.contains(".."))  // split can discard empty tokens
            return false;
        for (String name : str.split("\\."))
            if (!isValidIdentifier(name))
                return false;
        return true;
    }

    /**
     * Checks whether the provided string is valid NED identifier in the
     * "friendly" notation "SimpleName (pac.kage.name)"  or the formal notation "pac.kage.name.SimpleName"
     */
    public static boolean isValidFriendlyQualifiedIdentifier(String str) {
        str = str.replaceAll(" *\\( *", "("); // remove spaces
        str = str.replaceAll(" *\\) *", ")");
        String str2 = str.replaceAll("(.*)\\((.*)\\)", "$2.$1");
        return isValidQualifiedIdentifier(str2);
    }

    /**
     * When the user renames a submodule, we need to update the connections in the
     * same compound module (and its subclasses) accordingly, so that the model
     * will remain consistent. This method performs this change, for one compound
     * module.
     */
    public static void renameSubmoduleInConnections(CompoundModuleElementEx compoundModule, String oldSubmoduleName, String newSubmoduleName) {
        INedElement connectionsNode = compoundModule.getFirstConnectionsChild();
        if (connectionsNode != null)
            doRenameSubmoduleInConnections(connectionsNode, oldSubmoduleName, newSubmoduleName);
    }

    protected static void doRenameSubmoduleInConnections(INedElement parent, String oldName, String newName) {
        for (INedElement child : parent) {
            if (child instanceof ConnectionElementEx) {
                ConnectionElementEx conn = (ConnectionElementEx) child;
                if (conn.getSrcModule().equals(oldName))
                    conn.setSrcModule(newName);
                if (conn.getDestModule().equals(oldName))
                    conn.setDestModule(newName);
            }
            else if (child instanceof ConnectionGroupElement) {
                doRenameSubmoduleInConnections(child, oldName, newName);
            }
        }
    }

    /**
     * Determines what import is necessary so that simple name can be used instead of the fully qualified type name.
     * The result of this call is an optional "import" element that needs to be inserted into the file,
     * and the contents of the "modifiedName" string buffer.
     */
    public static ImportElement createImportFor(INedTypeLookupContext lookupContext, String fullyQualifiedTypeName, StringBuffer modifiedName) {
        Assert.isTrue(StringUtils.isNotBlank(fullyQualifiedTypeName));
        String simpleName = fullyQualifiedTypeName.indexOf('.')==-1 ? fullyQualifiedTypeName : StringUtils.substringAfterLast(fullyQualifiedTypeName, ".");

        // check what this simple name already means in this lookup context
        INedTypeInfo existingSimilarType = lookupContext.getResolver().lookupNedType(simpleName, lookupContext);
        if (existingSimilarType != null) {
            if (existingSimilarType.getFullyQualifiedName().equals(fullyQualifiedTypeName)) {
                // it already means the same type we want: just use short name
                modifiedName.append(simpleName);
                return null;
            }
            else {
                // something else with the same simple name is already visible, so leave fully qualified name
                modifiedName.append(fullyQualifiedTypeName);
                return null;
            }
        }
        else {
            if (lookupContext instanceof CompoundModuleElementEx && fullyQualifiedTypeName.equals(lookupContext.getQNameAsPrefix() + simpleName)) {
                // this is supposed to be a (currently nonexistent) inner type, but inner types cannot be imported anyway: just use the simple name
                modifiedName.append(simpleName);
                return null;
            }
            else {
                // add import
                ImportElement importElement = (ImportElement)NedElementFactoryEx.getInstance().createElement(NedElementTags.NED_IMPORT);
                importElement.setImportSpec(fullyQualifiedTypeName);
                modifiedName.append(simpleName);
                return importElement;
            }
        }
    }

    /**
     * Returns a good insertion point for a new "import" element in a NED file; see INedElement.insertBefore().
     */
    public static INedElement findInsertionPointForNewImport(NedFileElement nedFileElement, ImportElement importElement) {
        // insert just before other imports, so the trailing new lines (in the trailing comment
        // of the last import) will not appear between the import lines.
        ImportElement firstImport = nedFileElement.getFirstImportChild();
        ImportElement lastImport = firstImport;
        ImportElement currentImport = firstImport;
        while (currentImport != null && importElement != null && currentImport.getImportSpec().compareTo(importElement.getImportSpec()) < 0) {
            currentImport = currentImport.getNextImportSibling();
            if (currentImport != null)
                lastImport = currentImport;
        }
        if (currentImport != null)
            return currentImport;
        else if (lastImport != null)
            return lastImport.getNextSibling();

        // no existing imports; so skip comments and the package declaration at the top of file, and insert there
        for (INedElement child : nedFileElement)
            if (!(child instanceof CommentElement) && !(child instanceof PackageElement))
                return child;
        return null; // file only contains comments and package, so append import at the bottom
    }

    /**
     * Given a type which extends an other type and the base type name is a fully qualified name,
     * this method replaces it with the simple name plus an import in the NED file
     * if needed/possible.
     *
     * Returns the newly created ImportElement, or null if no import got added.
     * (I.e. it returns null as well if an existing import already covered this type.)
     *
     * The new import is inserted at the beginning rather than after
     * the last import so the trailing new lines (in the trailing comment
     * of the last import) will not appear between the import lines.
     */
    public static ImportElement addImportForExtends(INedTypeElement typeElement) {
        String fullyQualifiedTypeName = typeElement.getFirstExtends();
        if (StringUtils.isEmpty(fullyQualifiedTypeName))
            return null;

        StringBuffer modifiedName = new StringBuffer();
        ImportElement importElement = createImportFor(typeElement.getParentLookupContext(), fullyQualifiedTypeName, modifiedName);

        typeElement.setFirstExtends(modifiedName.toString());

        if (importElement != null)
            typeElement.getContainingNedFileElement().insertImport(importElement);

        return importElement;
    }

    /**
     * Given a submodule or connection whose type name is a fully qualified name,
     * this method replaces it with the simple name plus an import in the NED file
     * if needed/possible.
     *
     * Returns the newly created ImportElement, or null if no import got added.
     * (I.e. it returns null as well if an existing import already covered this type.)
     *
     * The new import is inserted at the beginning rather than after
     * the last import so the trailing new lines (in the trailing comment
     * of the last import) will not appear between the import lines.
     */
    public static ImportElement addImportFor(ISubmoduleOrConnection submoduleOrConnection) {
        String typeOrLikeType = getTypeOrLikeType(submoduleOrConnection);
        if (StringUtils.isEmpty(typeOrLikeType))
                return null;

        CompoundModuleElementEx compoundModule = (CompoundModuleElementEx) submoduleOrConnection.getEnclosingTypeElement();

        StringBuffer modifiedName = new StringBuffer();
        ImportElement importElement = createImportFor(submoduleOrConnection.getEnclosingLookupContext(), typeOrLikeType, modifiedName);

        setTypeOrLikeType(submoduleOrConnection, modifiedName.toString());

        if (importElement != null)
            compoundModule.getContainingNedFileElement().insertImport(importElement);

        return importElement;
    }

    /**
     * Sets whichever of "type" and "like-type" is already set on the element
     */
    public static void setTypeOrLikeType(ISubmoduleOrConnection submoduleOrConnection, String value) {
        if (StringUtils.isNotEmpty(submoduleOrConnection.getLikeType()))
            submoduleOrConnection.setLikeType(value);
        else
            submoduleOrConnection.setType(value);
    }

    /**
     * Returns whichever of "type" and "like-type" is already set on the element
     */
    public static String getTypeOrLikeType(ISubmoduleOrConnection submoduleOrConnection) {
        if (StringUtils.isNotEmpty(submoduleOrConnection.getLikeType()))
            return submoduleOrConnection.getLikeType();
        else
            return submoduleOrConnection.getType();
    }

    /**
     * Given an import that contains "*" or "**" as wildcard, this method returns
     * the corresponding regex that can be used to check whether a fully qualified
     * type name matches the import.
     */
    public static String importToRegex(String importSpec) {
        return importSpec.replace(".", "\\.").replace("**", ".*").replace("*", "[^.]*");
    }

    public interface INedElementVisitor {
        void visit(INedElement element);
    }

    public static void visitNedTree(INedElement element, INedElementVisitor visitor) {
        visitor.visit(element);
        for (INedElement child : element)
            visitNedTree(child, visitor);
    }

    public static CommentElement createCommentElement(String locId, String content) {
        CommentElement comment = (CommentElement)NedElementFactoryEx.getInstance().createElement(NedElementTags.NED_COMMENT);
        comment.setLocid(locId);
        comment.setContent(content);
        return comment;
    }


    public static void collectProperties(INedElement node, Map<String, Map<String, PropertyElementEx>> map) {
        INedElement propertyParent = node instanceof IHasParameters ? node.getFirstChildWithTag(NED_PARAMETERS) : node;

        if (propertyParent != null) {
            for (INedElement child : propertyParent) {
                if (child instanceof PropertyElementEx) {
                    PropertyElementEx property = (PropertyElementEx)child;
                    String name = property.getName();
                    Map<String, PropertyElementEx> propertyMap = map.get(name);
                    if (propertyMap == null) {
                        propertyMap = new HashMap<String, PropertyElementEx>();
                        map.put(name, propertyMap);
                    }
                    propertyMap.put(property.getIndex(), property);
                }
            }
        }
    }

    public static PropertyElementEx getProperty(IHasProperties element, String name, String index) {
        Map<String, PropertyElementEx> propertyMap = element.getProperties().get(name);
        if (propertyMap == null)
            return null;
        else
            return propertyMap.get(index == null ? PropertyElementEx.DEFAULT_PROPERTY_INDEX : index);
    }

    public static ArrayList<String> getPropertyValues(IHasProperties element, String name) {
        return getPropertyValues(element, name, null);
    }

    public static ArrayList<String> getPropertyValues(IHasProperties element, String name, String index) {
        PropertyElementEx propertyElement = getProperty(element, name, index);
        ArrayList<String> properties = new ArrayList<String>();

        if (propertyElement != null)
            for (PropertyKeyElement propertyKey = propertyElement.getFirstPropertyKeyChild(); propertyKey != null; propertyKey = propertyKey.getNextPropertyKeySibling())
                for (LiteralElement literal = propertyKey.getFirstLiteralChild(); literal != null; literal = literal.getNextLiteralSibling())
                    properties.add(literal.getValue());

        return properties;
    }

    public static void setPropertyValues(IHasProperties element, String propertyName, String propertyIndex, Collection<String> values) {
        PropertyElementEx propertyElement = NedElementUtilEx.getProperty(element, propertyName, propertyIndex);

        if (propertyElement != null)
            propertyElement.removeAllChildren();

        addPropertyValues(element, propertyName, propertyIndex, values);
    }

    public static void addPropertyValues(IHasProperties element, String propertyName, String propertyIndex, Collection<String> values) {
        NedElementFactoryEx factory = NedElementFactoryEx.getInstance();
        PropertyElementEx propertyElement = NedElementUtilEx.getProperty(element, propertyName, propertyIndex);

        if (propertyElement == null) {
            propertyElement = (PropertyElementEx)factory.createElement(NED_PROPERTY);
            propertyElement.setName(propertyName);
            propertyElement.setIndex(propertyIndex);
            ((INedElement)element).appendChild(propertyElement);
        }

        PropertyKeyElement propertyKey = (PropertyKeyElement)propertyElement.getFirstChildWithAttribute(NED_PROPERTY_KEY, "name", "");

        if (propertyKey == null) {
            propertyKey = (PropertyKeyElement)factory.createElement(NED_PROPERTY_KEY);
            propertyElement.appendChild(propertyKey);
        }

        for (String value : values) {
            LiteralElement literal = (LiteralElement)factory.createElement(NED_LITERAL);
            literal.setValue(value);
            literal.setText(value);
            literal.setType(NED_CONST_STRING);
            propertyKey.appendChild(literal);
        }
    }

    public static ArrayList<String> getLabels(IHasProperties element) {
        return getPropertyValues(element, "labels");
    }

    public static void setLabels(IHasProperties element, Collection<String> labels) {
        setPropertyValues(element, "labels", null, labels);
    }

    public static void addLabels(IHasProperties element, Collection<String> labels) {
        addPropertyValues(element, "labels", null, labels);
    }


    /**
     * Converts a fully qualified NED type name to a user-friendly form, "SimpleName (package.name)".
     */
    public static String qnameToFriendlyTypeName(String qname) {
        return qname==null ? null : qname.indexOf('.') == -1 ? qname : qname.replaceFirst("(.*)\\.(.*)", "$2 ($1)");
    }

    /**
     * Parses a user-friendly NED type name ("SimpleName (package.name)" syntax) into a
     * fully qualified name. No error is thrown: spaces and invalid characters are simply
     * removed from the string.
     *
     * Simple names and already fully qualified names are accepted
     * and returned verbatim (after trimming surrounding spaces).
     */
    public static String friendlyTypeNameToQName(String displayedName) {
        if (displayedName == null)
            return null;
        if (!displayedName.contains("("))
            return displayedName.trim();

        // swap the parts before and after the opening parenthesis
        String result = displayedName.replaceAll("(.*)\\((.*)", "$2.$1");

        // throw out invalid characters
        result = result.replaceAll("[^A-Za-z0-9_.]", "");
        return result;
    }


}
