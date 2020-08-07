//
// Created by Abdulrahman Semrie on 5/28/20.
//

#include <stdexcept>
#include <algorithm>
#include <string>
#include <regex>
#include <fstream>
#include <opencog/atoms/base/Handle.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/atom_types/NameServer.h>
#include <opencog/persist/file/fast_load.h>

#ifdef HAVE_BIOSCIENCE
#include <opencog/bioscience/types/atom_types.h>
#endif

#include "AtomSpaceManager.h"
#include "Timer.h"

static NameServer& namer = nameserver();

AtomSpacePtr AtomSpaceManager::loadAtomSpace(const std::string &fname, const std::string &id) {
    //Check if the id exists
    auto res = _atomspaceMap.find(id);
    if (res != _atomspaceMap.end()) {
        throw std::runtime_error("An Atomspace with id " + id + " already exists");
    }

    AtomSpacePtr atomspace = std::make_shared<AtomSpace>();

    load_file(fname, *atomspace);

    return atomspace;

}

AtomSpacePtr AtomSpaceManager::loadDirectory(const std::string &dirname, const std::string &id) {
    //Check if the directory exists
    fs::path p(dirname);
    AtomSpacePtr atomspace = std::make_shared<AtomSpace>();
    if (fs::exists(p)) {
        for (fs::directory_entry &entry : fs::recursive_directory_iterator(p)) {
            if (entry.path().extension() == ".scm") {
                std::cout << "Parsing " << entry.path().string() << std::endl;
                load_file(entry.path().string(), *atomspace);
            }
        }
    } else {
        throw std::runtime_error("No such directory " + dirname);
    }

    return atomspace;
}


bool AtomSpaceManager::removeAtomSpace(const std::string &id) {
    auto res = _atomspaceMap.find(id);
    if (res == _atomspaceMap.end()) {
        return false;
    }

    _atomspaceMap.erase(id);
    _atomIds.erase(std::remove(_atomIds.begin(), _atomIds.end(), id));
    return true;

}


Handle AtomSpaceManager::executePattern(const std::string &id, const std::string &pattern) const {
    auto res = _atomspaceMap.find(id);
    if (res == _atomspaceMap.end()) {
        throw std::runtime_error("An Atomspace with id " + id + " not found");
    }

    Handle h;
    AtomSpacePtr atomspace = res->second;

    try {
        h = opencog::parseExpression(pattern, *atomspace);
    } catch (std::runtime_error &err) {
        throw err;
    }

    if (h == nullptr) {
        throw std::runtime_error("Invalid Pattern Matcher query: " + pattern);
    }

    Handle result;
    if (h->is_executable()) {

        ValuePtr pattResult = h->execute(atomspace.get());
        result = std::dynamic_pointer_cast<Atom>(pattResult);
        return result;

    } // not a pattern matching query
    atomspace->remove_atom(h, true);
    throw std::runtime_error("Only send pattern matching query to execute patterns. " + pattern + " is not a "
                                                                                                     "pattern matching query");

}

std::vector<std::string> AtomSpaceManager::getAtomspaces() const {
    return _atomIds;
}

AtomSpacePtr AtomSpaceManager::getAtomspace(const std::string &id) const {
    auto search = _atomspaceMap.find(id);
    if (search == _atomspaceMap.end()) {
        return nullptr;
    }
    return search->second;
}

void AtomSpaceManager::loadFromSettings(const std::string &fname) {
    std::ifstream fstream(fname);

    json inputJson;

    if (!fstream.is_open())
        throw std::runtime_error("Cannot find file >>" + fname + "<<");

    fstream >> inputJson;
    Timer timer;
    AtomSpacePtr atomspace;
    for (const auto &j : inputJson) {
        std::cout << "Loading Atomspace " << j["id"] << std::endl;
        if (j.find("scmFile") != j.end()) { //load from scm file
            atomspace = loadAtomSpace(j["scmFile"], j["id"]);
        } else if (j.find("pathDir") != j.end()) {
            atomspace = loadDirectory(j["pathDir"], j["id"]);
        }

        _atomspaceMap.insert({j["id"], atomspace});
        _atomIds.push_back(j["id"]);
        std::cout << "Atomspace " << j["id"] << " Loaded!" << std::endl;
    }

    std::cout << "Took " << timer.elapsed() << " seconds load Atomspaces" << std::endl;

}

Handle AtomSpaceManager::findNode(Type type, const std::string &name, const std::string& id) {
     AtomSpacePtr as = getAtomspace(id);
     if(as == nullptr){
         throw std::runtime_error("Atomspace with id " + id + " not found!");
     }
     Handle h = createNode(type, name);
     return as->get_atom(h);
}

void AtomSpaceManager::findSimilarNames(const std::string &id, Type type, const std::string &name,
                      HandleSeq &result) {

    if (!namer.isNode(type)) return;

    AtomSpacePtr as = getAtomspace(id);
    if(as == nullptr){
        throw std::runtime_error("Atomspace with id " + id + " not found!");
    }
    std::string patt = name + ".+$";
    std::regex similar_regex(patt, std::regex_constants::ECMAScript | std::regex_constants::icase);

    opencog::HandleSet atoms;
    as->get_handleset_by_type(atoms, type);

    for(auto& h : atoms){
        if(result.size() == 10) break;

        if(std::regex_search(h->get_name(), similar_regex)){
            result.push_back(h);
        }

    }

    //TODO why isn't this working???
//    atoms.erase(std::remove_if(atoms.begin(), atoms.end(), [&similar_regex](const opencog::Handle& h) {
//        return std::regex_search(h->get_name(), similar_regex);
//    }), atoms.end());
//
//    std::transform(atoms.begin(), atoms.end(), result.begin(), [](const opencog::Handle& h) {
//        return h->get_name();
//    });
}
