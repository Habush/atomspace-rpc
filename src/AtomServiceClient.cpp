//
// Created by Abdulrahman Semrie on 6/25/20.
//
#include <opencog/atoms/value/Value.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/atom_types/NameServer.h>
#include <opencog/persist/sexpr/Sexpr.h>
#include "AtomServiceClient.h"

void AtomServiceClient::ExecutePattern(const std::string &atom_id, const Handle &patt, AtomSpace *as, HandleSeq &
result) {

    PatternMsg pattern;
    ClientContext context;
    LinkMsg query = AtomServiceUtils::ToLinkMsg(patt);
    pattern.set_atomspace(atom_id);
    pattern.mutable_query()->CopyFrom(query);
    std::unique_ptr<ClientReader<AtomMsg>> reader(_stub->ExecutePattern(&context, pattern));

    AtomMsg atomMsg;
    while (reader->Read(&atomMsg)) {
        Handle h = AtomServiceUtils::FromAtomMsg(atomMsg);
        result.push_back(as->add_atom(h));
    }
        std::cout << "Finished reading" << std::endl;
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
    req.mutable_atom()->CopyFrom(nodeMsg);
    Status status = _stub->CheckNode(&context, req, &response);
    if (status.ok()) {
        return AtomServiceUtils::FromNodeMsg(response);
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
    req.mutable_atom()->CopyFrom(nodeMsg);

    NodeMsg resMsg;
    std::unique_ptr<ClientReader<NodeMsg>> reader(_stub->FindSimilar(&context, req));

    while(reader->Read(&resMsg)) {
        result.push_back(as->add_atom(AtomServiceUtils::FromNodeMsg(resMsg)));
    }

    Status status = reader->Finish();
    if (!status.ok()) {
        throw std::runtime_error("Executing pattern failed. Reason: " + status.error_message());
    }
}

NodeMsg AtomServiceClient::FindType(const std::string &atom_id, const std::string &name) {
    ClientContext context;
    NodeMsg nodeMsg;
    AtomRequest req;
    req.set_atomspace(atom_id);
    nodeMsg.set_name(name);
    req.mutable_atom()->CopyFrom(nodeMsg);
    Status status = _stub->FindType(&context, req, &nodeMsg);
    if(!status.ok()){
        throw std::runtime_error("FindType rpc failed. Reason: " + status.error_message());
    }

    return nodeMsg;
}