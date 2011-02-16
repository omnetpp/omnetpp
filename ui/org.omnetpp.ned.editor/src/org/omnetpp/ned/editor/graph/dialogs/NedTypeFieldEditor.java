package org.omnetpp.ned.editor.graph.dialogs;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.core.ui.misc.NedTypeSelectionDialog;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;

/**
 * Represents a NED type selection compound widget in the Properties dialog
 * 
 * @author Andras
 */
public class NedTypeFieldEditor implements IFieldEditor {
    private Composite composite;
    private Text text;
    private Button button;
    private boolean grayed = false; 
    private INedTypeResolver.IPredicate typeFilter; // which types to offer / allow
    private IProject contextProject;
    private INedTypeInfo enclosingNedType; // if non-null, offer its inner types too
    private boolean multiple;
    private ControlDecoration problemDecoration;

    
    protected class NedTypeContentProposalProvider extends ContentProposalProvider {
        public NedTypeContentProposalProvider(boolean useWholePrefix) {
            super(useWholePrefix);
        }

        @Override
        protected List<IContentProposal> getProposalCandidates(String prefix) {
            INedResources nedResources = NedResourcesPlugin.getNedResources();
            Assert.isNotNull(typeFilter, "setTypeFilter() must have been called with non-null arg");
            
            // add suggestions both in "package.name.SimpleName" and "SimpleName (package.name)" syntax 
            Set<String> qnames = nedResources.getNedTypeQNames(typeFilter, contextProject);
            if (enclosingNedType != null)
                for (INedTypeElement innerType : enclosingNedType.getInnerTypes().values())
                    if (typeFilter.matches(innerType.getNedTypeInfo()))
                        qnames.add(innerType.getNedTypeInfo().getFullyQualifiedName());

            List<String> friendlyTypeNames = new ArrayList<String>();
            for (String qname : qnames)
                friendlyTypeNames.add(NedElementUtilEx.qnameToFriendlyTypeName(qname));
            List<IContentProposal> part1 = sort(toProposals(friendlyTypeNames.toArray(new String[]{})));

            List<String> qnamesMinusSimpleNames = new ArrayList<String>(); // simple names are already listed in part1
            for (String qname : qnames)
                if (qname.indexOf('.') != -1)
                    qnamesMinusSimpleNames.add(qname);
            List<IContentProposal> part2 = sort(toProposals(qnamesMinusSimpleNames.toArray(new String[]{})));  

            List<IContentProposal> result = new ArrayList<IContentProposal>();
            result.addAll(part1);
            result.addAll(part2);
            return result;
        }
    }
    
    
    public NedTypeFieldEditor(Composite parent, boolean multiple, IProject contextProject, INedTypeInfo enclosingNedType, INedTypeResolver.IPredicate typeFilter) {
        this.contextProject = contextProject;
        this.enclosingNedType = enclosingNedType;
        this.typeFilter = typeFilter;
        this.multiple = multiple;

        composite = new Composite(parent, SWT.NONE);
        GridLayout layout = new GridLayout(2, false);
        composite.setLayout(layout);
        layout.horizontalSpacing = layout.verticalSpacing = layout.marginHeight = layout.marginWidth = 0; 

        text = new Text(composite, SWT.BORDER);
        text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        new ContentAssistCommandAdapter(text, new TextContentAdapter(), 
                new NedTypeContentProposalProvider(!multiple),
                ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, null /*".".toCharArray()*/, true); // using "." triggers a bug: in empty field, type Ctrl+Space, then 1st char of any proposal --> proposal windows disappears, but it shouldn't
        
        button = new Button(composite, SWT.PUSH);
        button.setText("...");
        GridData gridData = new GridData(SWT.CENTER, SWT.CENTER, false, false);
        int textHeight = text.computeSize(SWT.DEFAULT, SWT.DEFAULT).y;
        gridData.heightHint = textHeight;
        button.setLayoutData(gridData);

        problemDecoration = new ControlDecoration(button, SWT.RIGHT | SWT.TOP);
        problemDecoration.setShowOnlyOnFocus(false);

        text.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                if (isGrayed())
                    setGrayed(false);
            }
        });
        button.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                openDialog();
            }
        });
    }

    public INedTypeResolver.IPredicate getTypeFilter() {
        return typeFilter;
    }

    public IProject getContextProject() {
        return contextProject;
    }

    public boolean getMultiple() {
        return multiple;
    }

    public Composite getControl() {
        return composite;
    }

    public Text getTextControl() {
        return text;
    }

    public boolean isGrayed() {
        return grayed;
    }

    public void setGrayed(boolean grayed) {
        if (grayed) {
            text.setText("");
            text.setBackground(GREY);
        } 
        else {
            text.setBackground(null); // default system color
        }
        this.grayed = grayed; // note: must be AFTER text.setText()
    }

    public String getText() {
        return text.getText();
    }

    public void setText(String content) {
        text.setText(content);
        text.selectAll();
    }

    public boolean isEnabled() {
        return text.isEnabled();
    }

    public void setEnabled(boolean enabled) {
        if (text.isEnabled() != enabled) {
            text.setEnabled(enabled);
            button.setEnabled(enabled);
        }
    }

    public void setMessage(int severity, String text) {
        UIUtils.updateProblemDecoration(problemDecoration, severity, text);
    }

    public void addModifyListener(ModifyListener listener) {
        text.addModifyListener(listener);
    }

    public void removeModifyListener(ModifyListener listener) {
        text.removeModifyListener(listener);
    }

    protected void openDialog() {
        // create and configure dialog
        NedTypeSelectionDialog dialog = new NedTypeSelectionDialog(composite.getShell());
        dialog.setContextProject(NedTypeFieldEditor.this.contextProject);
        dialog.setEnclosingNedType(enclosingNedType);
        dialog.setTypeFilter(NedTypeFieldEditor.this.typeFilter);
        dialog.setMultipleSelection(NedTypeFieldEditor.this.multiple);
        
        // select the types that are currently in the text field
        List<String> qnames = parseNedTypeList(text.getText());
        List<INedTypeInfo> types = new ArrayList<INedTypeInfo>();
        for (String qname : qnames) {
            INedTypeInfo type = NedResourcesPlugin.getNedResources().getToplevelOrInnerNedType(qname, contextProject);
            if (type != null)
                types.add(type);
        }
        dialog.setInitialElementSelections(types);
        
        // open dialog
        if (dialog.open() == Dialog.OK) {
            String result = "";
            for (INedTypeInfo type : dialog.getResult()) {
                String typeName = NedElementUtilEx.qnameToFriendlyTypeName(type.getFullyQualifiedName());
                result += (result.equals("") ? "" : ", ") + typeName;
            }
            text.setText(result); // overwrite existing contents
        }
    }

    public static List<String> parseNedTypeList(String list) {
        List<String> result = new ArrayList<String>();
        for (String value : list.split(",")) {
            if (StringUtils.isNotEmpty(value)) {
                value = NedElementUtilEx.friendlyTypeNameToQName(value);
                result.add(value);
            }
        }
        return result;
    }

}