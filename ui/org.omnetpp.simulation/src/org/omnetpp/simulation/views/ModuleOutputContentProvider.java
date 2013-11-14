package org.omnetpp.simulation.views;

import java.util.Arrays;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.EventEntry;
import org.omnetpp.simulation.controller.LogBuffer;
import org.omnetpp.simulation.ui.ITextChangeListener;
import org.omnetpp.simulation.ui.ITextViewerContentProvider;

/**
 * TODO
 *
 * @author Andras
 */
//TODO getters could use a fixed point plus incremental computation, exploiting the fact that painting of the lines occurs top-down
public class ModuleOutputContentProvider implements ITextViewerContentProvider {
    private LogBuffer logBuffer;
    private IEventEntryFilter filter;
    private IEventEntryLinesProvider linesProvider;
    private ListenerList textChangeListeners = new ListenerList();

    // cached data
    private int lineCount = -1;
    private int entryStartLineNumbers[] = null; // indexed by the entry's index in logBuffer

    public ModuleOutputContentProvider(LogBuffer logBuffer, IEventEntryLinesProvider labelProvider) {
        this.logBuffer = logBuffer;
        this.linesProvider = labelProvider;

//        logBuffer.addChangeListener(new ILogBufferChangedListener() {
//            @Override
//            public void changed(LogBuffer logBuffer) {
//                invalidateIndex();
//                fireTextChangeNotification();
//            }
//        });
    }

    public LogBuffer getLogBuffer() {
        return logBuffer;
    }

    public IEventEntryLinesProvider getLinesProvider() {
        return linesProvider;
    }

    public void setFilter(IEventEntryFilter filter) {
        if (!ObjectUtils.equals(this.filter, filter)) {
            this.filter = filter;
            invalidateIndex();
            fireTextChangeNotification();
        }
    }

    public IEventEntryFilter getFilter() {
        return filter;
    }

    public void refresh() {
        invalidateIndex();
        fireTextChangeNotification();
    }

    protected void invalidateIndex() {
        lineCount = -1;
        entryStartLineNumbers = null;
    }

    protected boolean isIndexValid() {
        return entryStartLineNumbers != null;
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
        Assert.isTrue(filter==null || filter.matches(eventEntry));
        return linesProvider.getLineText(eventEntry,  lineIndex - entryStartLineNumbers[entryIndex]);
    }

    @Override
    public Color getLineColor(int lineIndex) {
        Assert.isTrue(lineIndex >= 0 && lineIndex < lineCount);
        if (lineIndex == lineCount-1)  // empty last line
            return null;

        int entryIndex = getIndexOfEntryAt(lineIndex);
        EventEntry eventEntry = logBuffer.getEventEntry(entryIndex);
        Assert.isTrue(filter==null || filter.matches(eventEntry));
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
            if (filter==null || filter.matches(entry))
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
                    listener.textChanged(ModuleOutputContentProvider.this);
                }

                @Override
                public void handleException(Throwable e) {
                    SimulationPlugin.logError(e);
                }
            });
        }
    }

}
