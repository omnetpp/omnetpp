package org.omnetpp.simulation.ui;

import org.omnetpp.simulation.model.cObject;

/**
 *
 * @author Andras
 */
public interface IEssentialsProvider {

    /**
     * TODO
     */
    boolean supports(cObject object);

    /**
     * TODO
     */
    int getScore(cObject object);

    /**
     * TODO
     */
    boolean hasChildren(cObject object);

    /**
     * TODO
     */
    Object[] getChildren(cObject object);

}
