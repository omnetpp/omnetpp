package org.omnetpp.cdt.ui;

import org.eclipse.core.resources.IContainer;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.cdt.makefile.MakemakeOptions;

/**
 * @author Andras
 */
public class MakemakeOptionsDialog extends TitleAreaDialog {
    protected IContainer folder;
    protected MakemakeOptions options; // initial/result
    
    // controls
    protected MakemakeOptionsPanel optionsPanel;
    
    public MakemakeOptionsDialog(Shell parentShell, IContainer folder, MakemakeOptions options) {
        super(parentShell);
        this.folder = folder;
        this.options = options;
    }

    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        shell.setText("Makemake Options");
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        setTitle("Makemake Options");
        setMessage("Makefile generation (Makemake) options for " + folder.getFullPath().toString() + ".");
        
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1,false));
        
        optionsPanel = new MakemakeOptionsPanel(composite, SWT.NONE);
        optionsPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        optionsPanel.populate(folder, options, "TODO", "TODO"); //FIXME
        return optionsPanel;
    }
    
    @Override
    protected void okPressed() {
        // save result before dialog gets disposed
        options = optionsPanel.getResult();
        super.okPressed();
    }

    public MakemakeOptions getResult() {
        return options;
    }
}
