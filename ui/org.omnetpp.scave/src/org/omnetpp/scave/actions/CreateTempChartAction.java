/*--------------------------------------------------------------*

  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.concurrent.Callable;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ResultType;

/**
 * Creates a temporary chart from the selection on the BrowseDataPage, and opens it.
 */
public class CreateTempChartAction extends AbstractScaveAction {
    static int counter = 0;

    public CreateTempChartAction() {
        setText("Plot");
        setToolTipText("Plot");
        setImageDescriptor(ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_PLOT));
    }

    @Override
    protected void doRun(ScaveEditor editor, final IStructuredSelection selection) {
        IDListSelection idListSelection = (IDListSelection)selection;
        ResultFileManager manager = idListSelection.getResultFileManager();
        if (idListSelection.getScalarsCount() != 0) {
            IDList idList = IDList.fromArray(idListSelection.getScalarIDs());
            openChart(editor, manager, ResultType.SCALAR_LITERAL, idList);
        }
        if (idListSelection.getVectorsCount() != 0) {
            IDList idList = IDList.fromArray(idListSelection.getVectorIDs());
            openChart(editor, manager, ResultType.VECTOR_LITERAL, idList);
        }
        if (idListSelection.getHistogramsCount() != 0) {
            IDList idList = IDList.fromArray(idListSelection.getHistogramIDs());
            openChart(editor, manager, ResultType.HISTOGRAM_LITERAL, idList);
        }
    }

    protected void openChart(final ScaveEditor editor, final ResultFileManager manager, final ResultType type, final IDList idList) {
        Chart chart = ResultFileManager.callWithReadLock(manager, new Callable<Chart>() {
            @Override
            public Chart call() {
                System.out.println("TODO implemrnt chart creation!");
                return null;
//                Dataset dataset = ScaveModelUtil.createTemporaryDataset("dataset", idList, new String[] { FILE, CONFIGNAME, RUNNUMBER }, manager);
//                String chartTitle = StringUtils.defaultIfEmpty(DatasetManager.defaultTitle(ScaveModelUtil.getResultItems(idList, manager)), "temp" + ++counter);
//                Chart chart = ScaveModelUtil.createChart(chartTitle, type);
//                dataset.getItems().add(chart);
//                Command command = AddCommand.create(editor.getEditingDomain(), editor.getTempAnalysis().getDatasets(), ScaveModelPackage.eINSTANCE.getDatasets_Datasets(), dataset);
//
//                // Do not use the CommandStack of the editor, because it would make it dirty
//                // and the Add command undoable from the UI.
//                // It's safe to use a separate command stack, because the operations on the
//                // temporary resource does not interfere with the operations on the persistent resource.
//                CommandStack commandStack = new BasicCommandStack();
//                commandStack.execute(command);
//                commandStack.flush();
//                return chart;
            }
        });

        editor.openChart(chart);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return selection instanceof IDListSelection && !selection.isEmpty();
    }
}
