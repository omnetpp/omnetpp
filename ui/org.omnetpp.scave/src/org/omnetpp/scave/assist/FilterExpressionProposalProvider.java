/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.util.MatchExpressionContentProposalProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Scave;
import org.omnetpp.scave.model2.FilterField;
import org.omnetpp.scave.model2.FilterHintsCache;

/**
 * Content proposal provider for the filter text fields.
 *
 * @author andras
 */
public class FilterExpressionProposalProvider extends MatchExpressionContentProposalProvider {
    private ResultFileManager manager;
    private IDList idlist;
    private FilterHintsCache filterHintsCache;

    public FilterExpressionProposalProvider() {
    }

    public void setFilterHintsCache(FilterHintsCache filterHintsCache) {
        this.filterHintsCache = filterHintsCache;
    }

    public void setIDList(ResultFileManager manager, IDList idlist) {
        this.manager = manager;
        this.idlist = idlist;
    }

    protected List<IContentProposal> getFieldNameProposals(String prefix) {
        try {
            String fieldPrefix = prefix.contains(":") ? StringUtils.substringBefore(prefix, ":") + ":" : "";  //note: substringBefore() returns the entire string if it doesn't contain the separator!
            FilterField.Kind kind = FilterField.getKind(fieldPrefix);
            String[] hints = filterHintsCache.getNameHints(manager, idlist, kind);
            List<IContentProposal> proposals = toProposals(fieldPrefix, hints, " =~ ", null);
            if (fieldPrefix.isEmpty())
                proposals.addAll(toProposals(new String[] {"runattr:", "attr:", "itervar:", "config:"}));
            return sort(proposals);
        }
        catch (IllegalArgumentException e) { // invalid prefix
            return new ArrayList<>();
        }
    }

    protected List<IContentProposal> getDefaultFieldValueProposals(String prefix) {
        return getFieldValueProposals(Scave.NAME, prefix);
    }

    protected List<IContentProposal> getFieldValueProposals(String fieldName, String prefix) {
        FilterField field = new FilterField(fieldName);
        return sort(toProposals("", quote(filterHintsCache.getValueHints(manager, idlist, field)), " ", null));
    }

    private static String[] quote(String[] items) {
        String[] result = new String[items.length];
        for (int i=0; i<items.length; i++)
            result[i] = StringUtils.quoteString(items[i]);
        return result;
    }
}
