/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import static org.omnetpp.scave.engine.ResultItemField.FILE;
import static org.omnetpp.scave.engine.RunAttribute.CONFIGNAME;
import static org.omnetpp.scave.engine.RunAttribute.RUNNUMBER;

import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.Callable;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.datatable.IDataControl;
import org.omnetpp.scave.editors.ui.DatasetSelectionDialog;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Active on the "Browse Data" page. It adds the selected elements to
 * a dataset. The dataset selection dialog also allows creating
 * a new dataset.
 *
 * @author Andras
 */
public class AddSelectedToDatasetAction extends AbstractScaveAction {

    public AddSelectedToDatasetAction() {
        setText("Add Selected Data to Dataset...");
        setToolTipText("Add selected data to dataset");
        setImageDescriptor(ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_ADDSELECTEDTODATASET));
    }

    @Override
    protected void doRun(final ScaveEditor editor, IStructuredSelection selection) {
        FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
        if (activePanel == null)
            return;

        final IDataControl control = activePanel.getDataControl();
        String filterPattern = activePanel.getFilter().getFilterPattern();
        ResultType type = editor.getBrowseDataPage().getActivePanelType();

        boolean addByFilter = false;
        if (control.getSelectionCount() == control.getItemCount()) {
            addByFilter = MessageDialog.openQuestion(editor.getSite().getShell(),
                            "Add all",
                            "All items are selected. " +
                            "It is more terse to specify them by the current filter expression, " +
                            "than adding them one-by-one.\n\n" +
                            "Would you like to use the current filter expression?");
        }

        DatasetSelectionDialog dlg = new DatasetSelectionDialog(editor);
        if (dlg.open() == Window.OK) {
            Dataset dataset = (Dataset) dlg.getFirstResult();
            if (dataset != null) {
                // add the Add node before the first chart or group,
                // so they are affected by this action
                int index = 0;
                List<DatasetItem> items = dataset.getItems();
                for (; index < items.size(); ++index) {
                    DatasetItem item = items.get(index);
                    if (item instanceof Chart || item instanceof Group)
                        break;
                }

                Collection<Add> addItems;
                if (addByFilter) {
                    addItems = Collections.singletonList(
                                ScaveModelUtil.createAdd(filterPattern, type));
                }
                else {
                    ResultFileManager manager = control.getResultFileManager();
                    addItems = ResultFileManager.callWithReadLock(manager, new Callable<Collection<Add>>() {
                        public Collection<Add> call() {
                            return ScaveModelUtil.createAdds(control.getSelectedItems(),
                                                             new String[] { FILE, CONFIGNAME, RUNNUMBER });
                        }
                    });
                }

                Command command = AddCommand.create(
                            editor.getEditingDomain(),
                            dataset,
                            ScaveModelPackage.eINSTANCE.getDataset_Items(),
                            addItems,
                            index);
                editor.executeCommand(command);

                // show the dataset
                editor.showDatasetsPage(); // or: editor.openDataset(dataset);
                editor.setSelection(new StructuredSelection(addItems.toArray()));
            }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
        if (activePanel != null) {
            IDataControl control = activePanel.getDataControl();
            return control.getSelectionCount() > 0;
        }
        return false;
    }
}
