package org.omnetpp.ned.core;

import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.notification.INedChangeListener;
import org.omnetpp.ned.model.notification.NedMarkerChangeEvent;
import org.omnetpp.ned.model.notification.NedModelEvent;

/**
 * Default implementation of INedTypeResolver
 *
 * @author Andras
 */
public class ImmutableNedTypeResolver extends NedTypeResolver {
    private INedChangeListener listener = new INedChangeListener() {
        public void modelChanged(NedModelEvent event) {
            ImmutableNedTypeResolver.this.modelChanged(event);
        }
    };

    protected ImmutableNedTypeResolver(NedTypeResolver other) {
        super(other);

        // add change listeners
        for (NedFileElementEx fileElement : nedElementFiles.keySet())
            fileElement.addNedChangeListener(listener);
    }

    protected void modelChanged(NedModelEvent event) {
        if (event instanceof NedMarkerChangeEvent)
            return; // that's OK
        throw new RuntimeException("Attempt to modify element in an immutable type resolver: " + event.toString());
    }
}