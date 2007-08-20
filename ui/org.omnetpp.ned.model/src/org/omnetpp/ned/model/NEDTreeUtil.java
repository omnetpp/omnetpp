package org.omnetpp.ned.model;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.omnetpp.ned.engine.NED1Generator;
import org.omnetpp.ned.engine.NED2Generator;
import org.omnetpp.ned.engine.NEDBasicValidator;
import org.omnetpp.ned.engine.NEDDTDValidator;
import org.omnetpp.ned.engine.NEDElement;
import org.omnetpp.ned.engine.NEDElementCode;
import org.omnetpp.ned.engine.NEDErrorStore;
import org.omnetpp.ned.engine.NEDParser;
import org.omnetpp.ned.engine.NEDSourceRegion;
import org.omnetpp.ned.engine.NEDTools;
import org.omnetpp.ned.model.pojo.ChannelSpecNode;
import org.omnetpp.ned.model.pojo.ConnectionsNode;
import org.omnetpp.ned.model.pojo.GatesNode;
import org.omnetpp.ned.model.pojo.NEDElementFactory;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.NedFileNode;
import org.omnetpp.ned.model.pojo.ParametersNode;
import org.omnetpp.ned.model.pojo.SubmodulesNode;
import org.omnetpp.ned.model.pojo.TypesNode;
import org.omnetpp.ned.model.ui.NedModelContentProvider;
import org.omnetpp.ned.model.ui.NedModelLabelProvider;

/**
 * Utility functions working on NEDElement trees. Conversions, serialization, dumping of trees.
 *
 * @author rhornig
 */
public class NEDTreeUtil {

	private static ITreeContentProvider nedModelContentProvider = new NedModelContentProvider();
    private static ILabelProvider nedModelLabelProvider = new NedModelLabelProvider();

    /**
	 * Generate NED code from the given NEDElement tree. The root node
	 * does not have to be NedFileNode, any subtree can be converted
	 * to source form.
	 *
	 * @param keepSyntax if set, sources parsed in old syntax (NED-1) will be generated in old syntax as well
	 */
    public static String generateNedSource(INEDElement treeRoot, boolean keepSyntax) {
		// XXX for debugging
        //System.out.println(generateXmlFromPojoElementTree(treeRoot,""));

        NEDErrorStore errors = new NEDErrorStore();
		errors.setPrintToStderr(false); //XXX just for debugging
		if (keepSyntax && treeRoot instanceof NedFileNode && "1".equals(((NedFileNode)treeRoot).getVersion())) {
			NED1Generator ng = new NED1Generator(errors);
            return ng.generate(pojo2swig(treeRoot), ""); // TODO check NEDErrorStore for conversion errors!!
		}
		else {
			NED2Generator ng = new NED2Generator(errors);
			return ng.generate(pojo2swig(treeRoot), ""); // TODO check NEDErrorStore for errors!!
		}
    }

	/**
	 * Parse the NED source and return it as a NEDElement tree. Returns a non-null, 
	 * DTD-conforming (but possibly incomplete) tree even in case of parse errors. 
	 * Callers should check NEDErrorStore to determine whether a parse error occurred.
	 * The passed fileName will only be used to fill in the NedFileNode element.
	 */
	public static INEDElement parseNedSource(String source, NEDErrorStore errors, String fileName) {
        return parse(source, fileName, errors);
	}

	/**
	 * Load and parse NED file to a NEDElement tree. Returns a non-null,
	 * DTD-conforming (but possibly incomplete) tree even in case of parse errors. 
 	 * Callers should check NEDErrorStore to determine whether a parse error occurred. 
	 */
	public static INEDElement loadNedSource(String filename, NEDErrorStore errors) {
        return parse(null, filename, errors);
	}

	/**
	 * Parse the given source (when source!=null) or the given file (when source==null).
	 * Never returns null. 
	 */
	private static INEDElement parse(String source, String filename, NEDErrorStore errors) {
		Assert.isTrue(filename != null);
		NEDElement swigTree = null;
		try {
			// parse
			NEDParser np = new NEDParser(errors);
			np.setParseExpressions(false);
			swigTree = source!=null ? np.parseNEDText(source) : np.parseNEDFile(filename);
			if (swigTree == null) {
				// return an empty NedFileNode if parsing totally failed
				NedFileNode fileNode = (NedFileNode)NEDElementFactory.getInstance().createNodeWithTag(NEDElementTags.NED_NED_FILE, null);
				fileNode.setFilename(filename);
				return fileNode;
			}

			// set the file name property in the nedFileElement
            if (NEDElementCode.swigToEnum(swigTree.getTagCode()) == NEDElementCode.NED_NED_FILE)
                swigTree.setAttribute("filename", filename);

			if (!errors.empty()) {
				// There were parse errors, and the tree built may not be entirely correct.
				// Typical problems are "mandatory attribute missing" especially with connections,
				// due to parse errors before filling in the connection element was completed.
				// Here we try to check and repair the tree by discarding elements that cause
				// DTD validation error.
				NEDTools.repairNEDElementTree(swigTree);
			}

			// run DTD validation (once again)
			int numMessages = errors.numMessages();
			NEDDTDValidator dtdvalidator = new NEDDTDValidator(errors);
			dtdvalidator.validate(swigTree);
			Assert.isTrue(errors.numMessages() == numMessages, "NED tree fails DTD validation, even after repairs");

			NEDBasicValidator basicvalidator = new NEDBasicValidator(false, errors);
			basicvalidator.validate(swigTree);
			Assert.isTrue(errors.numMessages() == numMessages, "NED tree fails basic validation, even after repairs");

			// convert tree to pure Java objects
			INEDElement pojoTree = swig2pojo(swigTree, null, errors);

			// System.out.println(generateXmlFromPojoElementTree(pojoTree, ""));

			return pojoTree;
		}
		finally {
			if (swigTree != null)
				swigTree.delete();
		}
	}

