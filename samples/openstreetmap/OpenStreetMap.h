//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __OPENSTREETMAP_H
#define __OPENSTREETMAP_H

#include <string>
#include <vector>
#include <map>
#include <omnetpp.h>

using namespace omnetpp;

namespace osm {

typedef int64_t id_t;

struct Node {
  id_t id;
  double lat, lon;
  std::map<std::string,std::string> tags;
  std::string getTag(const std::string& k) const {auto it = tags.find(k); return it == tags.end() ? "" : it->second;}
};

struct Way {
  id_t id;
  std::vector<Node*> nodes;
  std::map<std::string,std::string> tags;
  std::string getTag(const std::string& k) const {auto it = tags.find(k); return it == tags.end() ? "" : it->second;}
};

struct Map {
  double minlat, minlon, maxlat, maxlon;
  std::vector<Node*> nodes;
  std::vector<Way*> ways;
  // note: relations are not stored
};

struct WayNode {
    const Way *way;
    int nodeIndex;
    WayNode(const Way *way, int nodeIndex) : way(way), nodeIndex(nodeIndex) {}
    const Node *getNode() const {return way->nodes[nodeIndex];}
};

struct StreetNetwork {
    typedef std::map<const Node*,std::vector<WayNode>> Intersections;
    std::vector<const Way*> streets;
    Intersections intersections;
};

class OpenStreetMap : public cSimpleModule
{
  private:
    Map map;
    StreetNetwork streetNetwork;

  protected:
    virtual void initialize() override;
    Map loadMap(cXMLElement *mapRoot);
    void drawMap(const Map& map);
    std::vector<const Way*> findStreets(const Map& map);
    StreetNetwork::Intersections findIntersections(const Map& map, const std::vector<const Way*>& streets);

  public:
    virtual ~OpenStreetMap();
    const Map& getMap() const {return map;}
    const StreetNetwork& getStreetNetwork() const {return streetNetwork;}
    cFigure::Point toCanvas(double lat, double lon);
};

}; //namespace

#endif
