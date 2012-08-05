package org.omnetpp.simulation.views;

import java.util.Arrays;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.EventEntry;
import org.omnetpp.simulation.controller.LogBuffer;
import org.omnetpp.simulation.controller.LogBuffer.ILogBufferChangedListener;
import org.omnetpp.simulation.widgets.ITextChangeListener;
import org.omnetpp.simulation.widgets.ITextViewerContent;

/**
 * TODO
 * 
 * @author Andras
 */
//TODO getters could use a fixed point plus incremental computation, exploiting the fact that painting of the lines occurs top-down
public class ModuleOutputContent implements ITextViewerContent {
    private LogBuffer logBuffer;
    private IEventEntryLinesProvider linesProvider; //TODO rename to IEventEntryLinesProvider
    private String filterModuleFullPath;  //TODO move filter into some IEventEntryFilter class?
    private ListenerList textChangeListeners = new ListenerList();
    
    // cached data
    private int lineCount = -1;
    private int entryStartLineNumbers[] = null; // indexed by the entry's index in logBuffer  

    public ModuleOutputContent(LogBuffer logBuffer, IEventEntryLinesProvider labelProvider) {
        this.logBuffer = logBuffer;
        this.linesProvider = labelProvider;
        
        logBuffer.addChangeListener(new ILogBufferChangedListener() {
            @Override
            public void changed(LogBuffer logBuffer) {
                invalidateIndex();
                fireTextChangeNotification();
            }
        });
    }
    
    public void setFilterModuleFullPath(String filterModuleFullPath) {
        this.filterModuleFullPath = filterModuleFullPath;
        invalidateIndex();
        fireTextChangeNotification();
    }

    public void invalidateIndex() {
        lineCount = -1;
        entryStartLineNumbers = null;
    }

    protected boolean isIndexValid() {
        return entryStartLineNumbers != null;
    }
    
    protected boolean filterMatches(EventEntry entry) {  //TODO into some IEventEntryFilter
        if (filterModuleFullPath == null)
        return true; // no filtering

        String moduleFullPath = entry.moduleFullPath;
        if (moduleFullPath == null)
            return false;
        if (moduleFullPath.startsWith(filterModuleFullPath))
            if (moduleFullPath.equals(filterModuleFullPath) || moduleFullPath.startsWith(filterModuleFullPath + "."))
                return true;
        return false;
    }

    @Override
    public int getLineCount() {
        if (!isIndexValid())
            rebuildIndex();
        return lineCount;
    }

    @Override
    public String getLineText(int lineIndex) {
        Assert.isTrue(lineIndex >= 0 && lineIndex < lineCount);
        if (lineIndex == lineCount-1)  // empty last line
            return "";

        int entryIndex = getIndexOfEntryAt(lineIndex);
        EventEntry eventEntry = logBuffer.getEventEntry(entryIndex);
        Assert.isTrue(filterMatches(eventEntry));
        return linesProvider.getLineText(eventEntry,  lineIndex - entryStartLineNumbers[entryIndex]);
    }

    @Override
    public Color getLineColor(int lineIndex) {
        Assert.isTrue(lineIndex >= 0 && lineIndex < lineCount);
        if (lineIndex == lineCount-1)  // empty last line
            return null;
        
        int entryIndex = getIndexOfEntryAt(lineIndex);
        EventEntry eventEntry = logBuffer.getEventEntry(entryIndex);
        Assert.isTrue(filterMatches(eventEntry));
        return linesProvider.getLineColor(eventEntry, lineIndex - entryStartLineNumbers[entryIndex]);
    }

    protected int getIndexOfEntryAt(int lineIndex) {
        if (!isIndexValid())
            rebuildIndex();
        int entryIndex = Arrays.binarySearch(entryStartLineNumbers, lineIndex);
        if (entryIndex < 0) entryIndex = -entryIndex-2;
        
        // entryStartLineNumber[] contains one slot for ALL event entries, even those that 
        // don't match the filter; so we have to find the LAST slot with the same line number,
        // and that will be the matching entry
        int baseLineNumber = entryStartLineNumbers[entryIndex];
        while (entryIndex+1 < entryStartLineNumbers.length && entryStartLineNumbers[entryIndex+1] == baseLineNumber)
            entryIndex++;
        return entryIndex;
    }

    protected void rebuildIndex() {
        //long startTime = System.currentTimeMillis();
        
        // recompute line numbers. note: entryStartLineNumber[] contains one slot 
        // for ALL event entries, even those that don't match the filter!
        int n = logBuffer.getNumEntries();
        entryStartLineNumbers = new int[n];
        
        int currentLineNumber = 0;
        for (int i = 0; i < n; i++) {
            entryStartLineNumbers[i] = currentLineNumber;

            EventEntry entry = logBuffer.getEventEntry(i);
            if (filterMatches(entry))
                currentLineNumber += linesProvider.getNumLines(entry);
        }
        
        lineCount = currentLineNumber + 1;  // note: +1 is for empty last line (content cannot be zero lines!)

        //System.out.println("rebuildIndex() for " + n + " log entries: " + (System.currentTimeMillis()-startTime) + "ms"); --> ~10..20ms for 1 million events
    }
    
    @Override
    public void addTextChangeListener(ITextChangeListener listener) {
        textChangeListeners.add(listener);
    }

    @Override
    public void removeTextChangeListener(ITextChangeListener listener) {
        textChangeListeners.remove(listener);
    }

    protected void fireTextChangeNotification() {
        for (Object o: textChangeListeners.getListeners()) {
            final ITextChangeListener listener = (ITextChangeListener) o;
            SafeRunner.run(new ISafeRunnable() {
                @Override
                public void run() throws Exception {
                    listener.textChanged(ModuleOutputContent.this);
                }
                
                @Override
                public void handleException(Throwable e) {
                    SimulationPlugin.logError(e);
                }
            });
        }
    }

}
