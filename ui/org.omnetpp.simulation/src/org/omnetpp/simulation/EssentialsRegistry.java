package org.omnetpp.simulation;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IConfigurationElement;
import org.eclipse.core.runtime.IExtension;
import org.eclipse.core.runtime.IExtensionPoint;
import org.eclipse.core.runtime.IRegistryEventListener;
import org.eclipse.core.runtime.Platform;
import org.omnetpp.simulation.model.cObject;
import org.omnetpp.simulation.ui.IEssentialsProvider;


/**
 *
 * @author Andras
 */
//XXX quite similar to InspectorRegistry, factor out some IScorable and ScorableRegistry as base?
public class EssentialsRegistry {
    public static final String ESSENTIALS_EXTENSIONPOINT_ID = "org.omnetpp.simulation.essentialsProviders";
    private List<IEssentialsProvider> providers = new ArrayList<IEssentialsProvider>();

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

    public EssentialsRegistry() {
        Platform.getExtensionRegistry().addListener(listener, ESSENTIALS_EXTENSIONPOINT_ID);
        providers = readConfiguredEssentialsProviders();
    }

    public void dispose() {
        Platform.getExtensionRegistry().removeListener(listener);
    }

    protected void reload() {
        providers = readConfiguredEssentialsProviders();
    }

    protected List<IEssentialsProvider> readConfiguredEssentialsProviders() {
        List<IEssentialsProvider> result = new ArrayList<IEssentialsProvider>();
        IConfigurationElement[] config = Platform.getExtensionRegistry().getConfigurationElementsFor(ESSENTIALS_EXTENSIONPOINT_ID);
        for (IConfigurationElement e : config) {
            if (e.isValid() && e.getName().equals("essentialsProvider")) {
                try {
                    //XXX ignore name and icon for now
                    //String name = e.getAttribute("name");
                    //Image image = SimulationPlugin.getCachedImage(e.getAttribute("icon"));
                    IEssentialsProvider provider = (IEssentialsProvider) e.createExecutableExtension("class");
                    result.add(provider);
                }
                catch (Exception ex) {
                    SimulationPlugin.logError("Error instantiating essentials provider", ex);
                }
            }
        }
        return result;
    }


    public IEssentialsProvider[] getEssentialsProviders() {
        return providers.toArray(new IEssentialsProvider[]{});
    }

    public IEssentialsProvider getBestEssentialsProvider(cObject object) {
        // note: we only try to call getScore() if there are at least two candidates; maybe this is not necessary, and then supports() and getScore() can be merged
        IEssentialsProvider bestProvider = null;
        int bestProviderScore = Integer.MIN_VALUE;
        for (IEssentialsProvider provider : providers) {
            if (provider.supports(object)) {
                if (bestProvider == null)
                    bestProvider = provider;
                else {
                    if (bestProviderScore == Integer.MIN_VALUE)
                        bestProviderScore = bestProvider.getScore(object);
                    int score = provider.getScore(object);
                    if (score > bestProviderScore) {
                        bestProvider = provider;
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
    public IEssentialsProvider[] getSupportedEssentialsProviders(cObject object) {
        return null;  //FIXME
    }

    /**
     * Sorted by score (best total score first)
     */
    public IEssentialsProvider[] getCommonSupportedEssentialsProviders(cObject[] objects) {
        return null;  //FIXME
    }
}
