package org.omnetpp.ned.editor.wizards;

import java.io.ByteArrayInputStream;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.NedEditorPlugin;

/**
 * Wizard page for creating a new NED file
 *
 * @author rhornig, andras
 */
public class NewNEDComponentWizardPage extends WizardPage {
	private static final String COMMENT = "//\n// TODO auto-generated module\n//\n";

	private static final String[] NEDFILE_TEMPLATES = {
		"#PACKAGEDECL#"+COMMENT+"simple #NAME#\n{\n}\n",
		"#PACKAGEDECL#"+COMMENT+"module #NAME#\n{\n}\n",
		"#PACKAGEDECL#"+COMMENT+"network #NAME#\n{\n}\n"
	};

	private static final String CC_FILE = 
		"#include \"#NAME#.h\"\n" + 
		"\n" + 
		"Define_Module(#NAME#);\n" + 
		"\n" + 
		"void #NAME#::initialize()\n" + 
		"{\n" + 
		"	// TODO - Generated method body\n" + 
		"}\n" + 
		"\n" + 
		"void #NAME#::handleMessage(cMessage *msg)\n" + 
		"{\n" + 
		"	// TODO - Generated method body\n" + 
		"}\n"; 
	private static final String H_FILE = 
		"#ifndef __#UPPERCASENAME#_H__\n" + 
		"#define __#UPPERCASENAME#_H__\n" + 
		"\n" + 
		"#include <omnetpp.h>\n" + 
		"\n" + 
		"/**\n" + 
		" * TODO - Generated class\n" + 
		" */\n" + 
		"class #NAME# : public cSimpleModule\n" + 
		"{\n" + 
		"  protected:\n" + 
		"    virtual void initialize();\n" + 
		"    virtual void handleMessage(cMessage *msg);\n" + 
		"};\n" + 
		"\n" + 
		"#endif\n";

	public static enum Type {SIMPLE, COMPOUND, NETWORK} 

	private IWorkbench workbench;
	private Type type;
	private Text nameText;
	private TreeViewer folderTree;
	private IStructuredSelection selection;

	public NewNEDComponentWizardPage(IWorkbench workbench, IStructuredSelection selection, Type type ) {
		super("page1");
		this.type = type;
		this.selection = selection;
		this.workbench = workbench;

		switch (type) {
		case SIMPLE:
			setTitle("Create a Simple module");
			setDescription("This wizard allows you to create a new simple module");
			break;
		case COMPOUND:
			setTitle("Create a Compound module");
			setDescription("This wizard allows you to create a new compound module");
			break;
		case NETWORK:
			setTitle("Create a Network");
			setDescription("This wizard allows you to create a new network");
			break;
		}

		setImageDescriptor(ImageDescriptor.createFromFile(getClass(),"/icons/newnedfile_wiz.png"));
	}

