package org.omnetpp.ned.model;

import java.nio.channels.Channel;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.model.pojo.ChannelSpecNode;
import org.omnetpp.ned.model.pojo.ConnectionsNode;
import org.omnetpp.ned.model.pojo.GatesNode;
import org.omnetpp.ned.model.pojo.NEDElementFactory;
import org.omnetpp.ned.model.pojo.NedFileNode;
import org.omnetpp.ned.model.pojo.ParametersNode;
import org.omnetpp.ned.model.pojo.SubmodulesNode;
import org.omnetpp.ned.model.pojo.TypesNode;
import org.omnetpp.ned.model.swig.NED1Generator;
import org.omnetpp.ned.model.swig.NED2Generator;
import org.omnetpp.ned.model.swig.NEDBasicValidator;
import org.omnetpp.ned.model.swig.NEDDTDValidator;
import org.omnetpp.ned.model.swig.NEDElement;
import org.omnetpp.ned.model.swig.NEDElementCode;
import org.omnetpp.ned.model.swig.NEDErrorCategory;
import org.omnetpp.ned.model.swig.NEDErrorStore;
import org.omnetpp.ned.model.swig.NEDParser;
import org.omnetpp.ned.model.swig.NEDSourceRegion;
import org.omnetpp.ned.model.swig.NEDTools;

/**
 * Utility functions working on NEDELelemt trees. Conversions, serialization, dumping of trees. 
 */
public class NEDTreeUtil {

	/**
	 * Generate NED code from the given NEDElement tree. The root node
	 * does not have to be NedFileNode, any subtree can be converted
	 * to source form.
	 * 
	 * @param keepSyntax if set, sources parsed in old syntax (NED-1) will be generated in old syntax as well 
	 */
	public static String generateNedSource(org.omnetpp.ned.model.NEDElement treeRoot, boolean keepSyntax) {
		// XXX for debugging
        System.out.println(generateXmlFromPojoElementTree(treeRoot,""));
        
        NEDErrorStore errors = new NEDErrorStore();
		errors.setPrintToStderr(true); //XXX just for debugging
		if (keepSyntax && treeRoot instanceof NedFileNode && "1".equals(((NedFileNode)treeRoot).getVersion())) {
			NED1Generator ng = new NED1Generator(errors);
			filterPojoTree(treeRoot);
            return ng.generate(pojo2swig(treeRoot), ""); // TODO check NEDErrorStore for conversion errors!! 
		}
		else {
			NED2Generator ng = new NED2Generator(errors);
            filterPojoTree(treeRoot);
			return ng.generate(pojo2swig(treeRoot), ""); // TODO check NEDErrorStore for errors!!
		}
	}

	/**
	 * Parse NED source and return it as a NEDElement tree. The parser implements recovery, and 
	 * a tree may be returned even if there were errors. Callers should check the 
	 * NEDErrorStore.
	 */
	public static org.omnetpp.ned.model.NEDElement parseNedSource(String source, NEDErrorStore errors, String fileName) {
        return parse(source, fileName, errors);
	}

	/**
	 * Load and parse NED file to a NEDElement tree. The parser implements recovery, and 
	 * a tree may be returned even if there were errors. Callers should check the 
	 * NEDErrorStore.
	 */
	public static org.omnetpp.ned.model.NEDElement loadNedSource(String filename, NEDErrorStore errors) {
        return parse(null, filename, errors);
	}

