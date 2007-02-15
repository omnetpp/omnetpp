package org.omnetpp.eventlogtable.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IPathEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.eventlog.engine.EventLog;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.engine.IEventLog;

// TODO: resuse this class from sequence chart
public abstract class EventLogEditor extends EditorPart {
	protected IEventLog eventLog;
	
	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
		setSite(site);
		setInput(input);
		setPartName(input.getName());

		String logFileName;
		if (input instanceof IFileEditorInput) {
			IFileEditorInput fileInput = (IFileEditorInput)input;
			logFileName = fileInput.getFile().getLocation().toFile().getAbsolutePath();
		}
		else if (input instanceof IPathEditorInput) {
			IPathEditorInput pathFileInput = (IPathEditorInput)input;
			logFileName = pathFileInput.getPath().toFile().getAbsolutePath();
		}
		else 
			throw new PartInitException("Unsupported input type");

		eventLog = new EventLog(new FileReader(logFileName, /* EventLog will delete it */false));
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		// void
	}

	@Override
	public void doSaveAs() {
		// void
	}

	@Override
	public boolean isDirty() {
		return false;
	}

	@Override
	public boolean isSaveAsAllowed() {
		return false;
	}
}
