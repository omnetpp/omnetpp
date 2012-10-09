package org.omnetpp.ned.editor.graph.dialogs;

import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.properties.ColorCellEditorEx;
import org.omnetpp.common.ui.ColorSelector;
import org.omnetpp.common.util.UIUtils;

/**
 * Represents a color selection widget in the Properties dialog
 *
 * @author Andras
 */
public class ColorFieldEditor implements IFieldEditor {
    private Composite composite;
    private Text text;
    private ColorSelector colorSelector;
    private boolean grayed = false;
    private ControlDecoration problemDecoration;

    public ColorFieldEditor(Composite parent, int style) {
        composite = new Composite(parent, SWT.NONE);
        GridLayout layout = new GridLayout(2, false);
        composite.setLayout(layout);
        layout.horizontalSpacing = layout.verticalSpacing = layout.marginHeight = layout.marginWidth = 0;

        text = new Text(composite, SWT.BORDER);
        text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        ((GridData)text.getLayoutData()).widthHint = 12 * PropertiesDialog.computeDialogFontAverageCharWidth(composite);
        new ContentAssistCommandAdapter(text, new TextContentAdapter(),
                new ColorCellEditorEx.ColorContentProposalProvider(),
                ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, null, true);

        colorSelector = new ColorSelector(composite);
        GridData gridData = new GridData(SWT.CENTER, SWT.CENTER, false, false);
        if (!Platform.getOS().equals(Platform.OS_MACOSX)) { // does not look good on OS X
            int textHeight = text.computeSize(SWT.DEFAULT, SWT.DEFAULT).y;
            gridData.heightHint = textHeight;
            gridData.widthHint = textHeight + 2;  // widthHint=textHeight is too small on Linux, 1 pixel columns get shaved off from the image both sides
        }
        colorSelector.getButton().setLayoutData(gridData);

        problemDecoration = new ControlDecoration(colorSelector.getButton(), SWT.RIGHT | SWT.TOP);
        problemDecoration.setShowOnlyOnFocus(false);

        text.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                if (isGrayed())
                    setGrayed(false);
                RGB rgb = ColorFactory.asRGB(text.getText());
                colorSelector.setColorValue(rgb);
            }
        });
        colorSelector.addListener(new IPropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent event) {
                if (isGrayed())
                    setGrayed(false);
                String color = ColorFactory.asString(colorSelector.getColorValue());
                text.setText(color);
            }
        });
    }

    public Composite getControl() {
        return composite;
    }

    public boolean isGrayed() {
        return grayed;
    }

    public void setGrayed(boolean grayed) {
        if (grayed) {
            text.setText("");
            text.setBackground(GREY);
            colorSelector.setColorValue(null);
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
        RGB rgb = ColorFactory.asRGB(content);
        colorSelector.setColorValue(rgb);
    }

    public boolean isEnabled() {
        return text.isEnabled();
    }

    public void setEnabled(boolean enabled) {
        if (text.isEnabled() != enabled) {
            text.setEnabled(enabled);
            colorSelector.setEnabled(enabled);
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
}