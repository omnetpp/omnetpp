/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.treeproviders;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.viewers.LabelProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.model.Inputs;

/**
 * Label provider for the "Physical view" tree of the Inputs page.
 */
public class InputsViewLabelProvider extends LabelProvider {

	public String getText(Object element) {
		if (element instanceof GenericTreeNode) {
			GenericTreeNode node = (GenericTreeNode)element;
			Object payload = node.getPayload();
			if (payload instanceof Inputs)
				return "";
			else if (payload instanceof ResultFile) {
				ResultFile file = (ResultFile)payload;
				IFile ifile = ScaveEditor.findFileInWorkspace(file.getFilePath());
				return ifile != null ? ifile.getFullPath().toString() : file.getFilePath();
			}
			else if (payload instanceof Run) {
				Run run = (Run)payload;
				if (run.getRunNumber()==0 && run.getRunName().equals("")) 
					return "(unnamed run)"; // old vector files
				else if (run.getRunNumber()!=0 && run.getRunName().equals("")) 
					return "run "+run.getRunNumber()+" - unnamed"; // old scalar files
				else if (run.getRunNumber()==0) 
					return "run \""+run.getRunName()+"\"";  // cannot normally happen
				else 
					return "run "+run.getRunNumber()+" - \""+run.getRunName()+"\"";
			}
			else if (payload != null && !payload.equals(""))
				return payload.toString();
		}
		
		return null;
	}
}
