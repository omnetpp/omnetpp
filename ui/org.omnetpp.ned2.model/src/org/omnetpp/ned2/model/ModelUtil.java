package org.omnetpp.ned2.model;

import org.omnetpp.ned2.model.swig.NEDElement;
import org.omnetpp.ned2.model.swig.NEDGenerator;
import org.omnetpp.ned2.model.swig.NEDParser;

public class ModelUtil {
//    private static final String NED_EMF_MODEL_PACKAGE = "org.omnetpp.ned.model.emf";

	public static String generateNedSource(NEDElement treeRoot) {
      NEDGenerator ng = new NEDGenerator();
      return ng.generate(treeRoot, "");
	}
	
	public static NEDElement parseNedSource(String source) {
        NEDParser np = new NEDParser();
        np.setParseExpressions(false);
        np.parseText(source);
        NEDElement treeRoot = np.getTree();
        // XXX just for debugging
        System.out.println(ModelUtil.printSwigElementTree(treeRoot,""));
        return treeRoot;
	}
	
	public static NEDElement loadNedSource(String fname) {
        NEDParser np = new NEDParser();
        np.setParseExpressions(false);
        np.parseFile(fname);
        NEDElement treeRoot = np.getTree();
        // XXX just for debugging
        System.out.println(ModelUtil.printSwigElementTree(treeRoot, ""));
        return treeRoot;
	}

	public static String printSwigElementTree(NEDElement swigNode, String indent) {
        String result = indent;
        result += "<"+ swigNode.getTagName();
        for(int i=0; i < swigNode.getNumAttributes(); ++i)
            result += " "+swigNode.getAttributeName(i)+"=\""+swigNode.getAttribute(i)+"\"";
        if (swigNode.getFirstChild() == null) {
        	result += "/> \n";
        } else {
        	result += "> \n";
        	for(NEDElement child = swigNode.getFirstChild(); child != null; child = child.getNextSibling())
        		result += printSwigElementTree(child, indent+"  ");

        	result += indent + "</"+ swigNode.getTagName()+">\n";
        }
        return result;
    }

    // TODO special cases and exceptions should be handled correctly
    // GATE In/Out; import statement; for loop and variable
//    public static EObject swig2emf(NEDElement swigNode) {
//        EClass eClass = Swig2EmfMapper.getEClass(swigNode.getTagName());
//        System.out.println("Node: "+swigNode.getTagName()+" -> "+eClass);
//        if (eClass == null) return null;
//        
//        EObject emfNode = NedFactory.eINSTANCE.create(eClass);
//
//        // set the attributes        
//        for(int i=0; i < swigNode.getNumAttributes(); ++i) {
//            EStructuralFeature eFeature = Swig2EmfMapper.getEStructuralFeature(swigNode.getTagName(), swigNode.getAttributeName(i));
//            System.out.println("  Attr: "+swigNode.getAttributeName(i)+" -> "+eFeature);
//            if (eFeature == null) continue;
//            
//            Object value = swigNode.getAttribute(i);
//            // convert the true/false string to booolean. all other attributes are stored as strings
//            if (emfNode.eGet(eFeature) instanceof Boolean)
//                value = Boolean.valueOf((String)value);
//            emfNode.eSet(eFeature, value);
//        }
//
//        // create child nodes
//        for(NEDElement child = swigNode.getFirstChild(); child != null; child = child.getNextSibling()) {
//            EObject emfChild = swig2emf(child);
//            if (emfChild == null) continue;
//            EStructuralFeature eFeature = Swig2EmfMapper.getEStructuralFeature(swigNode.getTagName(), child.getTagName());
//            System.out.println("  Assigning: "+swigNode.getTagName()+"."+child.getTagName()+" -> "+eFeature);
//            if (eFeature == null) continue;
//            
//            // if more than one child is possible (ie. the feature is an EList -> add the child to the EList)
//            // otherwise just set it as a usual attribute
//            Object childContainer = emfNode.eGet(eFeature);
//            if (childContainer instanceof EList)
//                ((EList)childContainer).add(emfChild);
//            else emfNode.eSet(eFeature, emfChild); 
//        }
//            
//        return emfNode;
//    }

}
