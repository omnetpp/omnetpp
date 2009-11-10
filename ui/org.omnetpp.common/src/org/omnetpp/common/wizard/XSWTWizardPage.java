package org.omnetpp.common.wizard;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.FileUtils;

import com.swtworkbench.community.xswt.XSWT;

/**
 * XSWT-based custom wizard page, to be used with ContentTemplate.
 * 
 * @author Andras
 */
public class XSWTWizardPage extends WizardPage implements ICustomWizardPage {
    protected final ContentTemplate creatorTemplate;
    protected String xswtFileNameInfo; // for error messages only
    protected String xswtContent;  // the XML source
	protected String condition; // FreeMarker expr, used as isEnabled() condition
	protected Map<String,Control> widgetMap;
	protected CreationContext context; // to transfer context from populatePage() to createControl()
	
	public XSWTWizardPage(ContentTemplate creatorTemplate, String name, IFile xswtFile, String condition) throws IOException, CoreException {
		super(name);
        this.creatorTemplate = creatorTemplate;
        this.xswtFileNameInfo = xswtFile.getFullPath().toString();
        this.xswtContent = FileUtils.readTextFile(xswtFile.getContents());
		this.condition = condition;
	}

	public XSWTWizardPage(ContentTemplate creatorTemplate, String name, InputStream xswtInput, String fileNameInfo, String condition) throws IOException {
	    super(name);
	    if (xswtInput == null)
	        throw new IllegalArgumentException("InputStream is null -- file may not exist: " + fileNameInfo);
	    this.creatorTemplate = creatorTemplate;
	    this.xswtFileNameInfo = fileNameInfo;
        this.xswtContent = FileUtils.readTextFile(xswtInput);
	    this.condition = condition;
	}

	@SuppressWarnings("unchecked")
	public void createControl(Composite parent) {

        // WORKAROUND: wizard blows up horizontally if we put a text or label widget into it
        // with some long text. This is because the wizard window wants to be as big as the
        // preferred size (see Control.computeSize()) of the contents. The workaround is to
        // override computeSize() to return the size of the parent. This solution looks a bit harsh, 
        // but others like setting GridData.widthHint don't work.
        Composite composite = new Composite(parent, SWT.NONE) {
            @Override
            public Point computeSize(int whint, int hhint, boolean changed) {
                Point size = super.computeSize(whint, hhint, changed);
                size.x = getParent().getSize().x - 20;
                return size;
            }
        };
        composite.setLayout(new FillLayout());
        setControl(composite);
        
        ClassLoader oldClassLoader = XSWT.getClassLoader();
        XSWT.setClassLoader(creatorTemplate.getClassLoader()); // ensure access to custom widget classes
        Composite xswtHolder = null;
    	try {
    	    // instantiate XSWT form
    		xswtHolder = new Composite(composite, SWT.NONE);
    		xswtHolder.setLayout(new GridLayout());
    		widgetMap = (Map<String,Control>) XSWT.create(xswtHolder, new ByteArrayInputStream(xswtContent.getBytes())); 
    		
    		if (context != null) {
    			// do deferred populatePage() call
    			populatePage(context);
    			context = null; // to be safe
    		}
    		
    	} catch (Exception e) {
    	    xswtHolder.dispose();
    		widgetMap = new HashMap<String, Control>(); // fake it
    		displayError(composite, e); 
		}
    	finally {
            XSWT.setClassLoader(oldClassLoader);
    	}
	}
	
	public void displayError(Composite composite, Exception e) {
		String msg = "Error creating form " + (xswtFileNameInfo!=null ? "from "+xswtFileNameInfo : getName());
		CommonPlugin.logError(msg, e);
		
		// create error text widget
		Text errorText = new Text(composite, SWT.MULTI|SWT.READ_ONLY|SWT.WRAP);
		errorText.setText(msg + ":\n\n" + e.getClass().getSimpleName()+": "+e.getMessage());
	}
	
	public void populatePage(CreationContext context) {
		if (widgetMap == null) {
			// call too early -- defer it to createControl()
			this.context = context; 
		}
		else {
			// fill up controls with values from the context
			for (String key : widgetMap.keySet()) {
				Control control = widgetMap.get(key);
				Object value = context.getVariables().get(key);
				if (value != null) {
					try {
			            //FIXME String result = creatorTemplate.evaluate(value, context.getVariables());
					    
						SWTDataUtil.writeValueIntoControl(control, value);
					}
					catch (Exception e) {
						// NumberFormatException, ParseException (for date/time), something like that
						String message = "Cannot put value '"+value+"' into "+control.getClass().getSimpleName()+" control '"+key+"'";
						MessageDialog.openError(getShell(), "Error", "Wizard page: "+message);
						CommonPlugin.logError(message, e);
					}
				}
			}
		}
	}

	public void extractPageContent(CreationContext context) {
		// extract data from the XSWT form
		Assert.isNotNull(widgetMap);
		for (String widgetName : widgetMap.keySet()) {
			Control control = widgetMap.get(widgetName);
			Object value = SWTDataUtil.getValueFromControl(control);
			context.getVariables().put(widgetName, value);
		}
	}

	public boolean isEnabled(CreationContext context) {
		if (StringUtils.isBlank(condition))
			return true;
		
		// evaluate as FreeMarker expression
		try {
			String macro = "${(" + condition + ")?string}";
			String result = creatorTemplate.evaluate(macro, context.getVariables());
			return result.equals("true");
		} 
		catch (Exception e) {
			String message = "Cannot evaluate condition '"+condition+"' for page "+getName();
			MessageDialog.openError(getShell(), "Error", "Wizard: "+message+": " + e.getMessage());
			CommonPlugin.logError(message, e);
			return true;
		}
	}

	@Override
	public IWizardPage getPreviousPage() {
		if (getWizard() != null)
			getWizard().getPreviousPage(this); // as required by ICustomWizardPage
		return super.getPreviousPage();
	}
}