/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.forms;

import java.util.Map;

import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.AnalysisObject;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Property;

/**
 * Factory for <code>ScaveObjectEditForm</code>s.
 *
 * @author tomi
 */
// XXX DELETE?
public class ScaveObjectEditFormFactory {

    private static ScaveObjectEditFormFactory instance;

    public static ScaveObjectEditFormFactory instance() {
        if (instance == null)
            instance = new ScaveObjectEditFormFactory();
        return instance;
    }

    /**
     * Returns true if this factory can create an edit form for this object
     */
    public boolean canCreateForm(AnalysisObject object) {
        if (object instanceof AnalysisItem || object instanceof InputFile || object instanceof Property)
            return true;
        return false;
    }

    /**
     * Creates a form containing all editable features of the object.
     */
    public IScaveObjectEditForm createForm(AnalysisObject object, Map<String,Object> formParameters, ResultFileManager manager) {
        return createForm(object, object.getParent(), -1, formParameters, manager);
    }

    /**
     * Creates a form containing all editable features of the object.
     * @param object the edited object
     * @param parent the parent node of the object where it is placed or will be placed
     */
    public IScaveObjectEditForm createForm(AnalysisObject object, AnalysisObject parent, int index, Map<String,Object> formParameters, ResultFileManager manager) {

        /* if (object instanceof BarChart)
            return new BarChartEditForm((BarChart)object, formParameters, manager);
        else if (object instanceof ScatterChart)
            return new ScatterChartEditForm((ScatterChart)object, formParameters, manager);
        else if (object instanceof LineChart)
            return new LineChartEditForm((LineChart)object, formParameters, manager);
        else if (object instanceof HistogramChart)
            return new HistogramChartEditForm((HistogramChart)object, formParameters, manager);
        else */ if (object instanceof Chart)
            return new ChartEditForm((Chart)object, formParameters, manager);
        else if (object instanceof InputFile)
            return new InputFileEditForm((InputFile)object, parent);
        else if (object instanceof Property)
            return new PropertyEditForm((Property)object, parent);
        else
            throw new IllegalArgumentException("no edit form for "+object.getClass().getSimpleName());
    }
}
