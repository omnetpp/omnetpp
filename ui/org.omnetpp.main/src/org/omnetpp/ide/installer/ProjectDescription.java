package org.omnetpp.ide.installer;

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
}