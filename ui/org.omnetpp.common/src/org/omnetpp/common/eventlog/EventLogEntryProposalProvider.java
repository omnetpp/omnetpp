/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.eventlog;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.util.MatchExpressionContentProposalProvider;
import org.omnetpp.common.util.MatchExpressionSyntax;
import org.omnetpp.common.util.MatchExpressionSyntax.Token;
import org.omnetpp.common.util.MatchExpressionSyntax.TokenType;
import org.omnetpp.eventlog.EventLogEntry;
import org.omnetpp.eventlog.engine.PStringVector;
import org.omnetpp.eventlog.entry.BeginSendEntry;
import org.omnetpp.eventlog.entry.BubbleEntry;
import org.omnetpp.eventlog.entry.CancelEventEntry;
import org.omnetpp.eventlog.entry.ComponentMethodBeginEntry;
import org.omnetpp.eventlog.entry.ComponentMethodEndEntry;
import org.omnetpp.eventlog.entry.ConnectionCreatedEntry;
import org.omnetpp.eventlog.entry.ConnectionDeletedEntry;
import org.omnetpp.eventlog.entry.ConnectionDisplayStringChangedEntry;
import org.omnetpp.eventlog.entry.DecapsulatePacketEntry;
import org.omnetpp.eventlog.entry.DeleteMessageEntry;
import org.omnetpp.eventlog.entry.EncapsulatePacketEntry;
import org.omnetpp.eventlog.entry.EndSendEntry;
import org.omnetpp.eventlog.entry.EventEntry;
import org.omnetpp.eventlog.entry.GateCreatedEntry;
import org.omnetpp.eventlog.entry.GateDeletedEntry;
import org.omnetpp.eventlog.entry.ModuleCreatedEntry;
import org.omnetpp.eventlog.entry.ModuleDeletedEntry;
import org.omnetpp.eventlog.entry.ModuleDisplayStringChangedEntry;
import org.omnetpp.eventlog.entry.SendDirectEntry;
import org.omnetpp.eventlog.entry.SendHopEntry;
import org.omnetpp.eventlog.entry.SimulationBeginEntry;
import org.omnetpp.eventlog.entry.SimulationEndEntry;

/**
 * Provide content proposals for eventlog lines. The various names
 * are taken from EventlogEntry subclasses.
 *
 * @author andras
 */
public class EventLogEntryProposalProvider extends MatchExpressionContentProposalProvider {
    private String configuredEntryType;  // BS,ES,MC,etc.
    private static Map<String, String[]> entryTypeToFieldNamesMap = new HashMap<>();

    static {
        // enumerate all EventlogEntry subclasses below
        EventLogEntry[] protoTypes = new EventLogEntry[] {
                new SimulationBeginEntry(),
                new SimulationEndEntry(),
                new BubbleEntry(),
                new ComponentMethodBeginEntry(),
                new ComponentMethodEndEntry(),
                new ModuleCreatedEntry(),
                new ModuleDeletedEntry(),
                new GateCreatedEntry(),
                new GateDeletedEntry(),
                new ConnectionCreatedEntry(),
                new ConnectionDeletedEntry(),
                new ConnectionDisplayStringChangedEntry(),
                new ModuleDisplayStringChangedEntry(),
                new EventEntry(),
                new CancelEventEntry(),
                new BeginSendEntry(),
                new EndSendEntry(),
                new SendDirectEntry(),
                new SendHopEntry(),
                new DeleteMessageEntry(),
                new EncapsulatePacketEntry(),
                new DecapsulatePacketEntry()
        };
        for (EventLogEntry prototype : protoTypes)
            entryTypeToFieldNamesMap.put(prototype.getAsString(), prototype.getAttributeNames().toArray(new String[0]));
    }

    public EventLogEntryProposalProvider() {
        configuredEntryType = null;
    }

    public EventLogEntryProposalProvider(String entryType) {
        this.configuredEntryType = entryType;
        if (!entryTypeToFieldNamesMap.containsKey(entryType))
            throw new IllegalArgumentException("unknown entry type: " + entryType);
    }

    public EventLogEntryProposalProvider(EventLogEntry prototype) {
        this(prototype.getAsString());
    }

    protected List<IContentProposal> getFieldNameProposals(String prefix) {
        String entryType = configuredEntryType != null ? configuredEntryType : getLastDefaultFieldValue();
        String[] fieldNames = entryTypeToFieldNamesMap.get(entryType);
        return fieldNames == null ? new ArrayList<>() : sort(toProposals("", fieldNames, " =~ ", null));
    }

    protected List<IContentProposal> getDefaultFieldValueProposals(String prefix) {
        if (configuredEntryType == null) {
            String[] entryTypes = entryTypeToFieldNamesMap.keySet().toArray(new String[0]);
            return sort(toProposals("", entryTypes, " ", null));
        }
        else {
            return new ArrayList<>();
        }
    }

    protected List<IContentProposal> getFieldValueProposals(String fieldName, String prefix) {
        return new ArrayList<>();
    }

    protected String getLastDefaultFieldValue() {
        // find the last STRING_LITERAL which is not followed by MATCHES, so that, for example,
        // if the input is "BS AND t=~10s AND ", we can offer the fields of "BS".
        String text = getFullContentPrefix();
        MatchExpressionSyntax.Lexer lexer = new MatchExpressionSyntax.Lexer(text);
        Token current = null, previous = null;
        while (true) {
            previous = current;
            current = lexer.getNextToken();
            if (previous != null && previous.getType() == TokenType.STRING_LITERAL && current.getType() != TokenType.MATCHES)
                return previous.getValue();
            if (current.getType() == TokenType.END)
                return null;
        }
    }

    private static String[] toArray(PStringVector vec) {
        String[] result = new String[(int)vec.size()];
        for (int i = 0; i < vec.size(); i++)
            result[i] = vec.get(i);
        return result;
    }

}