	/**
	 * Parse the given source or the given file. Try to return a non-null tree even in case 
	 * of parse errors. However, returned tree is always guaranteed to conform to the DTD. 
	 */
	private static org.omnetpp.ned.model.NEDElement parse(String source, String filename, NEDErrorStore errors) {
		Assert.isTrue(filename != null); 
		try {
			// parse
			NEDParser np = new NEDParser(errors);
			np.setParseExpressions(false);
			NEDElement swigTree = source!=null ? np.parseNEDText(source) : np.parseNEDFile(filename);
			if (swigTree == null)
				return null;
			// set the file name property in the nedFileElement
            if (NEDElementCode.swigToEnum(swigTree.getTagCode()) == NEDElementCode.NED_NED_FILE)
                swigTree.setAttribute("filename", filename);
            
			if (!errors.empty()) {
				// There were parse errors, and the tree built may not be entirely correct.
				// Typical problems are "mandatory attribute missing" esp with connections,
				// due to parse errors before filling in the connection element was completed.
				// Here we try to check and repair the tree by discarding elements that cause 
				// DTD validation error.
				NEDTools.repairNEDElementTree(swigTree);
			}

			// run DTD validation (once again)
			NEDDTDValidator dtdvalidator = new NEDDTDValidator(errors);
			int errs = errors.numMessages();
			dtdvalidator.validate(swigTree);
            // drop the tree if the validator added ANY new messages
            // FIXME we should check only for new error messages
            // currently validator do not add warning or info messageses
			if (errors.numMessages()!=errs) {
				// DTD validation produced additional errors -- give up
				swigTree.delete();
				return null;
			}
			NEDBasicValidator basicvalidator = new NEDBasicValidator(false, errors);
			basicvalidator.validate(swigTree);
            if (errors.numMessages()!=errs) {
                // BASIC validation produced additional errors -- give up
                swigTree.delete();
                return null;
            }

			// convert tree to pure Java objects
			org.omnetpp.ned.model.NEDElement pojoTree = swig2pojo(swigTree, null, errors);

			// XXX for debugging 
			// System.out.println(generateXmlFromPojoElementTree(pojoTree, "")); 

			swigTree.delete();
			return pojoTree;
		} 
		catch (RuntimeException e) {
			errors.add("", NEDErrorCategory.ERRCAT_ERROR.ordinal(), "internal error: "+e);
            NEDModelPlugin.log(e);
			return null;
		}
	}
	
	/**
	 * Converts a native C++ (SWIG-wrapped) NEDElement tree to a plain java tree.  
	 * WARNING there are two different NEDElement types hadled in this function. 
	 */
	public static org.omnetpp.ned.model.NEDElement swig2pojo(NEDElement swigNode, org.omnetpp.ned.model.NEDElement parent, NEDErrorStore errors) {
		org.omnetpp.ned.model.NEDElement pojoNode = null; 
		try {
			pojoNode = NEDElementFactory.getInstance().createNodeWithTag(swigNode.getTagCode(), parent);

			// set the attributes
			for (int i = 0; i < swigNode.getNumAttributes(); ++i) {
				pojoNode.setAttribute(i, swigNode.getAttribute(i));
			}

			// copy source location info
			pojoNode.setSourceLocation(swigNode.getSourceLocation());
			NEDSourceRegion swigRegion = swigNode.getSourceRegion();
			if (swigRegion.getStartLine()!=0) {
				org.omnetpp.ned.model.NEDSourceRegion pojoRegion = new org.omnetpp.ned.model.NEDSourceRegion();
				pojoRegion.startLine = swigRegion.getStartLine();
				pojoRegion.startColumn = swigRegion.getStartColumn();
				pojoRegion.endLine = swigRegion.getEndLine();
				pojoRegion.endColumn = swigRegion.getEndColumn();
				pojoNode.setSourceRegion(pojoRegion);
			} 

			// create child nodes
			for (NEDElement child = swigNode.getFirstChild(); child != null; child = child.getNextSibling()) {
				swig2pojo(child, pojoNode, errors);
			}

			return pojoNode;
		} 
		catch (NEDElementException e) {
			// prepare for errors during tree building, most notably 
			// "Nonexistent submodule" thrown from ConnectionNodeEx.
			errors.add(swigNode, e.getMessage()); // error message
			if (pojoNode!=null) {
				// throw out element that caused the error.
				parent.removeChild(pojoNode);
			}
			return null;
		}
		
	}	

	/**
	 * Converts a plain java NEDElement tree to a native C++ (SWIG-wrapped) tree.  
	 * WARNING there are two differenet NEDElement types hadled in this function. 
	 */
	public static NEDElement pojo2swig(org.omnetpp.ned.model.NEDElement pojoNode) {

		NEDElement swigNode = org.omnetpp.ned.model.swig.NEDElementFactory.getInstance()
				.createNodeWithTag(pojoNode.getTagCode());

		// set the attributes
		for (int i = 0; i < pojoNode.getNumAttributes(); ++i) {
			String value = pojoNode.getAttribute(i);
			value = (value == null) ? "" : value;
			swigNode.setAttribute(i, value);
		}
		swigNode.setSourceLocation(pojoNode.getSourceLocation());

		// create child nodes
		for (org.omnetpp.ned.model.NEDElement child = pojoNode.getFirstChild(); 
					child != null; child = child.getNextSibling()) {
            NEDElement convertedChild = pojo2swig(child); 
			if (convertedChild != null) 
                swigNode.appendChild(convertedChild);
		}

		return swigNode;
	}
    
