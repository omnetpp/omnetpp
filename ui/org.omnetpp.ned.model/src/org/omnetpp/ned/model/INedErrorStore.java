/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model;

import org.eclipse.core.resources.IFile;



/**
 * Interface for issuing NED error messages
 *
 * @author andras
 */
public interface INedErrorStore {

    /**
     * Sets the file that subsequent error/warning messages refer to.
     */
    public void setFile(IFile file);

    public void addError(INedElement context, String message);

    public void addError(INedElement context, int line, String message);

    public void addWarning(INedElement context, String message);

    public void addWarning(INedElement context, int line, String message);

    public void add(int severity, INedElement context, int line, String message);

    public int getNumProblems();

}
