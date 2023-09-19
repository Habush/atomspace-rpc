//
// Created by Habush on 8/29/2023.
//

#ifndef ATOMSPACE_SERVER_ATOMSERVICEUTILS_H
#define ATOMSPACE_SERVER_ATOMSERVICEUTILS_H
#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include <opencog/atoms/base/Handle.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/bioscience/types/atom_types.h>
#include <opencog/atomspace/AtomSpace.h>

#include "atom_server.grpc.pb.h"

using namespace opencog;

class AtomServiceUtils {
public:
    static NameServer& namer;
    static Handle FromNodeMsg(const NodeMsg& node);
    static Handle FromLinkMsg(const LinkMsg& link);
    static Handle FromAtomMsg(const AtomMsg& atom);
    static AtomMsg ToAtomMsg(const Handle& patt);
    static NodeMsg ToNodeMsg(const Handle& node);
    static LinkMsg ToLinkMsg(const Handle& link);
};


#endif //ATOMSPACE_SERVER_ATOMSERVICEUTILS_H
