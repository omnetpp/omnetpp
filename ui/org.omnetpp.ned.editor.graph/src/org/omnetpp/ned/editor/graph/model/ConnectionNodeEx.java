package org.omnetpp.ned.editor.graph.model;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.Bendpoint;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.ConnectionNode;

public class ConnectionNodeEx extends ConnectionNode {
	private INedNode srcModuleRef;
	private INedNode destModuleRef;

	public ConnectionNodeEx(NEDElement parent) {
		super(parent);
	}

	public INedNode getSrcModuleRef() {
		return srcModuleRef;
	}
	
	public void setSrcModuleRef(INedNode srcModule) {
		assert(srcModule != null);
		if(srcModuleRef == srcModule) 
			return;
		
		if (srcModuleRef != null) 
			srcModuleRef.removeSrcConnection(this);
		srcModuleRef = srcModule;
		srcModuleRef.addSrcConnection(this);
		attributeChanged(ATT_SRC_MODULE);
	}

	public INedNode getDestModuleRef() {
		return destModuleRef;
	}

	public void setDestModuleRef(INedNode destModule) {
		assert(destModule != null);
		if (destModuleRef == destModule)
			return;
		
		if (destModuleRef != null) 
			destModuleRef.removeDestConnection(this);
		destModuleRef = destModule;
		destModuleRef.addDestConnection(this);
		attributeChanged(ATT_DEST_MODULE);
	}

	@Override
	public String getDestModule() {
		// if the destination is the compound module, do not return its name  
		if(destModuleRef instanceof CompoundModuleNodeEx)
			return "";
		return getDestModuleRef().getName();
	}

	@Override
	public String getSrcModule() {
		// if the destination is the compound module, do not return its name  
		if(srcModuleRef instanceof CompoundModuleNodeEx)
			return "";
		return getSrcModuleRef().getName();
	}

	@Override
	public void setDestModule(String val) {
		setDestModuleRef(getSubmoduleByName(val));
	}

	@Override
	public void setSrcModule(String val) {
		setSrcModuleRef(getSubmoduleByName(val));
	}
	
	private INedNode getSubmoduleByName(String moduleName) {
		CompoundModuleNodeEx compMod = (CompoundModuleNodeEx)getParentWithTag(NED_COMPOUND_MODULE);
		assert (compMod != null);
		// check if the module name is empty. we should return the parent compoud module
		if("".equals(moduleName)) 
			return compMod;
		else {
			INedNode subMod = compMod.getSubmoduleByName(moduleName);
			if (subMod == null) throw new IllegalArgumentException("Nonexistent submodule: "+moduleName);
			return subMod;
		}
	}

	public List getBendpoints() {
		// TODO Implement bendpoints model
		return new ArrayList();
	}

	public void insertBendpoint(int index, Bendpoint wbp) {
		// TODO Implement this
	}

	public void removeBendpoint(int index) {
		// TODO Implement this
	}

	public void setBendpoint(int index, Bendpoint bp) {
		// TODO Implement this
	}
	
}
