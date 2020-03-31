package org.omnetpp.neddoc;

import java.io.ByteArrayInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.SequenceInputStream;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;


/**
 * Gathers extension fragments that can be inserted into the generated documentation.
 * The extension file may contain the following XML elements (without a single root element):
 * <nedfragment>, <msgfragment>, <filefragment>. Each element must have a 'location' and an 'anchor'
 * attribute. 
 *   
 *   location - The name of the FILE or NED, MSG type to be extended
 *   anchor - Internal anchor name where extension must be inserted in the document.
 *            Possible anchor values:
 *            - FILE: top, types, bottom
 *            - NED:  top, after-types, after-description, after-image, after-diagrams, after-usage, 
 *                    after-inheritance, after-parameters, after-properties, after-gates, after-signals,
 *                    after-statistics, after-unassigned-parameters, bottom 
 *            - MSG:  top, after-description, after-diagrams, after-inheritance, after-fields,
 *                    after-properties, bottom
 *   
 *  e.g.:
 *  <nedfragment> location="my.package.MyModule" anchor="after-properties"><![CDATA[
 *    Extra Server docs after properties.
 *  ]]></nedfragment>                   
 *
 */
public class NeddocExtensions {    
    private Map<String, Map<String, String>> nedExtensions = new HashMap<>();
    private Map<String, Map<String, String>> msgExtensions = new HashMap<>();
    private Map<String, Map<String, String>> fileExtensions = new HashMap<>();

    public enum ExtType { NED, MSG, FILE };
    
    public NeddocExtensions(String neddocExtensionsFilePath) {
        try {
            // just wrap a root element around the multiple fragments so it's a valid XML document.            
            List<InputStream> streams = Arrays.asList(
                    new ByteArrayInputStream("<root>".getBytes()),
                    new FileInputStream(neddocExtensionsFilePath),
                    new ByteArrayInputStream("</root>".getBytes())
                    );
            SequenceInputStream is = new SequenceInputStream(Collections.enumeration(streams));
            Document doc = DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(is);
            NodeList fragmentList = doc.getDocumentElement().getChildNodes();
            for(int i=0; i<fragmentList.getLength(); i++){
                Node element = fragmentList.item(i);
                if(element.getNodeType() == Node.ELEMENT_NODE) {
                    String location = element.getAttributes().getNamedItem("location").getNodeValue();
                    String anchor = element.getAttributes().getNamedItem("anchor").getNodeValue();
                    String text = element.getTextContent();

                    Map<String, Map<String, String>> extensions =
                            "nedfragment".equals(element.getNodeName()) ? nedExtensions :
                            "msgfragment".equals(element.getNodeName()) ? msgExtensions :
                            "filefragment".equals(element.getNodeName()) ? fileExtensions : nedExtensions;                    

                    Map<String, String> ext4location = extensions.getOrDefault(location, new HashMap<>());
                    extensions.put(location, ext4location);  // put extLocation back in case it was not yet present and was just crated

                    ext4location.put(anchor, ext4location.getOrDefault(anchor, "") + text);
                }
            }                       
        }
        catch (SAXException | IOException | ParserConfigurationException | RuntimeException e) {
            throw new RuntimeException(e);
        }
    }

    public String getFragment(ExtType type, String location, String anchor) {
        Map<String, String> exts4location = 
                type == ExtType.NED ? nedExtensions.get(location) :
                type == ExtType.MSG ? msgExtensions.get(location) :
                type == ExtType.FILE ? fileExtensions.get(location) : null;
        return exts4location == null ? null : exts4location.get(anchor);        
    }
}
