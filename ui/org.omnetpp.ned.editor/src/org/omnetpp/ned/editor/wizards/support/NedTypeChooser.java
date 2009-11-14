package org.omnetpp.ned.editor.wizards.support;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.IWidgetAdapter;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.core.ui.misc.NedTypeSelectionDialog;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;

/**
 * A control for selecting a NED type. Implemented as a Composite 
 * with a single-line Text and a Browse button. Plus, optionally
 * it can display a label below the text field, saying whether the 
 * NED type exists or not.
 * 
 * Does not replicate all the zillion methods of the Text class;
 * rather, it exposes the internal Text widget so that it can
 * be manipulated directly.
 * 
 * @author Andras
 */
public class NedTypeChooser extends Composite implements IWidgetAdapter {
    public static final int MODULE = 1;
    public static final int SIMPLE_MODULE = 2;
    public static final int COMPOUND_MODULE = 4;
    public static final int MODULEINTERFACE = 8;
    public static final int CHANNEL = 16;
    public static final int CHANNELINTERFACE = 32;
    public static final int NETWORK = 64;
    
	private Text text;
	private Button browseButton;
	private Label message;
	
	private int acceptedTypes = ~0; // binary OR of the above integer constants
	private String existsText = null;
	private String notExistsText = null;  // actually, "exists and is an accepted type"

	public NedTypeChooser(Composite parent, int style) {
		super(parent, style);
		GridLayout layout = new GridLayout(2,false);
		layout.marginHeight = layout.marginWidth = 0;
		setLayout(layout);
		
		text = new Text(this, SWT.SINGLE|SWT.BORDER);
		text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));
		browseButton = new Button(this, SWT.PUSH);
		browseButton.setText("Browse...");

		message = new Label(this, SWT.NONE);
		message.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));
		updateLabelsVisibility();
		
		text.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                textModified();
            }});
		browseButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				browse();
			}
		});
	}

    public String getExistsText() {
        return existsText;
    }

    public void setExistsText(String existsText) {
        this.existsText = existsText;
        updateLabelsVisibility();
    }

    public String getNotExistsText() {
        return notExistsText;
    }

    public void setNotExistsText(String notExistsText) {
        this.notExistsText = notExistsText;
        updateLabelsVisibility();
    }

    private void updateLabelsVisibility() {
        boolean visible = existsText!=null || notExistsText!=null;
        ((GridData)message.getLayoutData()).exclude = !visible;
    }

    /**
	 * Needed because for some reason XSWT doesn't like the int version...
	 */
    public void setAcceptedTypes(String filter) {
        int filt = 0;
        for (String type : filter.split("\\|")) {
            if (type.equals("MODULE")) filt |= MODULE;
            else if (type.equals("SIMPLE_MODULE")) filt |= SIMPLE_MODULE;
            else if (type.equals("COMPOUND_MODULE")) filt |= COMPOUND_MODULE;
            else if (type.equals("MODULEINTERFACE")) filt |= MODULEINTERFACE;
            else if (type.equals("CHANNEL")) filt |= CHANNEL;
            else if (type.equals("CHANNELINTERFACE")) filt |= CHANNELINTERFACE;
            else if (type.equals("NETWORK")) filt |= NETWORK;
            else throw new IllegalArgumentException("Unrecognized NED type flag " + type);
        }
        acceptedTypes = filt;
    }

    public void setAcceptedTypes(int filter) {
	    this.acceptedTypes = filter;   
	}

	public int getAcceptedTypes() {
	    return acceptedTypes;
	}
	
	protected void browse() {
        // pop up a chooser dialog
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        NedTypeSelectionDialog dialog = new NedTypeSelectionDialog(window.getShell());
        dialog.setMessage("Select NED type:");
        dialog.setTitle("Select NED Type");

        // need to filter its contents according to the filter flags
        List<INEDTypeInfo> types = new ArrayList<INEDTypeInfo>();
        for (INEDTypeInfo nedType : NEDResourcesPlugin.getNEDResources().getNedTypesFromAllProjects())
            if (isAcceptedType(nedType))
                types.add(nedType);
        dialog.setElements(types.toArray());

        if (dialog.open() == ListDialog.OK) {
            INEDTypeInfo component = dialog.getResult()[0];
            text.setText(component.getFullyQualifiedName());
            text.selectAll();
        }
	}

	protected void textModified() {
	    String name = text.getText();
	    Set<INEDTypeInfo> nedTypes = NEDResourcesPlugin.getNEDResources().getNedTypesFromAllProjects(name);
	    boolean found = false;
	    for (INEDTypeInfo nedType : nedTypes)
	        if (isAcceptedType(nedType))
	            found = true;
	    String newText = StringUtils.nullToEmpty(found ? existsText : notExistsText);
	    if (!message.getText().equals(newText))
	        message.setText(newText);
	}
	
	protected boolean isAcceptedType(INEDTypeInfo nedType) {
	    if ((acceptedTypes & MODULE)!=0 && NEDResources.MODULE_FILTER.matches(nedType)) return true;
	    if ((acceptedTypes & SIMPLE_MODULE)!=0 && NEDResources.SIMPLE_MODULE_FILTER.matches(nedType)) return true;
	    if ((acceptedTypes & COMPOUND_MODULE)!=0 && NEDResources.COMPOUND_MODULE_FILTER.matches(nedType)) return true;
	    if ((acceptedTypes & MODULEINTERFACE)!=0 && NEDResources.MODULEINTERFACE_FILTER.matches(nedType)) return true;
	    if ((acceptedTypes & CHANNEL)!=0 && NEDResources.CHANNEL_FILTER.matches(nedType)) return true;
	    if ((acceptedTypes & CHANNELINTERFACE)!=0 && NEDResources.CHANNELINTERFACE_FILTER.matches(nedType)) return true;
	    if ((acceptedTypes & NETWORK)!=0 && NEDResources.NETWORK_FILTER.matches(nedType)) return true;
	    return false;
	}
	
    public String getNedName() {
        return text.getText();
    }

    public void setNedName(String file) {
        text.setText(file);
        text.selectAll();
    }

	public Text getTextControl() {
		return text;
	}

	public Button getBrowseButton() {
		return browseButton;
	}

	/**
	 * Adapter interface.
	 */
	public boolean supportsControl(Control control) {
		return control instanceof NedTypeChooser; 
	}

	/**
	 * Adapter interface.
	 */
	public Object getValueFromControl(Control control) {
		return ((NedTypeChooser)control).getNedName();
	}

	/**
	 * Adapter interface.
	 */
	public void writeValueIntoControl(Control control, Object value) {
	    String nedTypeName = value instanceof INEDTypeInfo ? ((INEDTypeInfo)value).getFullyQualifiedName() : value.toString();
		((NedTypeChooser)control).setNedName(nedTypeName);
	}

}
