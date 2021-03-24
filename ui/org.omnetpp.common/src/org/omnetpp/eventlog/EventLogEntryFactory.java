package org.omnetpp.eventlog;

import org.omnetpp.eventlog.entry.BeginSendEntry;
import org.omnetpp.eventlog.entry.BubbleEntry;
import org.omnetpp.eventlog.entry.CancelEventEntry;
import org.omnetpp.eventlog.entry.CloneMessageEntry;
import org.omnetpp.eventlog.entry.ComponentMethodBeginEntry;
import org.omnetpp.eventlog.entry.ComponentMethodEndEntry;
import org.omnetpp.eventlog.entry.ConnectionCreatedEntry;
import org.omnetpp.eventlog.entry.ConnectionDeletedEntry;
import org.omnetpp.eventlog.entry.ConnectionDisplayStringChangedEntry;
import org.omnetpp.eventlog.entry.ConnectionDisplayStringFoundEntry;
import org.omnetpp.eventlog.entry.ConnectionFoundEntry;
import org.omnetpp.eventlog.entry.CreateMessageEntry;
import org.omnetpp.eventlog.entry.CustomChangedEntry;
import org.omnetpp.eventlog.entry.CustomCreatedEntry;
import org.omnetpp.eventlog.entry.CustomDeletedEntry;
import org.omnetpp.eventlog.entry.CustomEntry;
import org.omnetpp.eventlog.entry.CustomFoundEntry;
import org.omnetpp.eventlog.entry.DecapsulatePacketEntry;
import org.omnetpp.eventlog.entry.DeleteMessageEntry;
import org.omnetpp.eventlog.entry.EncapsulatePacketEntry;
import org.omnetpp.eventlog.entry.EndSendEntry;
import org.omnetpp.eventlog.entry.EventEntry;
import org.omnetpp.eventlog.entry.GateCreatedEntry;
import org.omnetpp.eventlog.entry.GateDeletedEntry;
import org.omnetpp.eventlog.entry.GateDisplayStringChangedEntry;
import org.omnetpp.eventlog.entry.GateDisplayStringFoundEntry;
import org.omnetpp.eventlog.entry.GateFoundEntry;
import org.omnetpp.eventlog.entry.IndexEntry;
import org.omnetpp.eventlog.entry.MessageDisplayStringChangedEntry;
import org.omnetpp.eventlog.entry.MessageDisplayStringFoundEntry;
import org.omnetpp.eventlog.entry.MessageFoundEntry;
import org.omnetpp.eventlog.entry.ModuleCreatedEntry;
import org.omnetpp.eventlog.entry.ModuleDeletedEntry;
import org.omnetpp.eventlog.entry.ModuleDisplayStringChangedEntry;
import org.omnetpp.eventlog.entry.ModuleDisplayStringFoundEntry;
import org.omnetpp.eventlog.entry.ModuleFoundEntry;
import org.omnetpp.eventlog.entry.ReferenceAddedEntry;
import org.omnetpp.eventlog.entry.ReferenceFoundEntry;
import org.omnetpp.eventlog.entry.ReferenceRemovedEntry;
import org.omnetpp.eventlog.entry.SendDirectEntry;
import org.omnetpp.eventlog.entry.SendHopEntry;
import org.omnetpp.eventlog.entry.SimulationBeginEntry;
import org.omnetpp.eventlog.entry.SimulationEndEntry;
import org.omnetpp.eventlog.entry.SnapshotEntry;