	/**
	 * Converts a native C++ (SWIG-wrapped) NEDElement tree to a plain java tree.
	 * NOTE: There are two different NEDElement types handled in this function.
	 */
	public static INEDElement swig2pojo(NEDElement swigNode, INEDElement parent, NEDErrorStore errors) {
		INEDElement pojoNode = null;
		try {
			pojoNode = NEDElementFactory.getInstance().createNodeWithTag(swigNode.getTagCode(), parent);

			// set the attributes
			for (int i = 0; i < swigNode.getNumAttributes(); ++i) {
				pojoNode.setAttribute(i, swigNode.getAttribute(i));
			}

			// copy source line number info
			pojoNode.setSourceLocation(swigNode.getSourceLocation());
			NEDSourceRegion swigRegion = swigNode.getSourceRegion();
			if (swigRegion.getStartLine() != 0)
				pojoNode.setSourceRegion(new org.omnetpp.ned.model.NEDSourceRegion(
						swigRegion.getStartLine(), swigRegion.getStartColumn(),
						swigRegion.getEndLine(), swigRegion.getEndColumn()));

			// create child nodes
			for (NEDElement child = swigNode.getFirstChild(); child != null; child = child.getNextSibling())
				swig2pojo(child, pojoNode, errors);

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
	 * NOTE: There are two different NEDElement types handled in this function.
	 */
	public static NEDElement pojo2swig(INEDElement pojoNode) {

		NEDElement swigNode = org.omnetpp.ned.engine.NEDElementFactory.getInstance()
				.createNodeWithTag(pojoNode.getTagCode());

		// set the attributes
		for (int i = 0; i < pojoNode.getNumAttributes(); ++i) {
			String value = pojoNode.getAttribute(i);
			value = value == null ? "" : value;
			swigNode.setAttribute(i, value);
		}
		swigNode.setSourceLocation(pojoNode.getSourceLocation());

		// create child nodes
		for (INEDElement child = pojoNode.getFirstChild(); child != null; child = child.getNextSibling()) {
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
    public static void cleanupPojoTree(INEDElement pojoNode) {
        // filter the child nodes first
        for (INEDElement child = pojoNode.getFirstChild(); child != null; child = child.getNextSibling()) {
            cleanupPojoTree(child);
        }

        // see if the current node can be filtered out

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
        if ((pojoNode instanceof TypesNode
                || pojoNode instanceof ParametersNode
                || pojoNode instanceof GatesNode
                || pojoNode instanceof SubmodulesNode
                || pojoNode instanceof ConnectionsNode && !((ConnectionsNode)pojoNode).getAllowUnconnected())
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
		}
		else {
			result += "> \n";
			for (NEDElement child = swigNode.getFirstChild(); child != null; child = child
					.getNextSibling())
				result += generateXmlFromSwigElementTree(child, indent + "  ");

			result += indent + "</" + swigNode.getTagName() + ">\n";
		}
		return result;
	}

	public static String generateXmlFromPojoElementTree(INEDElement pojoNode, String indent, boolean printDebugString) {
		String result = indent;
		result += "<" + pojoNode.getTagName();
		for (int i = 0; i < pojoNode.getNumAttributes(); ++i)
			result += " " + pojoNode.getAttributeName(i) + "=\""
					+ pojoNode.getAttribute(i) + "\"";

        String debugString = !printDebugString ? "" : pojoNode.debugString();
        if (!"".equals(debugString))
                debugString = "<!-- "+debugString + " -->";

		if (pojoNode.getFirstChild() == null) {
			result += "/> " +  debugString + "\n";
		}
		else {
			result += "> " +  debugString + "\n";
			for (INEDElement child = pojoNode.getFirstChild(); child != null; child = child.getNextSibling())
				result += generateXmlFromPojoElementTree(child, indent + "  ", printDebugString);

			result += indent + "</" + pojoNode.getTagName() + ">\n";
		}
		return result;
	}

    /**
     * Returns true if the two trees generate the same NED source code.
     * If either parameter is null, it returns false.
     */
    public static boolean isNEDTreeEqual(INEDElement tree1, INEDElement tree2) {
        if (tree1 == tree2)
            return true;
        if (tree1==null || tree2==null)
            return false;
        return tree1.getNEDSource().equals(tree2.getNEDSource());
    }

    /**
     * Returns the default content provider for ned model trees
     */
    public static ITreeContentProvider getNedModelContentProvider() {
        return nedModelContentProvider;
    }

    /**
     * Returns the default label/icon provider for ned model trees
     */
    public static ILabelProvider getNedModelLabelProvider() {
        return nedModelLabelProvider;
    }

}
