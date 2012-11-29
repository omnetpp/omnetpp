package org.omnetpp.main.sequencechart;

import java.util.ArrayList;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IStartup;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogSelection;
import org.omnetpp.common.util.CommandlineUtils;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.IMessageDependency;

public class Startup implements IStartup {

	@Override
	public void earlyStartup() {
        final ISelectionListener listener = new ISelectionListener() {
			@Override
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (selection instanceof EventLogSelection) {
					EventLogSelection eventSelection = (EventLogSelection)selection;
					ArrayList<Long> eventNumbers = eventSelection.getEventNumbers();
					if (eventNumbers.size() > 0) {
						long eventNumber = eventNumbers.get(eventNumbers.size()-1);
						IEventLog eventLog = eventSelection.getEventLogInput().getEventLog();
						IEvent event = eventLog.getEventForEventNumber(eventNumber);

						IFile eventLogFile = eventSelection.getEventLogInput().getFile();
						String fileName = eventLogFile.getLocationURI().getRawSchemeSpecificPart();
						String eventNumberStr = String.valueOf(eventNumber);
						BigDecimal simulationTime = event.getEventEntry().getSimulationTime();
						IMessageDependency causeMessage = event.getCause();
						String messageName = (causeMessage != null) ? causeMessage.getMessageEntry().getMessageName() : "";
						String messageId = (causeMessage != null) ? String.valueOf(causeMessage.getMessageEntry().getMessageId()) : "";
						
						System.out.println(fileName+"\t"+eventNumberStr+"\t"+simulationTime+"\t"+messageId+"\t"+messageName);
					}
					
				    
				}
			}
		};

		Display.getDefault().asyncExec(new Runnable() {
			@Override
			public void run() {
				for (IWorkbenchWindow window : PlatformUI.getWorkbench().getWorkbenchWindows())
					for (IWorkbenchPage page : window.getPages())
						page.addSelectionListener(listener);
				CommandlineUtils.autoimportAndOpenFilesOnCommandLine();
			}
		});
		
	}

}
