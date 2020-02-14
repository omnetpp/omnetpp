//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <cstring>
#include <cstdlib>
#include "OpenStreetMap.h"

namespace osm {

Define_Module(OpenStreetMap);

inline double strtod(const char *s)
{
    return std::strtod(s, nullptr);
}

inline long strtol(const char *s)
{
    return std::strtol(s, nullptr, 10);
}

OpenStreetMap::~OpenStreetMap()
{
   for (Way *way : map.ways)
       delete way;
   for (Node *node : map.nodes)
       delete node;
}

void OpenStreetMap::initialize()
{
    cXMLElement *mapXml = par("mapFile").xmlValue();
    map = loadMap(mapXml);
    EV << "loaded " << map.nodes.size() << " nodes, " << map.ways.size() << " ways\n";

    drawMap(map);

    streetNetwork.streets = findStreets(map);
    streetNetwork.intersections = findIntersections(map, streetNetwork.streets);
}

Map OpenStreetMap::loadMap(cXMLElement *mapRoot)
{
    Map map;
    std::map<id_t,Node*> nodeById;

    cXMLElement *boundsElement = mapRoot->getFirstChildWithTag("bounds");
    map.minlat = map.minlon = map.maxlat = map.maxlon = NAN;

    if (boundsElement) {
        map.minlat = strtod(boundsElement->getAttribute("minlat"));
        map.minlon = strtod(boundsElement->getAttribute("minlon"));
        map.maxlat = strtod(boundsElement->getAttribute("maxlat"));
        map.maxlon = strtod(boundsElement->getAttribute("maxlon"));
    }

    for (cXMLElement *child : mapRoot->getChildrenByTagName("node")) {
        Node *node = new Node();
        node->id = strtol(child->getAttribute("id"));
        node->lat = strtod(child->getAttribute("lat"));
        node->lon = strtod(child->getAttribute("lon"));

        for (cXMLElement *nodeChild : child->getChildren()) {
            if (strcmp(nodeChild->getTagName(),"tag")==0) {
                std::string k = nodeChild->getAttribute("k");
                std::string v = nodeChild->getAttribute("v");
                node->tags[k] = v;
            }
        }
        map.nodes.push_back(node);
        nodeById[node->id] = node;
    }

    for (cXMLElement *child : mapRoot->getChildrenByTagName("way")) {
        Way *way = new Way();
        way->id = strtol(child->getAttribute("id"));
        for (cXMLElement *wayChild : child->getChildren()) {
            if (strcmp(wayChild->getTagName(),"nd")==0) {
                id_t ref = strtol(wayChild->getAttribute("ref"));
                ASSERT(nodeById.find(ref) != nodeById.end());
                Node *node = nodeById[ref];
                way->nodes.push_back(node);
            }
            if (strcmp(wayChild->getTagName(),"tag")==0) {
                std::string k = wayChild->getAttribute("k");
                std::string v = wayChild->getAttribute("v");
                way->tags[k] = v;
            }
        }
        map.ways.push_back(way);
    }

    return map;
}

cFigure::Point OpenStreetMap::toCanvas(double lat, double lon)
{
    double x = 1000*(lon-map.minlon)/(map.maxlon-map.minlon);
    double y = 800*(map.maxlat-lat)/(map.maxlat-map.minlat);
    return { x, y };
}

void OpenStreetMap::drawMap(const Map& map)
{
    const cFigure::Color COLOR_HIGHWAY_PRIMARY = {255,255,120};
    const cFigure::Color COLOR_HIGHWAY_RESIDENTIAL = {240,240,240};
    const cFigure::Color COLOR_HIGHWAY_PATH = {128,128,128};

    cCanvas *canvas = getSystemModule()->getCanvas();
    canvas->setAnimationSpeed(1, this);

    for (const auto& way : map.ways) {
        std::vector<cFigure::Point> points;
        for (const auto& node : way->nodes)
            points.push_back(toCanvas(node->lat, node->lon));
        bool isArea = way->nodes.front() == way->nodes.back();

        if (!isArea) {
            // road, street, footway, etc.
            cPolylineFigure *polyline = new cPolylineFigure();
            polyline->setPoints(points);
            polyline->setZoomLineWidth(true);

            polyline->setName(std::to_string(way->id).c_str());
            std::string name = way->getTag("name");
            if (name != "")
                polyline->setTooltip(name.c_str());

            std::string highwayType = way->getTag("highway");
            if (highwayType == "primary" || highwayType == "secondary" || highwayType == "tertiary" ||
                highwayType == "primary_link" || highwayType == "secondary_link" || highwayType == "tertiary_link") {
                polyline->setLineWidth(8);
                polyline->setLineColor(COLOR_HIGHWAY_PRIMARY);
                polyline->setCapStyle(cFigure::CAP_ROUND);
                polyline->setJoinStyle(cFigure::JOIN_ROUND);
                polyline->setZIndex(2);
            }
            else if (highwayType == "residential" || highwayType == "service") {
                polyline->setLineWidth(4);
                polyline->setLineColor(COLOR_HIGHWAY_RESIDENTIAL);
                polyline->setCapStyle(cFigure::CAP_ROUND);
                polyline->setJoinStyle(cFigure::JOIN_ROUND);
                polyline->setZIndex(1);
            }
            else if (highwayType != "") { // footpath or similar
                polyline->setLineStyle(cFigure::LINE_DOTTED);
                polyline->setLineColor(COLOR_HIGHWAY_PATH);
                polyline->setZIndex(3); // on top
            }
            else { // administrative boundary or some other non-path thing
                delete polyline;
                polyline = nullptr;
            }

            if (polyline)
                canvas->addFigure(polyline);
        }
        else {
            // building, park, etc.
            cPolygonFigure *polygon = new cPolygonFigure();
            points.pop_back();
            polygon->setPoints(points);

            polygon->setName(std::to_string(way->id).c_str());
            std::string name = way->getTag("name");
            if (name != "")
                polygon->setTooltip(name.c_str());

            polygon->setFilled(true);
            polygon->setFillOpacity(0.1);
            polygon->setLineOpacity(0.5);
            polygon->setLineColor(cFigure::GREY);
            if (way->getTag("building") != "")
                polygon->setFillColor(cFigure::RED);
            else
                polygon->setFillColor(cFigure::GREEN);
            polygon->setZIndex(0);
            canvas->addFigure(polygon);
        }
    }
}

std::vector<const Way*> OpenStreetMap::findStreets(const Map& map)
{
    std::vector<const Way*> streets;
    for (const Way *way : map.ways) {
        std::string highwayType = way->getTag("highway");
        if (highwayType == "primary" || highwayType == "secondary" || highwayType == "tertiary" ||
            highwayType == "primary_link" || highwayType == "secondary_link" || highwayType == "tertiary_link" ||
            highwayType == "residential" || highwayType == "service")
        {
            streets.push_back(way);
        }
    }
    return streets;
}

StreetNetwork::Intersections OpenStreetMap::findIntersections(const Map& map, const std::vector<const Way*>& streets)
{
    // find nodes that occur in more than one street
    StreetNetwork::Intersections intersections;
    for (const Way *way : streets) {
        int i = 0;
        for (const Node *node : way->nodes)
            intersections[node].push_back(WayNode(way,i++));
    }
    std::vector<const Node*> nonintersections;
    for (auto intersection: intersections)
        if (intersection.second.size() == 1)
            nonintersections.push_back(intersection.first);
    for (const Node *node: nonintersections)
        intersections.erase(node);
    EV << intersections.size() << " intersections found\n";

    return intersections;
}

}; //namespace

