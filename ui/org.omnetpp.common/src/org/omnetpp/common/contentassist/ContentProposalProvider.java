/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.contentassist;

import java.util.List;

import org.eclipse.jface.fieldassist.IContentProposal;

/**
 * Deprecated. Use its base class ContentProposalProvider instead.
 *
 * @author Andras
 */
//TODO this class doesn't add much to its base class, should be removed (and existing users moved to the base class)
public abstract class ContentProposalProvider extends ContentProposalProviderBase {
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

    @Override
    public IContentProposal[] getProposals(String contents, int position) {
        String contentsPrefix = contents.substring(0, position);

        // calculate the last word that the user started to type. This is the basis of
        // proposal filtering: they have to start with this prefix.
        String completionPrefix = useWholePrefix ? contentsPrefix : getCompletionPrefix(contentsPrefix);

        List<IContentProposal> candidates = getProposalCandidates(contentsPrefix);

        return filterAndWrapProposals(candidates, completionPrefix, dropPrefix, position);
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
}
