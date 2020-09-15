//
// Created by Abdulrahman Semrie on 6/25/20.
//
#include <opencog/atoms/atom_types/NameServer.h>
#include "AtomServiceClient.h"

void AtomServiceClient::ExecutePattern(const std::string &atom_id, const Handle &patt, AtomSpace *as, HandleSeq &
result) {

    PatternMsg pattern;
    ClientContext context;
    AtomMsg atomMsg;
    pattern.set_atomspace(atom_id);
    pattern.set_query(patt->to_string());

    std::unique_ptr<ClientReader<AtomMsg>> reader(_stub->ExecutePattern(&context, pattern));


    while (reader->Read(&atomMsg)) {
        Handle h;
        if (atomMsg.has_link()) {
            h = FromLinkMsg(atomMsg.link());
        } else {
            h = FromNodeMsg(atomMsg.node());
        }
        result.push_back(as->add_atom(h));
    }

    Status status = reader->Finish();
    if (!status.ok()) {
        throw std::runtime_error("Executing pattern failed. Reason: " + status.error_message());
    }

}

Handle AtomServiceClient::CheckNode(const std::string &atom_id, const std::string &type_name, const std::string
&node_name) {

    ClientContext context;
    NodeMsg nodeMsg;
    nodeMsg.set_type(type_name);
    nodeMsg.set_name(node_name);

    NodeMsg response;
    AtomRequest req;
    req.set_atomspace(atom_id);
    req.mutable_atom()->mutable_node()->CopyFrom(nodeMsg);
    Status status = _stub->CheckNode(&context, req, &response);
    if (status.ok()) {
        return FromNodeMsg(response);
    } else if (status.error_code() == StatusCode::NOT_FOUND) {
        return Handle::UNDEFINED;
    } else {
        throw std::runtime_error("Executing pattern failed. Reason: " + status.error_message());
    }
}

void AtomServiceClient::FindSimilar(const std::string &atom_id, const std::string &type_name, const std::string
&node_name, HandleSeq& result, AtomSpace* as) {
    ClientContext context;
    AtomRequest req;
    NodeMsg nodeMsg;

    nodeMsg.set_type(type_name);
    nodeMsg.set_name(node_name);
    req.set_atomspace(atom_id);
    req.mutable_atom()->mutable_node()->CopyFrom(nodeMsg);

    NodeMsg resMsg;
    std::unique_ptr<ClientReader<NodeMsg>> reader(_stub->FindSimilar(&context, req));

    while(reader->Read(&resMsg)) {
        result.push_back(as->add_atom(FromNodeMsg(resMsg)));
    }

    Status status = reader->Finish();
    if (!status.ok()) {
        throw std::runtime_error("Executing pattern failed. Reason: " + status.error_message());
    }
}

NodeMsg AtomServiceClient::FindType(const std::string &atom_id, const std::string &name) {
    ClientContext context;
    PatternMsg msg;
    NodeMsg nodeMsg;

    msg.set_atomspace(atom_id);
    msg.set_query(name);

    Status status = _stub->FindType(&context, msg, &nodeMsg);
    if(!status.ok()){
        throw std::runtime_error("FindType rpc failed. Reason: " + status.error_message());
    }

    return nodeMsg;
}

Handle AtomServiceClient::FromNodeMsg(const NodeMsg &node) {
    Type type = nameserver().getType(node.type());
    return createNode(type, std::move(node.name()));
}

Handle AtomServiceClient::FromLinkMsg(const LinkMsg &link) {
    HandleSeq outgoing;
    for (auto &atom: link.outgoing()) {
        if (atom.has_node()) {
            outgoing.push_back(FromNodeMsg(atom.node()));
        } else {
            outgoing.push_back(FromLinkMsg(atom.link()));
        }
    }
    Type type = nameserver().getType(link.type());
    return createLink(std::move(outgoing), type);
}