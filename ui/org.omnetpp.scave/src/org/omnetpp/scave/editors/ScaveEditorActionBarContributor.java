package org.omnetpp.scave.editors;

import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.part.MultiPageEditorActionBarContributor;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.editors.ui.FormEditorPage;

public class ScaveEditorActionBarContributor extends MultiPageEditorActionBarContributor {

    @Override
    public void setActivePage(IEditorPart activeEditor) {
        ScaveEditor scaveEditor = (ScaveEditor)getPage().getActiveEditor();

        if (scaveEditor == null)
            return;

        FormEditorPage editorPage = scaveEditor.getActiveEditorPage();

        if (editorPage instanceof ChartPage) {
            ChartPage chartPage = (ChartPage)editorPage;
            ChartScriptEditor chartScriptEditor = chartPage.getChartScriptEditor();

            if (chartScriptEditor.isScriptEditorFocused())
                setTextEditorActions(chartScriptEditor);
            else
                setScaveEditorActions(scaveEditor.getActions());
        }
        else {
            setScaveEditorActions(scaveEditor.getActions());
        }

        getActionBars().updateActionBars();
    }

    private void setScaveEditorActions(ScaveEditorActions actions) {
        IActionBars bars = getActionBars();
        bars.setGlobalActionHandler(ActionFactory.DELETE.getId(), actions.removeAction);
        bars.setGlobalActionHandler(ActionFactory.SELECT_ALL.getId(), actions.selectAllAction);
        bars.setGlobalActionHandler(ActionFactory.CUT.getId(), actions.cutAction);
        bars.setGlobalActionHandler(ActionFactory.COPY.getId(), actions.copyAction);
        bars.setGlobalActionHandler(ActionFactory.PASTE.getId(), actions.pasteAction);
        bars.setGlobalActionHandler(ActionFactory.UNDO.getId(), actions.undoAction);
        bars.setGlobalActionHandler(ActionFactory.REDO.getId(), actions.redoAction);
    }

    private void setTextEditorActions(ChartScriptEditor chartScriptEditor) {
        IActionBars bars = getActionBars();
        bars.setGlobalActionHandler(ActionFactory.DELETE.getId(), chartScriptEditor.getAction(ActionFactory.DELETE.getId()));
        bars.setGlobalActionHandler(ActionFactory.SELECT_ALL.getId(), chartScriptEditor.getAction(ActionFactory.SELECT_ALL.getId()));
        bars.setGlobalActionHandler(ActionFactory.CUT.getId(), chartScriptEditor.getAction(ActionFactory.CUT.getId()));
        bars.setGlobalActionHandler(ActionFactory.COPY.getId(), chartScriptEditor.getAction(ActionFactory.COPY.getId()));
        bars.setGlobalActionHandler(ActionFactory.PASTE.getId(), chartScriptEditor.getAction(ActionFactory.PASTE.getId()));
        bars.setGlobalActionHandler(ActionFactory.UNDO.getId(), chartScriptEditor.getAction(ActionFactory.UNDO.getId()));
        bars.setGlobalActionHandler(ActionFactory.REDO.getId(), chartScriptEditor.getAction(ActionFactory.REDO.getId()));
    }

}
