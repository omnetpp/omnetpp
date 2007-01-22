package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.palette.PaletteDrawer;
import org.eclipse.gef.palette.PaletteEntry;
import org.eclipse.gef.ui.palette.PaletteCustomizer;
import org.eclipse.gef.ui.palette.customize.DefaultEntryPage;
import org.eclipse.gef.ui.palette.customize.DrawerEntryPage;
import org.eclipse.gef.ui.palette.customize.EntryPage;

// TODO do we need this?
public class ModulePaletteCustomizer extends PaletteCustomizer {

    protected static final String ERROR_MESSAGE = "Invalid character";

    /**
     * @see org.eclipse.gef.ui.palette.PaletteCustomizer#getPropertiesPage(PaletteEntry)
     */
    @Override
    public EntryPage getPropertiesPage(PaletteEntry entry) {
        if (PaletteDrawer.PALETTE_TYPE_DRAWER.equals( entry.getType() )) { return new ModuleDrawerEntryPage(); }
        return new ModuleEntryPage();
    }

    /**
     * @see org.eclipse.gef.ui.palette.PaletteCustomizer#revertToSaved()
     */
    @Override
    public void revertToSaved() {
    }

    /**
     * @see org.eclipse.gef.ui.palette.PaletteCustomizer#save()
     */
    @Override
    public void save() {
    }

    private class ModuleEntryPage extends DefaultEntryPage {
        @Override
        protected void handleNameChanged(String text) {
            if (text.indexOf('*') >= 0) {
                getPageContainer().showProblem(ERROR_MESSAGE);
            } else {
                super.handleNameChanged(text);
                getPageContainer().clearProblem();
            }
        }
    }

    private class ModuleDrawerEntryPage extends DrawerEntryPage {
        @Override
        protected void handleNameChanged(String text) {
            if (text.indexOf('*') >= 0) {
                getPageContainer().showProblem(ERROR_MESSAGE);
            } else {
                super.handleNameChanged(text);
                getPageContainer().clearProblem();
            }
        }
    }

}
