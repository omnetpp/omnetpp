/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.contentassist;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.util.StringUtils;

/**
 * A basic content proposal provider. Performs filtering by prefix the proposal candidates
 * returned by the abstract getProposalCandidates() method. Sorting the proposals should
 * be done within getProposalCandidates() too.
 *
 * Note: although IContentProposalProvider is for field editors, we use this class
 * in the text editor content assist too; we just re-wrap IContentProposals to
 * ICompletionProposal.
 *
 * @author Andras
 */
public abstract class ContentProposalProviderBase implements IContentProposalProvider {

    /**
     * Implement this, with the help of filterAndWrapProposals() and the other utility functions.
     */
    @Override
    public abstract IContentProposal[] getProposals(String contents, int position);

    /**
     * Utility function: Return the line being typed.
     */
    protected static String getLinePrefix(String contents, int position) {
        String contentPrefix = contents.substring(0, position);
        return contentPrefix.contains("\n") ? StringUtils.substringAfterLast(contentPrefix, "\n") : contentPrefix;
    }

    /**
     * Utility function: Return the word being typed.
     */
    protected static String getWordPrefix(String contents, int position) {
        String contentPrefix = contents.substring(0, position);
        return contentPrefix.replaceFirst("^.*?([A-Za-z0-9_]*)$", "$1");
    }

    /**
     * Collect those candidates that match a given prefix (usually the last
     * incomplete word in the editor), optionally removing the prefix from them
     * (so we don't duplicate what the user has already typed.)
     *
     * dropPrefix: This one is tricky, and in fact it shouldn't exist at all.
     * Ideally, when the proposal is selected, the prefix should be removed from the editor,
     * and the entire proposal content inserted. However, not every adapter honors
     * IContentProposalEx's startIndex/endIndex, and those that don't honor it won't
     * remove the prefix from the editor. At those places, the workaround is to
     * drop the prefix from the proposal content instead. dropPrefix=false selects the
     * former (correct) mode, and dropPrefix=true selects the latter (workaround) one.
     */
    protected static IContentProposal[] filterAndWrapProposals(List<IContentProposal> candidates, String prefix, boolean dropPrefix, int position) {
        // check if any of the proposals has description. If they do, we set "(no description)"
        // on the others as well. Reason: if left at null, previous tooltip will be shown,
        // which is very confusing.
        boolean descriptionSeen = false;
        for (IContentProposal p : candidates)
            if (!StringUtils.isEmpty(p.getDescription()))
                descriptionSeen = true;

        // collect those candidates that match the given prefix (the last incomplete word in the editor), optionally removed the prefix from them.
        // dropPrefix usage is tricky, see method content
        ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();
        for (IContentProposal candidate : candidates) {
            String content = candidate.getContent();
            if (content.toLowerCase().startsWith(prefix.toLowerCase()) && content.length()!= prefix.length()) {
                // from the content, drop the prefix that has already been typed by the user
                String modifiedContent = dropPrefix ? content.substring(prefix.length()) : content;
                int modifiedCursorPosition = candidate.getCursorPosition() + modifiedContent.length() - content.length();
                String description = (StringUtils.isEmpty(candidate.getDescription()) && descriptionSeen) ? "(no description)" : candidate.getDescription();
                Image image = candidate instanceof ContentProposalEx ? ((ContentProposalEx)candidate).getImage() : null;
                ContentProposalEx proposal = new ContentProposalEx(modifiedContent, candidate.getLabel(), description, modifiedCursorPosition, image);
                proposal.setStartIndex(dropPrefix ? position : position - prefix.length());
                proposal.setEndIndex(position);
                result.add(proposal);
            }
        }
        return result.toArray(new IContentProposal[] {});
    }

    /**
     * Utility function: Sort proposals
     */
    protected static IContentProposal[] sort(IContentProposal[] proposals) {
        Arrays.sort(proposals);
        return proposals;
    }

    @SuppressWarnings({ "unchecked", "rawtypes" })
    protected static List<IContentProposal> sort(List<IContentProposal> proposals) {
        Collections.sort((List)proposals);
        return proposals;
    }

    /**
     * Utility function: Turn strings into proposals.
     */
    protected static List<IContentProposal> toProposals(String[] strings) {
        return toProposals(strings, (String)null);
    }

    /**
     * Utility function: Turn strings into proposals.
     */
    protected static List<IContentProposal> toProposals(String[] strings, String labelSuffix) {
        return toProposals("", strings, "", labelSuffix);
    }

    /**
     * Utility function: Turn strings into proposals, adding the same prefix and suffix to each one.
     */
    protected static List<IContentProposal> toProposals(String prefix, String[] strings, String suffix, String labelSuffix) {
        List<IContentProposal> proposals = new ArrayList<>(strings.length);
        String labelSuffix2 = labelSuffix==null ? "" : " -- " + labelSuffix;
        for (String str: strings) {
            String content = prefix + str + suffix;
            proposals.add(new ContentProposalEx(content, content.trim() + labelSuffix2));
        }
        return proposals;
    }

    /**
     * Utility function: Turn strings-and-labels into proposals.
     */
    protected static List<IContentProposal> toProposals(String[] strings, String[] labels) {
        Assert.isTrue(strings.length == labels.length);
        List<IContentProposal> result = new ArrayList<IContentProposal>();
        for (int i=0; i<strings.length; i++)
            result.add(new ContentProposalEx(strings[i], labels[i], null));
        return result;
    }

    /**
     * Utility function: Turn strings-and-labels into proposals.
     */
    protected static List<IContentProposal> toProposals(Map<String,String> proposalsWithLabels) {
        List<IContentProposal> result = new ArrayList<IContentProposal>();
        for (String proposal : proposalsWithLabels.keySet())
            result.add(new ContentProposalEx(proposal, proposalsWithLabels.get(proposal), null));
        return result;
    }
}