	public void createControl(Composite parent) {
		Composite comp = new Composite(parent, SWT.NONE);
		comp.setLayout(new GridLayout(2, false));
		comp.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		createLabel(comp, "Folder:", 2);
		folderTree = new TreeViewer(comp);
		GridData layoutData = new GridData(SWT.FILL, SWT.FILL, true, false, 2, 1);
		layoutData.heightHint = 160;
		folderTree.getTree().setLayoutData(layoutData);
		folderTree.setLabelProvider(new WorkbenchLabelProvider());
		folderTree.setContentProvider(new WorkbenchContentProvider() {
			@Override
			public Object[] getChildren(Object element) {
				List<IContainer> filtered = new ArrayList<IContainer>();
				for (Object resource : super.getChildren(element))   
					if (resource instanceof IContainer && ((IContainer)resource).isAccessible()) // TODO filter for NEd source folders
						filtered.add((IContainer)resource);

				return filtered.toArray();
			}
		});
		folderTree.setInput(ResourcesPlugin.getWorkspace().getRoot());
		if (selection instanceof IStructuredSelection) {
			Object selObj = ((IStructuredSelection)selection).getFirstElement();
			if (selObj instanceof IContainer)
				setSelectedContainer((IContainer)selObj);
			if (selObj instanceof IAdaptable && ((IAdaptable)selObj).getAdapter(IResource.class) instanceof IContainer)
				setSelectedContainer((IContainer)((IAdaptable)selObj).getAdapter(IResource.class));
		}
		folderTree.addSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent event) {
				dialogChanged();
			}
		});

		createLabel(comp, "Name:", 1);
		nameText = createSingleText(comp, 1);
		nameText.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				dialogChanged();
			}
		});

		setControl(comp);
		setPageComplete(false);
	}

	public static Label createLabel(Composite parent, String text, int hspan) {
		Label l = new Label(parent, SWT.NONE);
		l.setFont(parent.getFont());
		l.setText(text);
		GridData gd = new GridData(GridData.FILL_HORIZONTAL);
		gd.horizontalSpan = hspan;
		gd.grabExcessHorizontalSpace = false;
		l.setLayoutData(gd);
		return l;
	}

	public static Text createSingleText(Composite parent, int hspan) {
		Text t = new Text(parent, SWT.SINGLE | SWT.BORDER);
		t.setFont(parent.getFont());
		GridData gd = new GridData(GridData.FILL_HORIZONTAL);
		gd.horizontalSpan = hspan;
		t.setLayoutData(gd);
		return t;
	}

	public static Button createPushButton(Composite parent, String label, int fill) {
		Button button = new Button(parent, SWT.PUSH);
		button.setFont(parent.getFont());
		button.setText(label);
		GridData gd = new GridData(fill);
		button.setLayoutData(gd);	
		return button;	
	}


	public boolean finish() {
		String name = nameText.getText().trim();
		IFile newNedFile = null;
		IFile newCCFile = null;
		IFile newHFile = null;

		try {
			// make a valid identifier
			name = StringUtils.makeValidIdentifier(name);
			// determine package
			newNedFile = getSelectedContainer().getFile(new Path(name+".ned"));
			String packagedecl = NEDResourcesPlugin.getNEDResources().getExpectedPackageFor(newNedFile);
			packagedecl = StringUtils.isNotEmpty(packagedecl) ? "package "+packagedecl+";\n\n" : "";

			// substitute name and package into the template
			String contents = NEDFILE_TEMPLATES[type.ordinal()].replaceAll("#NAME#", name);
			contents = contents.replaceAll("#PACKAGEDECL#", packagedecl);
			newNedFile.create(new ByteArrayInputStream(contents.getBytes()), true, null);

			// create CC and H files for simple modules
			if (type == Type.SIMPLE) {
				newCCFile = getSelectedContainer().getFile(new Path(name+".cc"));
				contents = CC_FILE.replaceAll("#NAME#", name);
				contents = contents.replaceAll("#UPPERCASENAME#", StringUtils.upperCase(name));
				newCCFile.create(new ByteArrayInputStream(contents.getBytes()), true, null);

				newHFile = getSelectedContainer().getFile(new Path(name+".h"));
				contents = H_FILE.replaceAll("#NAME#", name);
				contents = contents.replaceAll("#UPPERCASENAME#", StringUtils.upperCase(name));
				newHFile.create(new ByteArrayInputStream(contents.getBytes()), true, null);
			}
		} catch (CoreException e) {
			NedEditorPlugin.logError(e);
			return false;
		}


		// open the ned file
		// Since the file resource was created fine, open it for editing
		try {
			IWorkbenchWindow dwindow = workbench.getActiveWorkbenchWindow();
			IWorkbenchPage page = dwindow.getActivePage();
			if (page != null) {
				if (type == Type.SIMPLE) {
					IDE.openEditor(page, newCCFile, true);
					IDE.openEditor(page, newHFile, true);
				}
				IDE.openEditor(page, newNedFile, true);
			}
		} catch (org.eclipse.ui.PartInitException e) {
			NedEditorPlugin.logError(e);
			return false;
		}
		return true;
	}

	protected IContainer getSelectedContainer() {
		return (IContainer)((IStructuredSelection)folderTree.getSelection()).getFirstElement();
	}

	@SuppressWarnings("unchecked")
	protected void setSelectedContainer(IContainer container) {
		// expand to and select the specified container
		List itemsToExpand = new ArrayList();
		IContainer parent = container.getParent();
		while (parent != null) {
			itemsToExpand.add(0, parent);
			parent = parent.getParent();
		}
		folderTree.setExpandedElements(itemsToExpand.toArray());
		folderTree.setSelection(new StructuredSelection(container), true);
	}

	protected void dialogChanged() {
		String name = nameText.getText();
		if (StringUtils.isEmpty(name)) {
			setErrorMessage("Name cannot be empty");
			setPageComplete(false);
			return;
		}
		if (!name.equals(StringUtils.makeValidIdentifier(name))) {
			setErrorMessage("Not a valid identifier");
			setPageComplete(false);
			return;
		}
		IContainer cont = getSelectedContainer();

		if (cont == null) {
			setErrorMessage("Please select a container");
			setPageComplete(false);
			return;
		}

		IFile newNedFile = cont.getFile(new Path(name+".ned"));
		if (newNedFile.exists()) {
			setErrorMessage(newNedFile.getName()+" already exists");
			setPageComplete(false);
			return;
		}
		IFile newCCFile = cont.getFile(new Path(name+".cc"));
		if (newCCFile.exists()) {
			setErrorMessage(newCCFile.getName()+" already exists");
			setPageComplete(false);
			return;
		}
		IFile newHFile = cont.getFile(new Path(name+".h"));
		if (newHFile.exists()) {
			setErrorMessage(newHFile.getName()+" already exists");
			setPageComplete(false);
			return;
		}
		// TODO check if not a NED or C++ keyword
		setPageComplete(true);
		setErrorMessage(null);
	}



}
