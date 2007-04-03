package org.omnetpp.inifile.editor.model;

import java.util.ArrayList;
import java.util.HashMap;

import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * This is a layer above IInifileDocument, and contains
 * info about the relationship of inifile contents and 
 * NED. For example, which inifile parameter settings apply
 * to which NED module parameters.
 * 
 * @author Andras
 */
public class InifileAnalyzer {
	private IInifileDocument doc;
	
	class KeyData {
		// list of strings like "net.host[*].tcp.paramname"
		ArrayList<String> usedBy = new ArrayList<String>();
	};
	
	/**
	 * Ctor.
	 */
	public InifileAnalyzer(IInifileDocument doc) {
		this.doc = doc;
	}

	public void run() {
		INEDTypeResolver ned = NEDResourcesPlugin.getNEDResources();
		
		for (String section : doc.getSectionNames()) {
			for (String key : doc.getKeys(section)) {
				if (!key.contains(".")) {
					// must be some configuration, look up in the database
					validateConfig(section, key, doc, ned);
				}
				else if (!key.contains("-")) {
					// dotted, but no hyphen: must be parameter setting
					//XXX rename **.interval to **.record-interval, and warn for old name here
					doc.setData(section, key, new KeyData());
				}
				else {
					// dotted, and contains hyphen: per-object configuration
					// like use-default, rng-mapping, vector configuration, etc
					//XXX
				}
			}
		}
	}
	
	private void validateConfig(String section, String key, IInifileDocument doc, INEDTypeResolver ned) {
		ConfigurationEntry e = ConfigurationRegistry.getEntry(key);
		boolean isPerRunSection = section.startsWith("Run ");
		if (e == null) {
			System.out.println("ERROR: unknown configuration entry: "+key);
		}
		else if (e.isGlobal() && !section.equals(e.getSection())) {
			System.out.println("ERROR: key "+key+" occurs in wrong section, must be in ["+e.getSection()+"]");
		}
		else if (!e.isGlobal() && !section.equals(e.getSection()) && !isPerRunSection) {
			System.out.println("ERROR: key "+key+" occurs in wrong section, must be in ["+e.getSection()+"], or in a run-specific section [Run X]");
		}
		
		//XXX check the syntax of the value too, etc...
		//XXX check validity of some settings, like network=, preload-ned-files=, etc
	}

	public boolean isUsed(String section, String key) {
		return false;
	}

	public String[] getUsage(String section, String key) {
		return new String[] {};
	}
}
