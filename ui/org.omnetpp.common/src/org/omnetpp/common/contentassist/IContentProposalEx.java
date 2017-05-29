/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.contentassist;

import org.eclipse.jface.fieldassist.IContentProposal;

/**
 * This interface extends IContentProposal with a range that specifies which 
 * part of the input should be replaced..
 *
 * @author Andras
 */
public interface IContentProposalEx extends IContentProposal {
    public int getStartIndex();
    public int getEndIndex();
}