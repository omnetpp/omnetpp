package org.omnetpp.ide.installer;

import java.io.File;
import java.net.URL;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.commons.io.FileUtils;
import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * This class provides data from the project description XML file.
 *
 * @author levy
 */
public class ProjectDescription {
    protected Document document;

    public ProjectDescription(Document document) {
        this.document = document;
    }

    public String getName() {
        return getProjectAttribute("name");
    }

    public String getVersion() {
        return getProjectAttribute("version");
    }

    public String getTitle() {
        return getProjectAttribute("title");
    }

    public String getShortDescription() {
        return getProjectAttribute("short-description");
    }

    public String getLongDescription() {
        return getProjectAttribute("long-description");
    }

    public String getInstallerURL() {
        return getProjectAttribute("installer-url");
    }

    public String getInstallerClass() {
        return getProjectAttribute("installer-class");
    }

    public String getDistributionURL() {
        return getProjectAttribute("distribution-url");
    }

    public String getWelcomePage() {
        return getProjectAttribute("welcome-page");
    }

    protected String getProjectAttribute(String attribute) {
        NodeList projectNodeList = document.getElementsByTagName("project");
        if (projectNodeList.getLength() == 1) {
            NamedNodeMap projectAttributes = projectNodeList.item(0).getAttributes();
            Node namedItem = projectAttributes.getNamedItem(attribute);
            return namedItem != null ? namedItem.getNodeValue() : null;
        }
        else
            throw new RuntimeException("Project description document must include exactly one project element");
    }

	static private File downloadToTempFile(URL projectDescriptionURL) {
	    try {
	        File projectDescriptionFile = File.createTempFile("projectDescription", ".xml");
	        FileUtils.copyURLToFile(projectDescriptionURL, projectDescriptionFile);
	        return projectDescriptionFile;
	    }
	    catch (Exception e) {
	        throw new RuntimeException("Cannot download project description from " + projectDescriptionURL, e);
	    }
	}

	static private ProjectDescription parse(File descriptionFile) {		
	    try {
	        DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory.newInstance();
	        DocumentBuilder documentBuilder = documentBuilderFactory.newDocumentBuilder();
	        return new ProjectDescription(documentBuilder.parse(descriptionFile));
	    }
	    catch (Exception e) {
	        throw new RuntimeException("Cannot parse project description from " + descriptionFile.getAbsolutePath(), e);
	    }
	}
	
	static public ProjectDescription download(URL projectDescriptionURL) {
        File projectDescriptionFile = ProjectDescription.downloadToTempFile(projectDescriptionURL);
        ProjectDescription projectDescription = ProjectDescription.parse(projectDescriptionFile);
        projectDescriptionFile.delete();
		return projectDescription;
	}
}