package org.omnetpp.simulation.inspectors;

import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.model.cMessageHeap;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.model.cPar;
import org.omnetpp.simulation.model.cQueue;
import org.omnetpp.simulation.model.cWatchBase;

public class BuiltinInspectorTypes {
    public static class GraphicalModule implements IInspectorType {
        @Override
        public boolean supports(cObject object) {
            return object instanceof cModule && ((cModule)object).hasSubmodules();
        }

        @Override
        public int getScore(cObject object) {
            return 500;
        }

        @Override
        public IInspectorPart create(InspectorDescriptor descriptor, IInspectorContainer parent, cObject object) {
            return new GraphicalModuleInspectorPart(descriptor, parent, (cModule)object);
        }
    };

    public static class InfoText implements IInspectorType {
        @Override
        public boolean supports(cObject object) {
            return true;
        }

        @Override
        public int getScore(cObject object) {
            return (object instanceof cPar || object instanceof cWatchBase) ? 300 : 50;
        }

        @Override
        public IInspectorPart create(InspectorDescriptor descriptor, IInspectorContainer parent, cObject object) {
            return new InfoTextInspectorPart(parent, object, descriptor);
        }
    };

    public static class ObjectFields implements IInspectorType {
        @Override
        public boolean supports(cObject object) {
            return true;
        }

        @Override
        public int getScore(cObject object) {
            return 200;
        }

        @Override
        public IInspectorPart create(InspectorDescriptor descriptor, IInspectorContainer parent, cObject object) {
            return new ObjectFieldsInspectorPart(descriptor, parent, object);
        }
    };

    public static class Queue implements IInspectorType {
        @Override
        public boolean supports(cObject object) {
            return object instanceof cQueue || object instanceof cMessageHeap;
        }

        @Override
        public int getScore(cObject object) {
            return 400;
        }

        @Override
        public IInspectorPart create(InspectorDescriptor descriptor, IInspectorContainer parent, cObject object) {
            return new QueueInspectorPart(descriptor, parent, object);
        }
    };

    public static class ExampleSWT implements IInspectorType {  //TODO remove this inspector eventually
        @Override
        public boolean supports(cObject object) {
            return true;
        }

        @Override
        public int getScore(cObject object) {
            return 1;
        }

        @Override
        public IInspectorPart create(InspectorDescriptor descriptor, IInspectorContainer parent, cObject object) {
            return new ExampleSWTInspectorPart(descriptor, parent, (cModule)object);
        }
    };

}
