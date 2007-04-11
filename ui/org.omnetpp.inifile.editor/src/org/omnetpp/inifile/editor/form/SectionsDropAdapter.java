package org.omnetpp.inifile.editor.form;

import java.util.ArrayList;

import org.eclipse.jface.viewers.TreeSelection;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.inifile.editor.model.IInifileDocument;


/**
 * Drop adapter for modifying section interitance in the inifile.
 */
public class SectionsDropAdapter extends DropTargetAdapter {
	protected Viewer viewer;
	protected IInifileDocument doc;

	/**
	 * This creates an instance with the given domain and viewer.
	 */
	public SectionsDropAdapter(IInifileDocument doc, Viewer viewer)
	{
		this.viewer = viewer;
		this.doc = doc;
	}

	/**
	 * This is called to indicate that the drop action should be invoked.
	 */
	public void drop(DropTargetEvent event)
	{
		System.out.println("drop()");

		RunsPage.Payload[] draggedSections = getPayloadsFrom(event.data);
		RunsPage.Payload targetSection = getPayloadFrom(event.item==null ? null : event.item.getData());

		if (draggedSections.length != 0 && targetSection != null) {
	    	System.out.println(draggedSections.length + " items dropped to: "+targetSection);

	    	String newExtends = targetSection.sectionName.equals("General") ? null : targetSection.sectionName.replaceFirst("Config +", "");
	    	for (RunsPage.Payload draggedPayload : draggedSections)
	    		setSectionExtendsKey(draggedPayload.sectionName, newExtends);
	    }
	}

	private void setSectionExtendsKey(String sectionName, String newExtends) {
		if (newExtends == null)
			doc.removeKey(sectionName, "extends");
		else {
			if (!doc.containsKey(sectionName, "extends"))
				doc.addEntry(sectionName, "extends", newExtends, "", null); //XXX before the first!
			else 
				doc.setValue(sectionName, "extends", newExtends);
		}
	}
	
	@SuppressWarnings("unchecked")
	private RunsPage.Payload[] getPayloadsFrom(Object data) {
		if (data instanceof TreeSelection) {
			ArrayList list = new ArrayList();
			for (Object item : ((TreeSelection) data).toArray()) {
				RunsPage.Payload payload = getPayloadFrom(item);
				if (payload != null)
					list.add(payload);
			}
			return (RunsPage.Payload[]) list.toArray(new RunsPage.Payload[]{});
		}
		return null;
	}

	private RunsPage.Payload getPayloadFrom(Object data) {
		if (data instanceof GenericTreeNode) {
			Object payload = ((GenericTreeNode)data).getPayload();
			if (payload instanceof RunsPage.Payload)
				return (RunsPage.Payload) payload;
		}
		return null;
	}
}