    /**
     * The function allows the normalization of the node before converted to SWIG objects.
     * Unnecessary nodes can be removed from the tree.
     * (ie. empty channelSpec objects etc.)
     * @param pojoNode Node to be filtered
     */
    protected static void filterPojoTree(org.omnetpp.ned.model.NEDElement pojoNode) {
        // filter the child nodes first
        for (org.omnetpp.ned.model.NEDElement child = pojoNode.getFirstChild(); 
                    child != null; child = child.getNextSibling()) {
            filterPojoTree(child);
        }

        // se if the current node can be filtered out

        // skip a channel spec if it does not contain any meaningful information
        if (pojoNode instanceof ChannelSpecNode) {
            ChannelSpecNode cpn = (ChannelSpecNode) pojoNode;
            if ((cpn.getType() == null || "".equals(cpn.getType())) 
                && (cpn.getLikeType() == null || "".equals(cpn.getLikeType())) 
                && !cpn.hasChildren()) {

                // remove it from the parent if it does not matter
                pojoNode.removeFromParent();
            }
        }
        // check for empty types, parameters, gates, submodules, connections node
        if (((pojoNode instanceof TypesNode) 
                || (pojoNode instanceof ParametersNode)
                || (pojoNode instanceof GatesNode)
                || (pojoNode instanceof SubmodulesNode)
                || (pojoNode instanceof ConnectionsNode))
                                && !pojoNode.hasChildren()) {
            pojoNode.removeFromParent();
        }
    }

	/**
	 * Converts a NEDElement tree to an XML-like textual format. Useful for debugging.
	 */
	public static String generateXmlFromSwigElementTree(NEDElement swigNode, String indent) {
		String result = indent;
		result += "<" + swigNode.getTagName();
		for (int i = 0; i < swigNode.getNumAttributes(); ++i)
			result += " " + swigNode.getAttributeName(i) + "=\""
					+ swigNode.getAttribute(i) + "\"";
		if (swigNode.getFirstChild() == null) {
			result += "/> \n";
		} else {
			result += "> \n";
			for (NEDElement child = swigNode.getFirstChild(); child != null; child = child
					.getNextSibling())
				result += generateXmlFromSwigElementTree(child, indent + "  ");

			result += indent + "</" + swigNode.getTagName() + ">\n";
		}
		return result;
	}

	public static String generateXmlFromPojoElementTree(org.omnetpp.ned.model.NEDElement pojoNode, String indent) {
		String result = indent;
		result += "<" + pojoNode.getTagName();
		for (int i = 0; i < pojoNode.getNumAttributes(); ++i)
			result += " " + pojoNode.getAttributeName(i) + "=\""
					+ pojoNode.getAttribute(i) + "\"";
        
        String debugString = pojoNode.debugString();
        if (!"".equals(debugString))
                debugString = "<!-- "+debugString + " -->";
        
		if (pojoNode.getFirstChild() == null) {
			result += "/> " +  debugString + "\n";
		} else {
			result += "> " +  debugString + "\n";
			for (org.omnetpp.ned.model.NEDElement child = pojoNode.getFirstChild(); child != null; child = child
					.getNextSibling())
				result += generateXmlFromPojoElementTree(child, indent + "  ");

			result += indent + "</" + pojoNode.getTagName() + ">\n";
		}
		return result;
	}
    
    /**
     * @param s1
     * @param s2
     * @return true if both param is null, or s1 equals s2
     */
    private static boolean nullsafeIsEqual(String s1, String s2) {
        if (s1 == s2)
            return true;
        if (s1 != null)
            return s1.equals(s2);
        return false;
    }
    
    /**
     * @param tree1
     * @param tree2
     * @return Whether the two trees are equal
     */
    public static boolean isNEDTreeEqual(org.omnetpp.ned.model.NEDElement tree1, org.omnetpp.ned.model.NEDElement tree2) {
        if (tree1.getTagCode() != tree2.getTagCode())
            return false;
        
        for (int i = 0; i < tree1.getNumAttributes(); ++i) {
            if ( !nullsafeIsEqual(tree1.getAttribute(i), tree2.getAttribute(i)))
                return false;
        }
        
        org.omnetpp.ned.model.NEDElement child1 = tree1.getFirstChild();
        org.omnetpp.ned.model.NEDElement child2 = tree2.getFirstChild();
        
        while (child1 != null && child2!=null) {
            // TODO comments node may be ignored here
            if (!isNEDTreeEqual(child1, child2))
                return false;
            child1 = child1.getNextSibling();
            child2 = child2.getNextSibling();
        }
        // both child list must be the same length
        if (child1 != null || child2 != null )
            return false;

        return true;
    }
}
