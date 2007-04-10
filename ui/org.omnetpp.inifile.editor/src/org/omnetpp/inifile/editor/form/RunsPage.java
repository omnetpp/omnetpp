package org.omnetpp.inifile.editor.form;

import java.util.HashMap;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeLabelProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Inifile editor page to manage the [Run X] sections in the file.
 * 
 * @author Andras
 */
public class RunsPage extends FormPage {
	private static Font titleFont = new Font(null, "Arial", 10, SWT.BOLD);
	private TreeViewer treeViewer;
	
	public RunsPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);
		GridLayout gridLayout = new GridLayout(1,false);
		gridLayout.verticalSpacing = 0;
		setLayout(gridLayout);
		
		// title area (XXX a bit ugly -- re-think layout)
		Composite titleArea = new Composite(this, SWT.BORDER);
		GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		gridData.heightHint = 40;
		titleArea.setLayoutData(gridData);
		titleArea.setBackground(ColorFactory.asColor("white"));
		titleArea.setLayout(new GridLayout(2, false));
		Label imageLabel = new Label(titleArea, SWT.NONE);
		imageLabel.setImage(ImageFactory.getImage(ImageFactory.MODEL_IMAGE_FOLDER)); //XXX 
		imageLabel.setBackground(ColorFactory.asColor("white"));
		Label title = new Label(titleArea, SWT.NONE);
		title.setText("Manage Run Configurations");
		title.setFont(titleFont);
		title.setBackground(ColorFactory.asColor("white"));
		title.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, true));
		new Label(this, SWT.NONE);

		Label l = new Label(this, SWT.NONE);  //XXX
		l.setText("Section inheritance:");
		
		treeViewer = new TreeViewer(this, SWT.BORDER);
		treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		treeViewer.setLabelProvider(new GenericTreeLabelProvider(new LabelProvider()));
		treeViewer.setContentProvider(new GenericTreeContentProvider());
		reread();
	}
	
	@Override
	public void reread() {
		IInifileDocument doc = getInifileDocument();

		// create root node
		GenericTreeNode rootNode = new GenericTreeNode("root");
		GenericTreeNode generalSectionNode = new GenericTreeNode("[General]");
		rootNode.addChild(generalSectionNode);

		// build tree
		HashMap<String,GenericTreeNode> nodes = new HashMap<String, GenericTreeNode>();
		for (String sectionName : doc.getSectionNames()) {
			if (sectionName.startsWith("Config ")) {
				GenericTreeNode node = getOrCreate(nodes, sectionName, false);
				String extendsName = doc.getValue(sectionName, "extends");
				if (extendsName == null) {
					// no "extends=...": falls back to [General]
					generalSectionNode.addChild(node);
				}
				else {
					// add as child to the section it extends
					String extendsSectionName = "Config "+extendsName;
					if (doc.containsSection(extendsSectionName)) {
						GenericTreeNode extendsSectionNode = getOrCreate(nodes, extendsSectionName, false);
						extendsSectionNode.addChild(node);
					}
					else {
						// error: extends a nonexisting section. add it anyway
						GenericTreeNode extendsSectionNode = getOrCreate(nodes, extendsSectionName, true);
						if (extendsSectionNode.getParent() == null)
							generalSectionNode.addChild(extendsSectionNode);
						extendsSectionNode.addChild(node);
					}
				}
			}
		}
		treeViewer.setInput(rootNode);
		treeViewer.expandAll();
	}

	private GenericTreeNode getOrCreate(HashMap<String, GenericTreeNode> nodes, String sectionName, boolean isMissingSection) {
		GenericTreeNode node = nodes.get(sectionName);
		if (node==null) {
			node = new GenericTreeNode("["+sectionName+"]"+(isMissingSection?" (missing)":""));
			nodes.put(sectionName, node);
		}
		return node;
	}

	@Override
	public void commit() {
		//TODO
	}
}
