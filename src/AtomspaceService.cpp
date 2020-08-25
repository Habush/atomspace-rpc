//
// Created by Abdulrahman Semrie on 6/25/20.
//
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <regex>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include <opencog/atoms/base/Node.h>
#include <manager/AtomSpaceManager.h>
#include <opencog/guile/SchemePrimitive.h>
#include <boost/program_options.hpp>

#include "atom_server.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;

using namespace opencog;
using json = nlohmann::json;
namespace po = boost::program_options;

class AtomServerImpl final : public AtomServer::Service {
public:
    explicit AtomServerImpl(const AtomSpaceManager &manager) : _atomManager(manager) {

    }

    Status ExecutePattern(ServerContext *context, const PatternMsg *patt, ServerWriter<AtomMsg> *writer) override {
        try {
            Handle result = _atomManager.executePattern(patt->atomspace(), patt->query(), patt->key());

            AtomMsg atomMsg;

            for (auto &h : result->getOutgoingSet()) {
                if(h->is_node()){
                    atomMsg.mutable_node()->CopyFrom(buildNodeMsg(h));
                } else if (h->is_link()) {
                    atomMsg.mutable_link()->CopyFrom(buildLinkMsg(h));
                }
                writer->Write(atomMsg);
            }

            return Status::OK;
        } catch(std::runtime_error& err) {
            std::cout << "Error: " << err.what() << std::endl;
            return Status(grpc::StatusCode::CANCELLED, err.what());
        }
    }

    Status CheckNode(ServerContext *context, const AtomRequest *req, NodeMsg *res) override {
        try {
            if(!req->atom().has_node()){
                throw std::runtime_error("Expecting a Node message");
            }
            Handle h = _atomManager.findNode(req->atom().node().type(), req->atom().node().name(), req->atomspace());
            if(h == Handle::UNDEFINED){
                return Status(grpc::StatusCode::NOT_FOUND, "Node not found");
            }

            res->CopyFrom(buildNodeMsg(h));
            return Status::OK;
        } catch(std::runtime_error& err) {
            std::cout << "Error: " << err.what() << std::endl;
            return Status(grpc::StatusCode::CANCELLED, err.what());
        }
    }

    Status FindSimilar(ServerContext *context, const AtomRequest *req, ServerWriter<NodeMsg> *writer) override {
        try {
            if(!req->atom().has_node()){
                throw std::runtime_error("Expecting a Node message");
            }
        } catch(std::runtime_error& err) {
            std::cout << "Error: " << err.what() << std::endl;
            return Status(StatusCode::CANCELLED, err.what());
        }

        HandleSeq result;
        _atomManager.findSimilarNames(req->atomspace(), req->atom().node().type(), req->atom().node().name(), result);

        for(auto &h : result) {
            writer->Write(buildNodeMsg(h));
        }

        return Status::OK;
    }

private:

    NodeMsg buildNodeMsg(const Handle &h) {
        NodeMsg nodeMsg;
        nodeMsg.set_type(nameserver().getTypeName(h->get_type()));
        nodeMsg.set_name(h->get_name());
        return nodeMsg;
    }

    LinkMsg buildLinkMsg(const Handle& h) {
        LinkMsg linkMsg;
        linkMsg.set_type(nameserver().getTypeName(h->get_type()));
        for(auto& out : h->getOutgoingSet()){
            AtomMsg msg;
            if(out->is_node()){
                msg.mutable_node()->CopyFrom(buildNodeMsg(out));

            } else if(out->is_link()){
                msg.mutable_link()->CopyFrom(buildLinkMsg(out));
            }
            linkMsg.mutable_outgoing()->Add(std::move(msg));
        }
        return linkMsg;
    }

    AtomSpaceManager _atomManager;
};

void RunServer(const std::string &fname, const std::string &addr) {
    AtomSpaceManager atomSpaceManager;
    //load the atomspaces from the setting file
    atomSpaceManager.loadFromSettings(fname);

    AtomServerImpl service(atomSpaceManager);

    std::string server_address(addr);

    ServerBuilder  builder;
    builder.SetDefaultCompressionAlgorithm(GRPC_COMPRESS_GZIP);
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}


int main(int argc, char *argv[]) {

    po::options_description desc("Usage");
    desc.add_options()
            ("help", "show help message")
            ("config, c", po::value<std::string>(), "path to the setting json file")
            ("host, h", po::value<std::string>()->default_value("localhost"), "host address - default localhost")
            ("port, p", po::value<std::string>()->default_value("50051"), "port number - default 50051");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("config")) {
        std::string addr = vm["host"].as<std::string>() + ":" + vm["port"].as<std::string>();
        RunServer(vm["config"].as<std::string>(), addr);
        return 0;
    } else {
        std::cout << "Please set path to the setting file. \n" << desc << std::endl;
        return 1;
    }
}