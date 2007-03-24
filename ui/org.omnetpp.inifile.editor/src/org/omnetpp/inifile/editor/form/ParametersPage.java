package org.omnetpp.inifile.editor.form;

import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * For editing module parameters.
 * 
 * @author Andras
 */
//XXX for now it only edits the [Parameters] section, should be extended to run configs as well
public class ParametersPage extends FormPage {
	private TableViewer tableViewer;
	
	public ParametersPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);

		// layout: 2x2 grid: (label, dummy) / (table, buttonGroup)
		setLayout(new GridLayout(2,false));
		Label label = new Label(this, SWT.NONE);
		label.setText("Parameter Assignments");
		new Label(this, SWT.NONE); // dummy
		Table table = new Table(this, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION | SWT.VIRTUAL);
		table.setLinesVisible(true);
		table.setHeaderVisible(true);
		table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		addTableColumn(table, "Key", 200);
		addTableColumn(table, "Value", 150);
		addTableColumn(table, "Comment", 200);
		tableViewer = new TableViewer(table);
		tableViewer.setContentProvider(new ArrayContentProvider());
		tableViewer.setLabelProvider(new ITableLabelProvider() {
			public Image getColumnImage(Object element, int columnIndex) {
				return null;
			}

			public String getColumnText(Object element, int columnIndex) {
				String key = (String) element;
				switch (columnIndex) {
				case 0: return key;
				case 1: return getInifileDocument().getValue("Parameters", key); 
				case 2: return getInifileDocument().getComment("Parameters", key);
				default: throw new IllegalArgumentException();
				}
			}

			public void addListener(ILabelProviderListener listener) {
				//XXX
			}

			public void dispose() {
			}

			public boolean isLabelProperty(Object element, String property) {
				return true;
			}

			public void removeListener(ILabelProviderListener listener) {
				//XXX
			}
		});

		Composite buttonGroup = new Composite(this, SWT.NONE);
		buttonGroup.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, true));
		buttonGroup.setLayout(new GridLayout(1,false));
		
		Button addButton = createButton(buttonGroup, "Add...");
		Button editButton = createButton(buttonGroup, "Edit...");
		Button removeButton = createButton(buttonGroup, "Remove");
		Button upButton = createButton(buttonGroup, "Up");
		Button downButton = createButton(buttonGroup, "Down");
		
		reread();
		dumpUnboundParameters(); //XXX temp
	}

	private TableColumn addTableColumn(Table table, String label, int width) {
		TableColumn column = new TableColumn(table, SWT.NONE);
		column.setText(label);
		column.setWidth(width);
		return column;
	}

	private Button createButton(Composite buttonGroup, String label) {
		Button button = new Button(buttonGroup, SWT.PUSH);
		button.setText(label);
		button.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		return button;
	}
	
	@Override
	public void reread() {
		//XXX use keys like: "section/key"
		IInifileDocument doc = getInifileDocument();
		//XXX get only dotted keys! if (key.contains("."))...
		//TODO should introduce rule: parameter refs must contain a dot; to check this in C++ code as well
		tableViewer.setInput(doc.getKeys("Parameters")); //XXX or empty array if there's no such section
		tableViewer.refresh();
	}

	@Override
	public void commit() {
		//XXX todo
	}

	private void dumpUnboundParameters() {
		IInifileDocument doc = getInifileDocument();
		//XXX consider changing the return type of NEDResourcesPlugin.getNEDResources() to INEDTypeResolver
		INEDTypeResolver nedResources = NEDResourcesPlugin.getNEDResources();
		
		String networkName = doc.getValue("General", "network");
		if (networkName == null) {
			System.out.println("no network name specified (no [General]/network= setting)");
			return;
		}
		//XXX todo: parse networkName, i.e. remove quotes
		
		dumpModule(networkName, networkName, nedResources, doc);
	}

	private void dumpModule(String moduleTypeName, String moduleFullPath, INEDTypeResolver nedResources, IInifileDocument doc) {
		System.out.println("- "+moduleFullPath+"\t\t\t("+moduleTypeName+")");

		// dig out type info (NED declaration)
		if (isEmpty(moduleTypeName)) {
			System.out.println("*** no module type for "+moduleFullPath);
			return;
		}
		INEDTypeInfo moduleType = nedResources.getComponent(moduleTypeName);
		if (moduleType == null) {
			System.out.println("*** no such module type: "+moduleTypeName);
			return;
		}

		dumpParameters(moduleType, moduleFullPath, doc);
		
		for (NEDElement node : moduleType.getSubmods().values()) {
			SubmoduleNode submodule = (SubmoduleNode) node;

			// produce submodule name; if vector, append [*]
			String submoduleName = submodule.getName();
			if (!isEmpty(submodule.getVectorSize())) //XXX what if parsed expressions are in use?
				submoduleName += "[*]";
			
			// produce submodule type: if "like", use like type
			//XXX should try to evaluate "like" expression and use result as type (if possible)
			String submoduleType = submodule.getType();
			if (isEmpty(submoduleType))
				submoduleType = submodule.getLikeType();
			
			// recursive call
			dumpModule(submoduleType, moduleFullPath+"."+submoduleName, nedResources, doc);
		}
	}
	
	private static void dumpParameters(INEDTypeInfo moduleType, String moduleFullPath, IInifileDocument doc) {
		for (NEDElement node : moduleType.getParamValues().values()) {
			ParamNode param = (ParamNode) node;

			String paramFullPath = moduleFullPath + "." + param.getName();
			String iniKey = lookupParameter(paramFullPath, doc, "Parameters");
			String value = doc.getValue("Parameters", iniKey);
			
			System.out.println("\t"+param.getName()+"= NED:"+param.getValue()+",  INI:"+value);
		}
	}

	/**
	 * Given a parameter's fullPath, returns the key of the matching
	 * inifile entry in the given section, or null if it's not 
	 * in the inifile. 
	 */
	private static String lookupParameter(String paramFullPath, IInifileDocument doc, String section) {
		String[] keys = doc.getKeys(section);
		for (String key : keys)
			if (new PatternMatcher(key, true, true, true).matches(paramFullPath))
				return key;
		return null;
	}

	private static boolean isEmpty(String string) {
		return string==null || "".equals(string);
	}
}
