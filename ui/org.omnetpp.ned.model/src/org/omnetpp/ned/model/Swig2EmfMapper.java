package org.omnetpp.ned.model;

import java.util.HashMap;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.omnetpp.ned.model.emf.NedPackage;

public class Swig2EmfMapper {
    static private HashMap<String, EClass> elementMap = new HashMap<String, EClass>();
    static private HashMap<String, EStructuralFeature> featureMap = new HashMap<String, EStructuralFeature>();
    
    static {
        NedPackage pkg = NedPackage.eINSTANCE;
        
        // TODO special cases and exceptions should be handled correctly
        System.out.println("Importing tags...");
        for (Object nodeType : pkg.eContents()) {
            if (nodeType instanceof EClass) {
                EClass eClass = (EClass) nodeType;
                // get the XML tagname metadata
                String tagName = (String)eClass.getEAnnotation("http:///org/eclipse/emf/ecore/util/ExtendedMetaData")
                                    .getDetails().get("name");
                addElement(tagName, eClass);
                System.out.println("Mapping "+tagName+" -> "+eClass.getName());
                
                // add the current node's features
                for(Object featureType : eClass.getEStructuralFeatures()) {
                    if (featureType instanceof EStructuralFeature) {
                        EStructuralFeature eFeature = (EStructuralFeature)featureType;
                        String featureName = (String)eFeature.getEAnnotation("http:///org/eclipse/emf/ecore/util/ExtendedMetaData")
                            .getDetails().get("name");
                        addFeature(tagName, featureName, eFeature);
                        System.out.println("   "+tagName+"."+featureName+" -> "+eFeature.getName());
                    }
                }
            }
        }
        
////        add("imported-file", pkg.getImportedFile());
////        add("loop-var", pkg.getLoopVar());
    }
    
    static private void addElement(String key, EClass clazz) {
        elementMap.put(key, clazz);
    }

    static private void addFeature(String parent, String child, EStructuralFeature efeature) {
        featureMap.put(parent+"__"+child, efeature);
    }

    static EClass getEClass(String tagName) {
        return elementMap.get(tagName);
    }

    static EStructuralFeature getEStructuralFeature(String parent, String child) {
        return featureMap.get(parent+"__"+child);
    }
}
