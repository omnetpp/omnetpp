//==========================================================================
//  TOPOLOGYEXPORTER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete Event Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2009 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <fstream>
#include <omnetpp.h>

/**
 * Helper class to facilitate generating well-formed XML.
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
    bool printClassNames;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void dump(const char *filename);
    virtual void dump(XMLWriter& xml, cModule *mod);
    virtual void dump(XMLWriter& xml, cProperties *properties);
};

Define_Module(TopologyExporter);

void TopologyExporter::initialize()
{
    scheduleAt(2, new cMessage());
}

void TopologyExporter::handleMessage(cMessage *msg)
{
    delete msg;
    dump("out.xml");
}

void TopologyExporter::dump(const char *filename)
{
    std::ofstream os(filename, std::ios::out|std::ios::trunc);

    os << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    XMLWriter xml(os);
    dump(xml, simulation.getSystemModule());

    if (os.fail())
        throw cRuntimeError("Cannot write '%s'", filename);

    os.close();
}

void TopologyExporter::dump(XMLWriter& xml, cModule *mod)
{
    const char *tagName = (mod==simulation.getSystemModule()) ? "network" : "module";
    xml.openTag(tagName);
    xml.writeAttr("name", mod->getFullName());
    xml.writeAttr("id", mod->getId());
    xml.writeAttr("type", mod->getNedTypeName());


    if (mod->getNumParams() > 0 || mod->getProperties()->getNumProperties() > 0) {
        xml.openTag("parameters");
        dump(xml, mod->getProperties());
        for (int i = 0; i < mod->getNumParams(); i++) {
            cPar& p = mod->par(i);
            xml.openTag("param");
            xml.writeAttr("name", p.getFullName());
            xml.writeAttr("type", cPar::getTypeName(p.getType()));
            xml.writeAttr("value", p.str());
            dump(xml, p.getProperties());
            xml.closeTag("param");
        }
        xml.closeTag("parameters");
    }

    if (!cModule::GateIterator(mod).end()) {
        xml.openTag("gates");
        for (cModule::GateIterator i(mod); !i.end(); i++) {
            cGate *gate = i();
            xml.openTag("gate");
            xml.writeAttr("name", gate->getFullName());
            xml.writeAttr("type", cGate::getTypeName(gate->getType()));
            //XXX where the gate is connected
            dump(xml, gate->getProperties());
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

    xml.closeTag(tagName);
}

void TopologyExporter::dump(XMLWriter& xml, cProperties *properties)
{
    for (int i = 0; i < properties->getNumProperties(); i++) {
        cProperty *prop = properties->get(i);
        xml.openTag("property");
        xml.writeAttr("name", prop->getFullName());
        //XXX keys, values, etc...
        xml.closeTag("property");
    }
}

