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
public abstract class ContentProposalProvider implements IContentProposalProvider {
    private boolean useWholePrefix = false;
    private boolean dropPrefix = false;

    public ContentProposalProvider(boolean useWholePrefix) {
        this(useWholePrefix, true);
    }

    /**
     * Constructor.
     * @param useWholePrefix whether the whole substring before the cursor needs to be
     *                       be matched by completions (false: only the last "word").
     * @param dropPrefix     whether the prefix should be omitted from the proposal content
     *                       (i.e. when "Ne" is already typed, return only "twork"
     *                       and not "Network" as the string to be inserted
     */
    public ContentProposalProvider(boolean useWholePrefix, boolean dropPrefix) {
        this.useWholePrefix = useWholePrefix;
        this.dropPrefix = dropPrefix;
    }


    /* (non-Javadoc)
     * @see org.eclipse.jface.fieldassist.IContentProposalProvider#getProposals(java.lang.String, int)
     */
    public IContentProposal[] getProposals(String contents, int position) {
        ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();

        String prefix = contents.substring(0, position);

        // calculate the last word that the user started to type. This is the basis of
        // proposal filtering: they have to start with this prefix.
        String prefixToMatch = useWholePrefix ? prefix : getCompletionPrefix(prefix);

        List<IContentProposal> candidates = getProposalCandidates(prefix);

        // check if any of the proposals has description. If they do, we set "(no description)"
        // on the others as well. Reason: if left at null, previous tooltip will be shown,
        // which is very confusing.
        boolean descriptionSeen = false;
        for (IContentProposal p : candidates)
            if (!StringUtils.isEmpty(p.getDescription()))
                descriptionSeen = true;

        // collect those candidates that match the last incomplete word in the editor
        for (IContentProposal candidate : candidates) {
            String content = candidate.getContent();
            if (content.startsWith(prefixToMatch) && content.length()!= prefixToMatch.length()) {
                // from the content, drop the prefix that has already been typed by the user
                String modifiedContent = dropPrefix ? content.substring(prefixToMatch.length(), content.length()) : content;
                int modifiedCursorPosition = candidate.getCursorPosition() + modifiedContent.length() - content.length();
                String description = (StringUtils.isEmpty(candidate.getDescription()) && descriptionSeen) ? "(no description)" : candidate.getDescription();
                Image image = candidate instanceof ContentProposal ? ((ContentProposal)candidate).getImage() : null;
                result.add(new ContentProposal(modifiedContent, candidate.getLabel(), description, modifiedCursorPosition, image));
            }
        }

        if (result.isEmpty()) {
            // WORKAROUND: returning an empty array or null apparently causes NPE in the framework, so return a blank proposal instead
            //XXX may cause multiple "(no proposal)" strings to appear in the text editor completion
            result.add(new ContentProposal("", "(no proposal)", null, 0, null));
        }
        return result.toArray(new IContentProposal[] {});
    }

    /**
     * Return the suffix of the text (the last, incomplete "word" the user is typing)
     * for which completions will be provided.
     *
     *  Default version detects words (A-Z, a-z, 0-9, underscore); this can be overridden
     *  in subclasses.
     */
    protected String getCompletionPrefix(String text) {
        // calculate the last word that the user started to type. This is the basis of
        // proposal filtering: they have to start with this prefix.
        return text.replaceFirst("^.*?([A-Za-z0-9_]*)$", "$1");
    }

    /**
     * Generate a list of proposal candidates. They will be sorted and filtered by prefix
     * before presenting them to the user.
     */
    abstract protected List<IContentProposal> getProposalCandidates(String prefix);

    /**
     * Utility function.
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
        IContentProposal[] p = new IContentProposal[strings.length];
        if (labelSuffix==null)
            for (int i=0; i<p.length; i++)
                p[i] = new ContentProposal(strings[i], strings[i].trim(), null);
        else
            for (int i=0; i<p.length; i++)
                p[i] = new ContentProposal(strings[i], strings[i].trim()+" -- "+labelSuffix, null);
        return Arrays.asList(p);
    }

    /**
     * Utility function: Turn strings-and-labels into proposals.
     */
    protected static List<IContentProposal> toProposals(String[] strings, String[] labels) {
        Assert.isTrue(strings.length == labels.length);
        List<IContentProposal> result = new ArrayList<IContentProposal>();
        for (int i=0; i<strings.length; i++)
            result.add(new ContentProposal(strings[i], labels[i], null));
        return result;
    }

    /**
     * Utility function: Turn strings-and-labels into proposals.
     */
    protected static List<IContentProposal> toProposals(Map<String,String> proposalsWithLabels) {
        List<IContentProposal> result = new ArrayList<IContentProposal>();
        for (String proposal : proposalsWithLabels.keySet())
            result.add(new ContentProposal(proposal, proposalsWithLabels.get(proposal), null));
        return result;
    }
}
