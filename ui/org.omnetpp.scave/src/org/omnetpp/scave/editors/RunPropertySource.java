package org.omnetpp.scave.editors;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.Run;

/**
 * Property source for run
 *
 * @author levy
 */
public class RunPropertySource implements IPropertySource {
    // display labels as well as property IDs
    public static final String PROP_RUN_NAME = "Run Name";
    public static final String PROP_RUN_NUMBER = "Run Number";

    public static final String[] MAIN_PROPERTY_IDS = {
        PROP_RUN_NAME, PROP_RUN_NUMBER
    };

    protected static final IPropertyDescriptor[] MAIN_PROPERTY_DESCS = makeDescriptors(MAIN_PROPERTY_IDS, "", "Main");

    private static IPropertyDescriptor[] makeDescriptors(String ids[], String prefix, String category) {
        IPropertyDescriptor[] descs = new IPropertyDescriptor[ids.length];
        for (int i = 0; i < descs.length; i++)
            descs[i] = new ReadonlyPropertyDescriptor(prefix+ids[i], StringUtils.capitalize(ids[i]), category);
        return descs;
    }

    private Run run;

    public RunPropertySource(Run run) {
        this.run = run;
    }

    public IPropertyDescriptor[] getPropertyDescriptors() {
        IPropertyDescriptor[] attrs = makeDescriptors(run.getAttributes().keys().toArray(), "@", "Attributes");
        IPropertyDescriptor[] moduleParams = makeDescriptors(run.getModuleParams().keys().toArray(), "%", "Attributes");
        return (IPropertyDescriptor[])ArrayUtils.addAll(MAIN_PROPERTY_DESCS, ArrayUtils.addAll(attrs, moduleParams));
    }

    public Object getPropertyValue(Object propertyId) {
        // run attribute
        if (propertyId instanceof String && propertyId.toString().charAt(0)=='@')
            return run.getAttribute(propertyId.toString().substring(1));
        // module params
        if (propertyId instanceof String && propertyId.toString().charAt(0)=='%')
            return run.getModuleParam(propertyId.toString().substring(1));

        if (propertyId.equals(PROP_RUN_NAME)) return run.getRunName();
        if (propertyId.equals(PROP_RUN_NUMBER)) return run.getRunNumber();

        return null;
    }

    public boolean isPropertySet(Object id) {
        return false;
    }

    public void resetPropertyValue(Object id) {
    }

    public void setPropertyValue(Object id, Object value) {
    }

    public Object getEditableValue() {
        return null; // not editable
    }
}
