package org.omnetpp.scave.editors.ui;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;

public class DatasetPage extends ScaveEditorPage {
	private TreeViewer datasetTreeViewer;
	private Dataset dataset; // backreference to the model object we operate on

	public DatasetPage(Composite parent, ScaveEditor scaveEditor, Dataset dataset) {
		super(parent, SWT.V_SCROLL | SWT.H_SCROLL, scaveEditor);
		this.dataset = dataset;
		initialize();
	}

	public void updatePage(Notification notification) {
		if (ScaveModelPackage.eINSTANCE.getDataset_Name().equals(notification.getFeature())) {
			setPageTitle("Dataset: " + dataset.getName());
			setFormTitle("Dataset: " + dataset.getName());
		}
	}

	public TreeViewer getDatasetTreeViewer() {
		return datasetTreeViewer;
	}

	private void initialize() {
		// set up UI
		String datasetName = StringUtils.isEmpty(dataset.getName()) ? "<unnamed>" : dataset.getName();
		setPageTitle("Dataset: " + datasetName);
		setFormTitle("Dataset: " + datasetName);
		setExpandHorizontal(true);
		setExpandVertical(true);
		//setDelayedReflow(false);
		setBackground(ColorFactory.WHITE);
		getBody().setLayout(new GridLayout(2, false));
		
		Label label = new Label(getBody(), SWT.WRAP);
		label.setBackground(getBackground());
		label.setText("Here you can edit the dataset. " +
	      "The dataset allows you to create a subset of the input data and work with it. "+
	      "Datasets may include processing steps, and one dataset can serve as input for another.");
		label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		((GridData)label.getLayoutData()).horizontalSpan = 2;
		
		// create dataset treeviewer with buttons
		datasetTreeViewer = new TreeViewer(getBody(), SWT.BORDER | SWT.MULTI);
		datasetTreeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

		createPalette(getBody(), false);
		
		// configure dataset treeviewer
		TreeViewer treeViewer = getDatasetTreeViewer();
		scaveEditor.configureTreeViewer(treeViewer);
		treeViewer.setInput(dataset);
	}

	@Override
	public boolean gotoObject(Object object) {
		if (object instanceof EObject) {
			EObject eobject = (EObject)object;
			if (EcoreUtil.isAncestor(dataset, eobject)) {
				if (getDatasetTreeViewer() != null)
					getDatasetTreeViewer().reveal(object);
				return true;
			}
		}
		return false;
	}
}
