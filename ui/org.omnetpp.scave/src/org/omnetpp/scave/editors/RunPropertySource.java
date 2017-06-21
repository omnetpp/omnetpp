package org.omnetpp.scave.editors;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.OrderedKeyValueList;
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

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        IPropertyDescriptor[] attrs = makeDescriptors(run.getAttributes().keys().toArray(), "@", "Attributes");
        IPropertyDescriptor[] itervars = makeDescriptors(run.getIterationVariables().keys().toArray(), "$", "Iteration Variables");
        IPropertyDescriptor[] moduleParams = makeDescriptors(getParamAssignmentKeys(run), "%", "Parameter Assignments");
        return ArrayUtils.addAll(MAIN_PROPERTY_DESCS, ArrayUtils.addAll(attrs, ArrayUtils.addAll(itervars, moduleParams)));
    }

    private static String[] getParamAssignmentKeys(Run run) {
        OrderedKeyValueList paramAssignments = run.getParamAssignments();
        int n = (int)paramAssignments.size();
        String[] result = new String[n];
        for (int i = 0; i < n; i++)
            result[i] = paramAssignments.get(i).getFirst();
        return result;
    }

    @Override
    public Object getPropertyValue(Object propertyId) {
        if (propertyId instanceof String && propertyId.toString().charAt(0)=='@')
            return run.getAttribute(propertyId.toString().substring(1));
        if (propertyId instanceof String && propertyId.toString().charAt(0)=='$')
            return run.getIterationVariable(propertyId.toString().substring(1));
        if (propertyId instanceof String && propertyId.toString().charAt(0)=='%')
            return run.getParamAssignment(propertyId.toString().substring(1));

        if (propertyId.equals(PROP_RUN_NAME)) return run.getRunName();
        if (propertyId.equals(PROP_RUN_NUMBER)) return run.getAttribute("runnumber");

        return null;
    }

    @Override
    public boolean isPropertySet(Object id) {
        return false;
    }

    @Override
    public void resetPropertyValue(Object id) {
    }

    @Override
    public void setPropertyValue(Object id, Object value) {
    }

    @Override
    public Object getEditableValue() {
        return null; // not editable
    }
}
