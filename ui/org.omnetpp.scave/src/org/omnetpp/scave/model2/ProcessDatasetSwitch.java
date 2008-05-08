package org.omnetpp.scave.model2;

import java.util.List;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.AddDiscardOp;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.SelectDeselectOp;
import org.omnetpp.scave.model.util.ScaveModelSwitch;

/**
 * Visitor-like thingy, where doSwitch() traverses the dataset.
 * It stops at the "target" item. 
 */
abstract class ProcessDatasetSwitch extends ScaveModelSwitch<Object> {

	protected EObject target;
	protected boolean stopBefore;
	protected ResultFileManager manager;
	private boolean finished;
	

	public ProcessDatasetSwitch(EObject target, boolean stopBefore, ResultFileManager manager) {
		this.target = target;
		this.stopBefore = stopBefore;
		this.manager = manager;
	}
	
	protected void processBaseDataset(Dataset dataset) {
		EObject origTarget = target;
		target = null;
		doSwitch(dataset);
		target = origTarget;
	}
	
	@Override
	protected Object doSwitch(int classifierID, EObject object) {
		Object result = this;
		if (!finished) {
			if (object == target && stopBefore)
				finished = true;
			else {
				result = super.doSwitch(classifierID, object);
				if (object == target)
					finished = true;
			}
		}
		return result;
	}

	public Object caseDataset(Dataset dataset) {
		// TODO recurse on dataset.getBasedOn()
		Dataset baseDataset = dataset.getBasedOn();
		if (baseDataset != null) {
			processBaseDataset(baseDataset);
		}
		
		for (Object item : dataset.getItems())
			doSwitch((EObject)item);

		return this;
	}

	/**
	 * If the target is in the group, then process its items,
	 * otherwise ignore it. (Operations in a group have no
	 * effects outside the group.)
	 */
	public Object caseGroup(Group group) {
		if (EcoreUtil.isAncestor(group, target))
			for (Object item : group.getItems())
				doSwitch((EObject)item);
		return this;
	}

	protected IDList select(IDList source, List<SelectDeselectOp> filters, ResultType type) {
		return DatasetManager.select(source, filters, manager, type);
	}

	protected IDList select(IDList source, AddDiscardOp op) {
		return DatasetManager.select(source, op, manager);
	}
}