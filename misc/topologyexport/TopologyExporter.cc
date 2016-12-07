//==========================================================================
//  TOPOLOGYEXPORTER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete Event Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
 *--------------------------------------------------------------*/

#include <fstream>
#include <omnetpp.h>

/**
 * Helper class that facilitates writing XML documents.
 * Example:
 * <pre>
 *   xml.openTag("items");
 *   xml.openTag("item");
 *   xml.writeAttr("color", "red");
 *   xml.writeAttr("count", 42);
 *   xml.closeTag("item");
 *   xml.closeTag("items");
 * </pre>
 */
class XMLWriter
{
private:
    std::string indent;
    std::ostream& out;
    bool tagOpen;

public:
    XMLWriter(std::ostream& out_) : out(out_) {
        tagOpen = false;
    }

    void openTag(const char *tagName) {
        if (tagOpen)
            out << ">\n"; // close previous
        out << indent << "<" << tagName;
        tagOpen = true;
        indent += "    ";
    }

    void writeAttr(const char *attrName, const char *value) {
        ASSERT(tagOpen);
        out << " " << attrName << "=\"" << xmlquote(value) << "\"";
    }

    void writeAttr(const char *attrName, std::string value) {
        ASSERT(tagOpen);
        out << " " << attrName << "=\"" << xmlquote(value) << "\"";
    }

    void writeAttr(const char *attrName, int value) {
        ASSERT(tagOpen);
        out << " " << attrName << "=\"" << value << "\"";
    }

    void writeAttr(const char *attrName, bool value) {
        ASSERT(tagOpen);
        out << " " << attrName << "=\"" << (value?"true":"false") << "\"";
    }

    void closeTag(const char *tagName) {
        indent = indent.substr(4);
        if (tagOpen)
            out << "/>\n";
        else
            out << indent << "</" << tagName << ">\n";
        tagOpen = false;
    }

    std::string xmlquote(const std::string& str) {
        if (!strchr(str.c_str(), '<') && !strchr(str.c_str(), '>') && !strchr(str.c_str(), '"') && !strchr(str.c_str(), '&'))
            return str;

        std::stringstream out;
        for (const char *s=str.c_str(); *s; s++)
        {
            switch (*s) {
            case '<': out << "&lt;"; break;
            case '>': out << "&gt;"; break;
            case '"': out << "&quot;"; break;
            case '&': out << "&amp;"; break;
            default: out << *s;
            }
        }
        return out.str();
    }
};


/**
 * Implements the TopologyExporter module. See NED file for more info.
 */
class TopologyExporter : public cSimpleModule
{
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void dump(const char *filename);
    virtual void dump(XMLWriter& xml, cComponent *component);
    virtual void dumpProperties(XMLWriter& xml, cProperties *properties);
    virtual std::string quote(const char *propertyValue);
};

Define_Module(TopologyExporter);

void TopologyExporter::initialize()
{
    scheduleAt(par("t").doubleValue(), new cMessage());
}

void TopologyExporter::handleMessage(cMessage *msg)
{
    delete msg;
    dump(par("filename").stringValue());
}

void TopologyExporter::dump(const char *filename)
{
    std::ofstream os(filename, std::ios::out|std::ios::trunc);

    os << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    XMLWriter xml(os);
    dump(xml, simulation.getSystemModule());

    if (os.fail())
        throw cRuntimeError("Cannot write output file '%s'", filename);

    os.close();
}

inline const char *replace(const char *orig, const char *what, const char *replacement) {
    return strcmp(orig,what)==0 ? replacement : orig;
}