public class EventLogEntryFactory
{
    @SuppressWarnings("unused")
    public static EventLogTokenBasedEntry parseEntry(IChunk chunk, int entryIndex, String[] tokens, int numTokens) {
        if (numTokens < 1)
            return null;

        String code = tokens[0];
        EventLogTokenBasedEntry entry = null;

        if (false)
            ;
        else if (code.length() == 2 && code.charAt(0) == 'S' && code.charAt(1) == 'B') // SB
            entry = new SimulationBeginEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'S' && code.charAt(1) == 'E') // SE
            entry = new SimulationEndEntry(chunk, entryIndex);
        else if (code.length() == 1 && code.charAt(0) == 'E') // E
            entry = new EventEntry(chunk, entryIndex);
        else if (code.length() == 1 && code.charAt(0) == 'S') // S
            entry = new SnapshotEntry(chunk, entryIndex);
        else if (code.length() == 1 && code.charAt(0) == 'I') // I
            entry = new IndexEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'R' && code.charAt(1) == 'F') // RF
            entry = new ReferenceFoundEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'R' && code.charAt(1) == 'A') // RA
            entry = new ReferenceAddedEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'R' && code.charAt(1) == 'R') // RR
            entry = new ReferenceRemovedEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'C' && code.charAt(1) == 'M' && code.charAt(2) == 'B') // CMB
            entry = new ComponentMethodBeginEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'C' && code.charAt(1) == 'M' && code.charAt(2) == 'E') // CME
            entry = new ComponentMethodEndEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'M' && code.charAt(1) == 'C') // MC
            entry = new ModuleCreatedEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'M' && code.charAt(1) == 'D') // MD
            entry = new ModuleDeletedEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'G' && code.charAt(1) == 'C') // GC
            entry = new GateCreatedEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'G' && code.charAt(1) == 'D') // GD
            entry = new GateDeletedEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'C' && code.charAt(1) == 'C') // CC
            entry = new ConnectionCreatedEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'C' && code.charAt(1) == 'D') // CD
            entry = new ConnectionDeletedEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'M' && code.charAt(1) == 'D' && code.charAt(2) == 'C') // MDC
            entry = new ModuleDisplayStringChangedEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'G' && code.charAt(1) == 'D' && code.charAt(2) == 'C') // GDC
            entry = new GateDisplayStringChangedEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'C' && code.charAt(1) == 'D' && code.charAt(2) == 'C') // CDC
            entry = new ConnectionDisplayStringChangedEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'E' && code.charAt(1) == 'D' && code.charAt(2) == 'C') // EDC
            entry = new MessageDisplayStringChangedEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'C' && code.charAt(1) == 'M') // CM
            entry = new CreateMessageEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'C' && code.charAt(1) == 'L') // CL
            entry = new CloneMessageEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'D' && code.charAt(1) == 'M') // DM
            entry = new DeleteMessageEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'E' && code.charAt(1) == 'N') // EN
            entry = new EncapsulatePacketEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'D' && code.charAt(1) == 'E') // DE
            entry = new DecapsulatePacketEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'B' && code.charAt(1) == 'S') // BS
            entry = new BeginSendEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'E' && code.charAt(1) == 'S') // ES
            entry = new EndSendEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'S' && code.charAt(1) == 'D') // SD
            entry = new SendDirectEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'S' && code.charAt(1) == 'H') // SH
            entry = new SendHopEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'C' && code.charAt(1) == 'E') // CE
            entry = new CancelEventEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'M' && code.charAt(1) == 'F') // MF
            entry = new ModuleFoundEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'G' && code.charAt(1) == 'F') // GF
            entry = new GateFoundEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'C' && code.charAt(1) == 'F') // CF
            entry = new ConnectionFoundEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'E' && code.charAt(1) == 'F') // EF
            entry = new MessageFoundEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'M' && code.charAt(1) == 'D' && code.charAt(2) == 'F') // MDF
            entry = new ModuleDisplayStringFoundEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'G' && code.charAt(1) == 'D' && code.charAt(2) == 'F') // GDF
            entry = new GateDisplayStringFoundEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'C' && code.charAt(1) == 'D' && code.charAt(2) == 'F') // CDF
            entry = new ConnectionDisplayStringFoundEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'E' && code.charAt(1) == 'D' && code.charAt(2) == 'F') // EDF
            entry = new MessageDisplayStringFoundEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'B' && code.charAt(1) == 'U') // BU
            entry = new BubbleEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'C' && code.charAt(1) == 'U' && code.charAt(2) == 'C') // CUC
            entry = new CustomCreatedEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'C' && code.charAt(1) == 'U' && code.charAt(2) == 'D') // CUD
            entry = new CustomDeletedEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'C' && code.charAt(1) == 'U' && code.charAt(2) == 'M') // CUM
            entry = new CustomChangedEntry(chunk, entryIndex);
        else if (code.length() == 3 && code.charAt(0) == 'C' && code.charAt(1) == 'U' && code.charAt(2) == 'F') // CUF
            entry = new CustomFoundEntry(chunk, entryIndex);
        else if (code.length() == 2 && code.charAt(0) == 'C' && code.charAt(1) == 'U') // CU
            entry = new CustomEntry(chunk, entryIndex);
        else
            return null;

        entry.parse(tokens, numTokens);
        return entry;
    }
}

