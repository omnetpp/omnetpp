package org.omnetpp.ned.model;

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.omnetpp.ned.model.emf.NedFactory;
import org.omnetpp.ned.model.swig.NEDElement;

public class ModelUtil {
    private static final String NED_EMF_MODEL_PACKAGE = "org.omnetpp.ned.model.emf";

    public static String printSwigElement(NEDElement swigNode) {
        String result = "";
        result += "<"+ swigNode.getTagName();
        for(int i=0; i < swigNode.getNumAttributes(); ++i)
            result += " "+swigNode.getAttributeName(i)+"=\""+swigNode.getAttribute(i)+"\"";
        result += "> \n";
        
        for(NEDElement child = swigNode.getFirstChild(); child != null; child = child.getNextSibling())
            result += printSwigElement(child);
        
        result += "</"+ swigNode.getTagName()+">\n";
        return result;
    }

    // TODO special cases and exceptions should be handled correctly
    // GATE In/Out; import statement; for loop and variable
    public static EObject swig2emf(NEDElement swigNode) {
        EClass eClass = Swig2EmfMapper.getEClass(swigNode.getTagName());
        System.out.println("Node: "+swigNode.getTagName()+" -> "+eClass);
        if (eClass == null) return null;
        
        EObject emfNode = NedFactory.eINSTANCE.create(eClass);

        // set the attributes        
        for(int i=0; i < swigNode.getNumAttributes(); ++i) {
            EStructuralFeature eFeature = Swig2EmfMapper.getEStructuralFeature(swigNode.getTagName(), swigNode.getAttributeName(i));
            System.out.println("  Attr: "+swigNode.getAttributeName(i)+" -> "+eFeature);
            if (eFeature == null) continue;
            
            Object value = swigNode.getAttribute(i);
            // convert the true/false string to booolean. all other attributes are stored as strings
            if (emfNode.eGet(eFeature) instanceof Boolean)
                value = Boolean.valueOf((String)value);
            emfNode.eSet(eFeature, value);
        }

        // create child nodes
        for(NEDElement child = swigNode.getFirstChild(); child != null; child = child.getNextSibling()) {
            EObject emfChild = swig2emf(child);
            if (emfChild == null) continue;
            EStructuralFeature eFeature = Swig2EmfMapper.getEStructuralFeature(swigNode.getTagName(), child.getTagName());
            System.out.println("  Assigning: "+swigNode.getTagName()+"."+child.getTagName()+" -> "+eFeature);
            if (eFeature == null) continue;
            
            // if more than one child is possible (ie. the feature is an EList -> add the child to the EList)
            // otherwise just set it as a usual attribute
            Object childContainer = emfNode.eGet(eFeature);
            if (childContainer instanceof EList)
                ((EList)childContainer).add(emfChild);
            else emfNode.eSet(eFeature, emfChild); 
        }
            
        return emfNode;
    }

}
