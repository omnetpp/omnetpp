package org.omnetpp.simulation.views;

import java.util.Arrays;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.simulation.controller.LogBuffer;
import org.omnetpp.simulation.controller.LogBuffer.EventEntry;
import org.omnetpp.simulation.widgets.ITextChangeListener;
import org.omnetpp.simulation.widgets.ITextViewerContent;

/**
 * TODO
 * 
 * @author Andras
 */
//TODO computation of line numbers and offsets could be incremental for efficiency
//TODO getters could use a fixed point plus incremental computation, exploiting the fact that painting of the lines occurs top-down
public class ModuleOutputContent implements ITextViewerContent {
    private LogBuffer logBuffer;
    private String filterModuleFullPath;
    
    // cached data
    private int lineCount;
    private int charCount;
    private int entryStartLineNumbers[]; // indexed by the entry's index in logBuffer  
    private int entryStartOffsets[];

    public ModuleOutputContent(LogBuffer logBuffer) {
        this.logBuffer = logBuffer;
        refresh();
    }
    
    public void setFilterModuleFullPath(String filterModuleFullPath) {
        this.filterModuleFullPath = filterModuleFullPath;
        refresh(); //XXX or at least invalidate
    }
    
    protected boolean filterMatches(LogBuffer.EventEntry entry) {
        if (filterModuleFullPath != null)
            return entry.moduleFullPath.startsWith(filterModuleFullPath);  //XXX properly: equals, or starts with path+"."
        return true;
    }
    
    //XXX to be called when logbuffer content or some filter condition changes
    public void refresh() {
        // recompute line numbers and offsets
        int n = logBuffer.getNumEntries();
        entryStartLineNumbers = new int[n];
        entryStartOffsets = new int[n];
        
        int currentLineNumber = 0;
        int currentOffset = 0;
        for (int i = 0; i < n; i++) {
            entryStartLineNumbers[i] = currentLineNumber;
            entryStartOffsets[i] = currentOffset;

            EventEntry entry = logBuffer.getEventEntry(i);
            currentLineNumber += getNumLines(entry);
            currentOffset += getTextLength(entry);
        }
        
        lineCount = currentLineNumber;
        charCount = currentOffset;
    }
    
    protected int getNumLines(LogBuffer.EventEntry entry) {
        return 1 + entry.logItems.length;   // header plus lines
    }
    
    protected int getTextLength(LogBuffer.EventEntry entry) {
        // for offset computation
        int result = getBannerLine(entry).length();
        for (int i = 0; i < entry.logItems.length; i++)
            result += entry.logItems[i].toString().length();
        return result;
    }

    protected String getEntryLine(LogBuffer.EventEntry entry, int lineNumber) {
        if (lineNumber == 0)
            return getBannerLine(entry);
        else 
            return entry.logItems[lineNumber-1].toString();
    }

    protected Color getEntryLineColor(LogBuffer.EventEntry entry, int lineNumber) {
        if (lineNumber == 0)
            return ColorFactory.BLUE;
        else if (entry.logItems[lineNumber-1] instanceof String)
            return null;
        else
            return ColorFactory.GREY50;
    }

    protected int getEntryLineByOffset(LogBuffer.EventEntry entry, int offset) {
        int currentOffset = 0;
        int numEntryLines = getNumLines(entry);
        for (int i = 0; i < numEntryLines; i++) {
            int lineLength = getEntryLine(entry, i).length();
            if (offset < currentOffset + lineLength)
                return i;
            currentOffset += lineLength;
        }
        throw new RuntimeException();
    }

    protected String getBannerLine(LogBuffer.EventEntry entry) {
        if (entry.moduleId == 0)
            return "Log lines:\n";
        else
            return "Event #" + entry.eventNumber + " t=" + entry.simulationTime.toString() + " at " + entry.moduleFullPath + " on " + entry.messageName + " (" + entry.messageClassName + ")\n";
    }

    @Override
    public int getLineCount() {
        return lineCount;
    }

    @Override
    public int getCharCount() {
        return charCount;
    }

    @Override
    public String getLine(int lineIndex) {
        int entryIndex = Arrays.binarySearch(entryStartLineNumbers, lineIndex);
        if (entryIndex < 0) entryIndex = -entryIndex-2;
        return getEntryLine(logBuffer.getEventEntry(entryIndex),  lineIndex - entryStartLineNumbers[entryIndex]);
    }

    @Override
    public int getLineAtOffset(int offset) {
        int entryIndex = Arrays.binarySearch(entryStartOffsets, offset);
        if (entryIndex < 0) entryIndex = -entryIndex-2;
        return getEntryLineByOffset(logBuffer.getEventEntry(entryIndex),  offset - entryStartOffsets[entryIndex]);
    }

    @Override
    public int getOffsetAtLine(int lineIndex) {
        int entryIndex = Arrays.binarySearch(entryStartLineNumbers, lineIndex);
        if (entryIndex < 0) entryIndex = -entryIndex-2;
        EventEntry entry = logBuffer.getEventEntry(entryIndex);
        int entryLineIndex = lineIndex - entryStartLineNumbers[entryIndex];
        
        int offset = 0;
        Assert.isTrue(entryLineIndex < getNumLines(entry));
        for (int i = 0; i < entryLineIndex; i++)
            offset += getEntryLine(entry, i).length();
        return offset;
    }

    @Override
    public Color getLineColor(int lineIndex) {
        int entryIndex = Arrays.binarySearch(entryStartLineNumbers, lineIndex);
        if (entryIndex < 0) entryIndex = -entryIndex-2;
        return getEntryLineColor(logBuffer.getEventEntry(entryIndex), lineIndex - entryStartLineNumbers[entryIndex]);
        
    }

    @Override
    public void addTextChangeListener(ITextChangeListener listener) {
        //TODO
    }

    @Override
    public void removeTextChangeListener(ITextChangeListener listener) {
        //TODO
    }

}
