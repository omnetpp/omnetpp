package org.omnetpp.simulation;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.IConfigurationElement;
import org.eclipse.core.runtime.IExtension;
import org.eclipse.core.runtime.IExtensionPoint;
import org.eclipse.core.runtime.IRegistryEventListener;
import org.eclipse.core.runtime.Platform;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.simulation.inspectors.IInspectorType;
import org.omnetpp.simulation.inspectors.InspectorDescriptor;
import org.omnetpp.simulation.model.cObject;


/**
 *
 * @author Andras
 */
public class InspectorRegistry {
    public static final String INSPECTORS_EXTENSIONPOINT_ID = "org.omnetpp.simulation.inspectors";
    private Map<String,InspectorDescriptor> inspectorTypes = new HashMap<String,InspectorDescriptor>(); // key: inspector ID

    private IRegistryEventListener listener = new IRegistryEventListener() {
        @Override
        public void removed(IExtensionPoint[] extensionPoints) {
            reload();
        }

        @Override
        public void removed(IExtension[] extensions) {
            reload();
        }

        @Override
        public void added(IExtensionPoint[] extensionPoints) {
            reload();
        }

        @Override
        public void added(IExtension[] extensions) {
            reload();
        }
    };

    public InspectorRegistry() {
        Platform.getExtensionRegistry().addListener(listener, INSPECTORS_EXTENSIONPOINT_ID);
        inspectorTypes = readConfiguredInspectorTypes();
    }

    public void dispose() {
        Platform.getExtensionRegistry().removeListener(listener);
    }

    protected void reload() {
        inspectorTypes = readConfiguredInspectorTypes();
    }

    protected Map<String,InspectorDescriptor> readConfiguredInspectorTypes() {
        Map<String,InspectorDescriptor> result = new HashMap<String,InspectorDescriptor>();
        IConfigurationElement[] config = Platform.getExtensionRegistry().getConfigurationElementsFor(INSPECTORS_EXTENSIONPOINT_ID);
        for (IConfigurationElement e : config) {
            if (e.isValid() && e.getName().equals("inspectorType")) {
                try {
                    String id = e.getAttribute("id");
                    String name = e.getAttribute("name");
                    String description = e.getAttribute("description");
                    String iconPath = e.getAttribute("icon");
                    Image image = iconPath != null ? SimulationPlugin.getCachedImage(iconPath) : null;
                    IInspectorType factory = (IInspectorType) e.createExecutableExtension("class");
                    result.put(id, new InspectorDescriptor(id, name, image, description, factory));
                }
                catch (Exception ex) {
                    SimulationPlugin.logError("Error instantiating inspector type", ex);
                }
            }
        }
        return result;
    }

    public InspectorDescriptor[] getInspectorTypes() {
        return inspectorTypes.values().toArray(new InspectorDescriptor[]{});
    }

    public InspectorDescriptor getInspectorType(String id) {
        return inspectorTypes.get(id);
    }

    public InspectorDescriptor getBestInspectorType(cObject object) {
        // note: we only try to call getScore() if there are at least two candidates; maybe this is not necessary, and then supports() and getScore() can be merged
        InspectorDescriptor bestProvider = null;
        int bestProviderScore = Integer.MIN_VALUE;
        for (InspectorDescriptor inspectorType : inspectorTypes.values()) {
            if (inspectorType.supports(object)) {
                if (bestProvider == null)
                    bestProvider = inspectorType;
                else {
                    if (bestProviderScore == Integer.MIN_VALUE)
                        bestProviderScore = bestProvider.getScore(object);
                    int score = inspectorType.getScore(object);
                    if (score > bestProviderScore) {
                        bestProvider = inspectorType;
                        bestProviderScore = score;
                    }
                }
            }
        }
        return bestProvider;
    }

    /**
     * Sorted by score (best first)
     */
    public InspectorDescriptor[] getSupportedInspectorTypes(cObject object) {
        return null;  //FIXME
    }

    /**
     * Sorted by score (best total score first)
     */
    public InspectorDescriptor[] getCommonSupportedInspectorTypes(cObject[] objects) {
        return null;  //FIXME
    }
}
