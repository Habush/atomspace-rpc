//
// Created by Abdulrahman Semrie on 5/28/20.
//

#ifndef ATOMSPACE_SERVER_ATOMSPACEMANAGER_H
#define ATOMSPACE_SERVER_ATOMSPACEMANAGER_H

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/value/QueueValue.h>
#include <nlohmann/json.hpp>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using namespace opencog;
using json = nlohmann::json;
typedef std::shared_ptr<AtomSpace> AtomSpacePtr;

typedef std::map<std::string, AtomSpacePtr> AtomSpaceMap;
typedef std::vector<Type> TypeList;
static NameServer& namer = nameserver();

class AtomSpaceManager {

public:
    AtomSpaceManager() {
        namer.getChildrenRecursive(CONCEPT_NODE, std::back_inserter(_types));
        _types.push_back(CONCEPT_NODE);
    }

    ~AtomSpaceManager() = default;


    AtomSpacePtr loadDirectory(const std::string &dirname, const std::string &id);

    AtomSpacePtr loadAtomSpace(const std::string &fname, const std::string &id);

    bool removeAtomSpace(const std::string &id);

    [[nodiscard]] Handle executePattern(const std::string &id, const std::string &pattern) const;

    //Load atomspaces from a settings json file
    void loadFromSettings(const std::string &fname);

    std::vector<std::string> getAtomspaces() const;

    AtomSpacePtr getAtomspace(const std::string &id) const;

    Handle findNode(const std::string& type_name, const std::string &name, const std::string &id);

    //Given a name, return atoms with that name
    Handle findType(const std::string& id, const std::string &name);

    void findSimilarNames(const std::string &id, const std::string& type_name, const std::string &name,
                          HandleSeq &result);

private:
    AtomSpaceMap _atomspaceMap;
    std::vector<std::string> _atomIds;
    TypeList _types;
};


#endif //ATOMSPACE_SERVER_ATOMSPACEMANAGER_H
