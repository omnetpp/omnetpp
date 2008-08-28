package org.omnetpp.common.eventlog;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.util.MatchExpressionContentProposalProvider;
import org.omnetpp.common.util.MatchExpressionSyntax;
import org.omnetpp.common.util.MatchExpressionSyntax.Node;
import org.omnetpp.common.util.MatchExpressionSyntax.Token;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.BubbleEntry;
import org.omnetpp.eventlog.engine.CancelEventEntry;
import org.omnetpp.eventlog.engine.ConnectionCreatedEntry;
import org.omnetpp.eventlog.engine.ConnectionDeletedEntry;
import org.omnetpp.eventlog.engine.ConnectionDisplayStringChangedEntry;
import org.omnetpp.eventlog.engine.DeleteMessageEntry;
import org.omnetpp.eventlog.engine.EndSendEntry;
import org.omnetpp.eventlog.engine.EventEntry;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.GateCreatedEntry;
import org.omnetpp.eventlog.engine.GateDeletedEntry;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.ModuleDeletedEntry;
import org.omnetpp.eventlog.engine.ModuleDisplayStringChangedEntry;
import org.omnetpp.eventlog.engine.ModuleMethodBeginEntry;
import org.omnetpp.eventlog.engine.ModuleMethodEndEntry;
import org.omnetpp.eventlog.engine.ModuleReparentedEntry;
import org.omnetpp.eventlog.engine.PStringVector;
import org.omnetpp.eventlog.engine.SendDirectEntry;
import org.omnetpp.eventlog.engine.SendHopEntry;
import org.omnetpp.eventlog.engine.SimulationBeginEntry;
import org.omnetpp.eventlog.engine.SimulationEndEntry;

public class EventLogEntryProposalProvider extends MatchExpressionContentProposalProvider {
    protected Class<?> clazz;

    protected static Map<Class<?>, ContentProposal[]> classToFieldProposalsMap = new HashMap<Class<?>, ContentProposal[]>();
    
    static {
        // FIXME: KLUDGE: Java reflection is so lame that we can't enumerate these classes automagically
        addClassFieldProposals(SimulationBeginEntry.class);
        addClassFieldProposals(SimulationEndEntry.class);
        addClassFieldProposals(BubbleEntry.class);
        addClassFieldProposals(ModuleMethodBeginEntry.class);
        addClassFieldProposals(ModuleMethodEndEntry.class);
        addClassFieldProposals(ModuleCreatedEntry.class);
        addClassFieldProposals(ModuleDeletedEntry.class);
        addClassFieldProposals(ModuleReparentedEntry.class);
        addClassFieldProposals(GateCreatedEntry.class);
        addClassFieldProposals(GateDeletedEntry.class);
        addClassFieldProposals(ConnectionCreatedEntry.class);
        addClassFieldProposals(ConnectionDeletedEntry.class);
        addClassFieldProposals(ConnectionDisplayStringChangedEntry.class);
        addClassFieldProposals(ModuleDisplayStringChangedEntry.class);
        addClassFieldProposals(EventEntry.class);
        addClassFieldProposals(CancelEventEntry.class);
        addClassFieldProposals(BeginSendEntry.class);
        addClassFieldProposals(EndSendEntry.class);
        addClassFieldProposals(SendDirectEntry.class);
        addClassFieldProposals(SendHopEntry.class);
        addClassFieldProposals(DeleteMessageEntry.class);
    }
    
    private static void addClassFieldProposals(Class<?> clazz) {
        try {
            EventLogEntry eventLogEntry = (EventLogEntry)clazz.newInstance();
            
            PStringVector names = eventLogEntry.getAttributeNames();
            ContentProposal[] fieldProposals = new ContentProposal[(int) names.size()];

            for (int i = 0; i < names.size(); i++) {
                String name = names.get(i);
                fieldProposals[i] = new ContentProposal(name);
            }
            
            classToFieldProposalsMap.put(clazz, fieldProposals);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public EventLogEntryProposalProvider(Class<?> clazz) {
        this.clazz = clazz;
    }

    @Override
    protected void addProposalsForToken(String contents, int position, Token token, List<IContentProposal> proposals) {
        Node parent = token.getParent();
        if (parent != null) {
            int type = parent.getType();
            String prefix;
            int startIndex, endIndex, decorators;
            boolean atEnd = token.getEndPos() <= position;
            
            // TODO: fill in cases

            // inside field name: replace field name
            if (type == Node.FIELDPATTERN && token == parent.getField() && !atEnd) {
            }
            // after field name: complete field name
            else if (type == Node.FIELDPATTERN && token == parent.getField() && atEnd) {
            }
            // inside the pattern of a field pattern: replace pattern with filter hints of the field
            else if (type == Node.FIELDPATTERN && token == parent.getPattern() && !atEnd) {
            }
            // after the '(' of a field pattern: complete the pattern with hints of the field
            else if (type == Node.FIELDPATTERN && (token == parent.getPattern() || token == parent.getOpeningParen()) && atEnd) {
            }
            // after the ')' of a field pattern or parenthesized expression: insert binary operator
            else if ((type == Node.FIELDPATTERN || type == Node.PARENTHESISED_EXPR)&& token == parent.getClosingParen()) {
            }
            // after pattern (may be a field name without '('): complete to field name, pattern, unary operator
            else if (type == Node.PATTERN && token == parent.getPattern()) {
                prefix = parent.getPatternString();
                startIndex = parent.getPattern().getStartPos();
                endIndex = parent.getPattern().getEndPos();
                collectFilteredProposals(proposals, unaryOperatorProposals, prefix, startIndex, endIndex, ContentProposal.DEC_NONE);
                collectFilteredProposals(proposals, classToFieldProposalsMap.get(clazz), prefix, startIndex, endIndex, ContentProposal.DEC_QUOTE | ContentProposal.DEC_OP | ContentProposal.DEC_CP);
            }
            // inside unary operator: replace unary operator
            else if (type == Node.UNARY_OPERATOR_EXPR && !atEnd) {
            }
            // inside binary operator: replace the binary operator
            else if (type == Node.BINARY_OPERATOR_EXPR && !atEnd) {
            }
            // incomplete binary operator
            else if ((type == Node.BINARY_OPERATOR_EXPR && token.isIncomplete())) {
            }
            // after unary or binary operator or empty input: insert unary operator, field name or default pattern
            else if (((type == Node.BINARY_OPERATOR_EXPR  || type == Node.UNARY_OPERATOR_EXPR) && atEnd) ||
                     (type == Node.ROOT && token.getType() == MatchExpressionSyntax.TokenType.END))
            {
            }
        }
    }
}
