package org.omnetpp.scave.editors;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.eclipse.ui.views.properties.FilePropertySource;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.ResultFile;

/**
 * Property source for run
 *
 * @author levy
 */
public class ResultFilePropertySource extends FilePropertySource {
    // display labels as well as property IDs

    public static final String[] MAIN_PROPERTY_IDS = {
    };

    protected static final IPropertyDescriptor[] MAIN_PROPERTY_DESCS = makeDescriptors(MAIN_PROPERTY_IDS, "", "Main");

    private static IPropertyDescriptor[] makeDescriptors(String ids[], String prefix, String category) {
        IPropertyDescriptor[] descs = new IPropertyDescriptor[ids.length];
        for (int i = 0; i < descs.length; i++)
            descs[i] = new ReadonlyPropertyDescriptor(prefix+ids[i], StringUtils.capitalize(ids[i]), category);
        return descs;
    }

    public ResultFilePropertySource(ResultFile resultFile) {
        super(ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(resultFile.getFilePath())));
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        return ArrayUtils.addAll(MAIN_PROPERTY_DESCS, super.getPropertyDescriptors());
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
