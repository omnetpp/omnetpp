package org.omnetpp.common.eventlog;

import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.util.MatchExpressionContentProposalProvider;
import org.omnetpp.common.util.MatchExpressionSyntax.Node;
import org.omnetpp.common.util.MatchExpressionSyntax.Token;
import org.omnetpp.common.util.MatchExpressionSyntax.TokenType;
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
    private Class<?> clazz;
    
    private static Map<Class<?>, ContentProposal> classToDefaultFieldProposalMap = new HashMap<Class<?>, ContentProposal>();

    private static Map<String, Class<?>> defaultFieldToClassMap = new HashMap<String, Class<?>>();

    private static Map<Class<?>, ContentProposal[]> classToFieldProposalsMap = new HashMap<Class<?>, ContentProposal[]>();

    static {
        // FIXME: KLUDGE: Java reflection is so lame that we can't enumerate these classes automagically
        storeProposals(SimulationBeginEntry.class);
        storeProposals(SimulationEndEntry.class);
        storeProposals(BubbleEntry.class);
        storeProposals(ModuleMethodBeginEntry.class);
        storeProposals(ModuleMethodEndEntry.class);
        storeProposals(ModuleCreatedEntry.class);
        storeProposals(ModuleDeletedEntry.class);
        storeProposals(ModuleReparentedEntry.class);
        storeProposals(GateCreatedEntry.class);
        storeProposals(GateDeletedEntry.class);
        storeProposals(ConnectionCreatedEntry.class);
        storeProposals(ConnectionDeletedEntry.class);
        storeProposals(ConnectionDisplayStringChangedEntry.class);
        storeProposals(ModuleDisplayStringChangedEntry.class);
        storeProposals(EventEntry.class);
        storeProposals(CancelEventEntry.class);
        storeProposals(BeginSendEntry.class);
        storeProposals(EndSendEntry.class);
        storeProposals(SendDirectEntry.class);
        storeProposals(SendHopEntry.class);
        storeProposals(DeleteMessageEntry.class);
    }
    
    private static void storeProposals(Class<?> clazz) {
        try {
            EventLogEntry eventLogEntry = (EventLogEntry)clazz.newInstance();

            // default proposal
            String defaultField = eventLogEntry.getDefaultAttribute();
            classToDefaultFieldProposalMap.put(clazz, new ContentProposal(defaultField));
            defaultFieldToClassMap.put(defaultField, clazz);
            
            // field proposals
            PStringVector names = eventLogEntry.getAttributeNames();
            ContentProposal[] fieldProposals = new ContentProposal[(int)names.size()];

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
            int startIndex, endIndex;
            boolean atEnd = token.getEndPos() <= position;
            
            // content: incomplete unary operator
            // example: "N|"
            // action: replace with complete binary operator
            // result: "NOT |"
            if ((type == Node.UNARY_OPERATOR_EXPR && token.isIncomplete())) {
                collectFilteredProposals(proposals, binaryOperatorProposals, token.getValue(), token.getStartPos(), token.getEndPos(), ContentProposal.DEC_SP_AFTER);
            }
            // content: incomplete binary operator
            // example: "A|"
            // action: replace with complete binary operator
            // result: "AND |"
            else if ((type == Node.BINARY_OPERATOR_EXPR && token.isIncomplete())) {
                collectFilteredProposals(proposals, binaryOperatorProposals, token.getValue(), token.getStartPos(), token.getEndPos(), ContentProposal.DEC_SP_AFTER);
            }
            // content: inside binary operator
            // example: "AN|D"
            // action: replace with another binary operator
            // result: "OR|"
            else if (type == Node.BINARY_OPERATOR_EXPR && !atEnd) {
                collectFilteredProposals(proposals, binaryOperatorProposals, "", token.getStartPos(), token.getEndPos(), ContentProposal.DEC_NONE);
            }

            // class specific proposals
            if (clazz == EventLogEntry.class) {
                // content: empty or after the OR binary operator
                // example: "|" or "OR |"
                // action: insert NOT or any of the subclass default fields
                // result: "NOT |" or "OR BS |"
                if (contents.equals("") || (type == Node.BINARY_OPERATOR_EXPR && token.getType() == TokenType.OR && atEnd)) {
                    if (type == Node.PATTERN) {
                        prefix = parent.getPatternString();
                        startIndex = parent.getPattern().getStartPos();
                        endIndex = parent.getPattern().getEndPos();
                    }
                    else {
                        prefix = "";
                        startIndex = token.getEndPos() + 1;
                        endIndex = startIndex;
                    }

                    collectFilteredProposals(proposals, unaryOperatorProposals, prefix, startIndex, endIndex, ContentProposal.DEC_SP_AFTER);
                    collectFilteredProposals(proposals, getSubclassDefaultFieldProposals(), prefix, startIndex, endIndex, ContentProposal.DEC_SP_AFTER);
                }
                // content: after default field
                // example: "BS |"
                // action: insert binary operator
                // result: "BS AND |"
                else if (type == Node.PATTERN && atEnd) {
                    startIndex = token.getEndPos() + 1;
                    endIndex = startIndex;
                    collectFilteredProposals(proposals, binaryOperatorProposals, "", startIndex, endIndex, ContentProposal.DEC_SP_AFTER);
                }
                // content: after default field followed by AND binary operator
                // example: "BS AND |"
                // action: insert NOT or any of the preceding subclass'es fields
                // result: "BS AND m(|)"
                else if ((type == Node.BINARY_OPERATOR_EXPR && token.getType() == TokenType.AND && atEnd)) {
                    prefix = "";
                    startIndex = token.getEndPos() + 1;
                    endIndex = startIndex;
                    collectFilteredProposals(proposals, unaryOperatorProposals, prefix, startIndex, endIndex, ContentProposal.DEC_SP_AFTER);
                    Class<?> clazz = defaultFieldToClassMap.get(parent.getLeftOperand().getPatternString());
                    collectFilteredProposals(proposals, classToFieldProposalsMap.get(clazz), prefix, startIndex, endIndex, ContentProposal.DEC_QUOTE | ContentProposal.DEC_OP | ContentProposal.DEC_CP);
                }
            }
            else {
                // content: empty or after binary operator
                // example: "|" or "AND |"
                // action: insert NOT or any of the class'es fields
                // result: "NOT |" or "AND t(|)"
                if (contents.equals("") || (type == Node.BINARY_OPERATOR_EXPR && atEnd)) {
                    if (type == Node.PATTERN) {
                        prefix = parent.getPatternString();
                        startIndex = parent.getPattern().getStartPos();
                        endIndex = parent.getPattern().getEndPos();
                    }
                    else {
                        prefix = "";
                        startIndex = token.getEndPos() + 1;
                        endIndex = startIndex;
                    }

                    collectFilteredProposals(proposals, unaryOperatorProposals, prefix, startIndex, endIndex, ContentProposal.DEC_SP_AFTER);
                    collectFilteredProposals(proposals, classToFieldProposalsMap.get(clazz), prefix, startIndex, endIndex, ContentProposal.DEC_QUOTE | ContentProposal.DEC_OP | ContentProposal.DEC_CP);
                }
                // content: after field expression
                // example: "t(<expression>) |"
                // action: insert binary operator and a space
                // result: "t(<expression>) AND |"
                else if (type == Node.FIELDPATTERN && atEnd) {
                    startIndex = token.getEndPos() + 1;
                    endIndex = startIndex;
                    collectFilteredProposals(proposals, binaryOperatorProposals, "", startIndex, endIndex, ContentProposal.DEC_SP_AFTER);
                }
            }
        }
    }

    private ContentProposal[] getSubclassDefaultFieldProposals() {
        ContentProposal[] result = classToDefaultFieldProposalMap.values().toArray(new ContentProposal[0]);
        Arrays.sort(result,
            new Comparator<ContentProposal>() {
                public int compare(ContentProposal o1, ContentProposal o2) {
                    return o1.getContent().compareTo(o2.getContent());
                }
        });
        return result;
    }
}