void TopologyExporter::dump(XMLWriter& xml, cComponent *component)
{
    const char *tagName = (component==simulation.getSystemModule()) ? "network" : component->isModule() ? "module" : "channel";
    xml.openTag(tagName);
    xml.writeAttr("name", component->getFullName());
    xml.writeAttr("type", component->getNedTypeName());

    if (component->getNumParams() > 0 || component->getProperties()->getNumProperties() > 0) {
        xml.openTag("parameters");
        dumpProperties(xml, component->getProperties());
        for (int i = 0; i < component->getNumParams(); i++) {
            cPar& p = component->par(i);
            xml.openTag("param");
            xml.writeAttr("name", p.getFullName());
            xml.writeAttr("type", replace(cPar::getTypeName(p.getType()), "long", "int"));
            xml.writeAttr("value", p.str());
            dumpProperties(xml, p.getProperties());
            xml.closeTag("param");
        }
        xml.closeTag("parameters");
    }

    if (component->isModule()) {
        cModule *mod = check_and_cast<cModule *>(component);

        if (!cModule::GateIterator(mod).end()) {
            xml.openTag("gates");
            for (cModule::GateIterator i(mod); !i.end(); i++) {
                cGate *gate = i();
                xml.openTag("gate");
                xml.writeAttr("name", gate->getFullName());
                xml.writeAttr("type", cGate::getTypeName(gate->getType()));
                //xml.writeAttr("connected-inside", gate->isConnectedInside());
                //xml.writeAttr("connected-outside", gate->isConnectedOutside());
                dumpProperties(xml, gate->getProperties());
                xml.closeTag("gate");
            }
            xml.closeTag("gates");
        }

        if (!cModule::SubmoduleIterator(mod).end()) {
            xml.openTag("submodules");
            for (cModule::SubmoduleIterator submod(mod); !submod.end(); submod++)
                dump(xml, submod());
            xml.closeTag("submodules");
        }

        xml.openTag("connections");
        bool atParent = false;
        for (cModule::SubmoduleIterator it(mod); !atParent; it++) {
            cModule *srcmod = !it.end() ? it() : (atParent=true,mod);
            for (cModule::GateIterator i(srcmod); !i.end(); i++) {
                cGate *srcgate = i();
                cGate *destgate = srcgate->getNextGate();
                if (srcgate->getType()==(atParent ? cGate::INPUT : cGate::OUTPUT) && destgate) {
                    cModule *destmod = destgate->getOwnerModule();
                    xml.openTag("connection");
                    if (srcmod == mod)
                        /* default - leave it out */;
                    else if (srcmod->getParentModule() == mod)
                        xml.writeAttr("src-module", srcmod->getFullName());
                    else
                        xml.writeAttr("src-module", srcmod->getFullPath());
                    xml.writeAttr("src-gate", srcgate->getFullName());
                    if (destmod == mod)
                        /* default - leave it out */;
                    else if (destmod->getParentModule() == mod)
                        xml.writeAttr("dest-module", destmod->getFullName());
                    else
                        xml.writeAttr("dest-module", destmod->getFullPath());
                    xml.writeAttr("dest-gate", destgate->getFullName());
                    cChannel *channel = srcgate->getChannel();
                    if (channel)
                        dump(xml, channel);
                    xml.closeTag("connection");
                }
            }
        }
        xml.closeTag("connections");
    }
    xml.closeTag(tagName);
}

void TopologyExporter::dumpProperties(XMLWriter& xml, cProperties *properties)
{
    for (int i = 0; i < properties->getNumProperties(); i++) {
        cProperty *prop = properties->get(i);
        xml.openTag("property");
        xml.writeAttr("name", prop->getFullName());
        std::string value;
        for (int k = 0; k < (int)prop->getKeys().size(); k++) {
            const char *key = prop->getKeys()[k];
            if (k!=0)
                value += ";";
            if (key && key[0])
                value += std::string(key) + "=";
            for (int v = 0; v < prop->getNumValues(key); v++)
                value += std::string(v==0?"":",") + quote(prop->getValue(key, v));
        }
        xml.writeAttr("value", value);
        xml.closeTag("property");
    }
}

std::string TopologyExporter::quote(const char *propertyValue)
{
    if (!strchr(propertyValue, ',') && !strchr(propertyValue, ';'))
        return propertyValue;
    return std::string("\"") + propertyValue + "\"";
}


