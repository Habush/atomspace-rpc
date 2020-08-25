//
// Created by Abdulrahman Semrie on 6/26/20.
//

#ifndef ATOMSPACE_SERVER_ATOMSERVICECLIENT_H
#define ATOMSPACE_SERVER_ATOMSERVICECLIENT_H

#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include <opencog/atoms/base/Handle.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/bioscience/types/atom_types.h>
#include <opencog/atomspace/AtomSpace.h>

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "atom_server.grpc.pb.h"

using namespace opencog;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using grpc::StatusCode;



class AtomServiceClient {
public:
    AtomServiceClient(const std::string& address):
        _stub(AtomServer::NewStub(grpc::CreateChannel(address, grpc::InsecureChannelCredentials())))
        {}

    //Re-use existing channel
    explicit AtomServiceClient(const std::shared_ptr<grpc::Channel>& channel) :
            _stub(AtomServer::NewStub(channel))
    {}

    /**
     * Executes a pattern on a remote atomspace
     * @param atom_id - the id of the remote atomspace
     * @param patt - the pattern to execute
     * @param as - the local atomspace to add the results to
     * @param result - a handle vector that will contain the result of the pattern matcher query
     */
    void ExecutePattern(const std::string &atom_id, const std::string &key, const Handle &patt,
                        AtomSpace *as, HandleSeq &result);

    /**
     * Checks if a node exists on a remote atomspace
     * @param atom_id - the id of the remote atomspace
     * @param type - the type id of the node
     * @param node_name - the name of the node
     * @return a Handle that is constructed from the node if it is found, otherwise returns UNDEFINED handle.
     * Note: this function doesn't add the result handle to a local atomspace
     * as the call is to check if a node exists
     */
    Handle CheckNode(const std::string &atom_id, const std::string &type_name, const std::string
    &node_name);

    /**
     * Finds similar nodes to a given node
     * @param atom_id - the id of the remote atomspace
     * @param type - the type id of the node
     * @param node_name - the name of the node
     * @param result -  a handle vector that will contain the similar nodes.
     * Note: Here similarity is just how similar the name of the nodes are given that they are the same time
     */
    void FindSimilar(const std::string &atom_id, const std::string &type_name, const std::string
    &node_name, HandleSeq& result, AtomSpace* as);

private:
    Handle FromNodeMsg(const NodeMsg& node);
    Handle FromLinkMsg(const LinkMsg& link);
    std::unique_ptr<AtomServer::Stub> _stub;
};

#endif //ATOMSPACE_SERVER_ATOMSERVICECLIENT_H