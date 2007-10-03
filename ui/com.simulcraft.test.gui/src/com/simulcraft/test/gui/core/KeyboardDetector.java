package com.simulcraft.test.gui.core;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.bindings.keys.KeyStroke;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

import com.simulcraft.test.gui.access.Access;

/**
 * 
 * @author Andras
 */
public class KeyboardDetector {
    private char receivedChar;
    
    public void run() {
        Assert.isTrue(Display.getCurrent() != null, "must be in the UI thread");
        
        final Shell shell = new Shell(SWT.TOOL | SWT.ON_TOP | SWT.APPLICATION_MODAL);
        shell.setSize(200, 20);
        Text text = new Text(shell, SWT.SINGLE | SWT.BORDER);
        shell.setLayout(new FillLayout());
        shell.layout();
        shell.open();
        text.setFocus();
        
        text.addListener(SWT.KeyDown, new Listener() {
            public void handleEvent(Event e) {
                if (receivedChar != 0)
                    MessageDialog.openWarning(shell, "Warning", "Please do NOT press that key again! Or any other key.\nThat's interfering with keyboard layout detection.");
                receivedChar = e.character;
            }
        });
        
        Map<Character,KeyStroke> mapping = new HashMap<Character, KeyStroke>();
        for (char naturalKey=32; naturalKey<127; naturalKey++) {
            if (naturalKey>='A' && naturalKey<='Z') 
                continue;  // we test letter keys on the keyboard in lowercase only 
            testKey(mapping, naturalKey, 0);
            testKey(mapping, naturalKey, SWT.SHIFT);
        }
        shell.dispose();

        System.out.println("RESULT:");
        for (Character naturalKey : sort(mapping.keySet()))
            System.out.println(" for "+naturalKey+" press "+mapping.get(naturalKey));
    }

    private Character[] sort(Set<Character> chars) {
        Character[] array = chars.toArray(new Character[]{});
        Arrays.sort(array);
        return array;
    }
    
    private void testKey(Map<Character, KeyStroke> mapping, char naturalKey, int modifier) {
        new Access().pressKey(naturalKey, modifier);
        while (Display.getCurrent().readAndDispatch());
        registerMapping(mapping, naturalKey, modifier, receivedChar);
        receivedChar = 0;
    }

    private static void registerMapping(Map<Character, KeyStroke> mapping, char naturalKey, int modifier, char receivedChar) {
        KeyStroke keyStroke = KeyStroke.getInstance(modifier, naturalKey);
        //System.out.println(keyStroke.format() + " ==> " + receivedChar);
        if (!mapping.containsKey(receivedChar))
            mapping.put(receivedChar, keyStroke);
        else {
            // only overwrite if new one is better
            KeyStroke existingKeyStroke = mapping.get(receivedChar);
            if (existingKeyStroke.getModifierKeys()==0 && existingKeyStroke.getNaturalKey() != receivedChar) // already good enough
                if (modifier==0 || (naturalKey>='a' && naturalKey<='z') || (naturalKey>='0' && naturalKey<='9')) // prefer keys without SHIFT, and letter and number keys
                    mapping.put(receivedChar, keyStroke);
        }
    }

}
