//
// Created by Abdulrahman Semrie on 6/26/20.
//

#include <cstdlib>
#include <string>
#include <memory>
#include <opencog/guile/SchemeModule.h>
#include <AtomServiceClient.h>

using namespace opencog;


class AtomServiceSCM : public ModuleWrap {
public:
    AtomServiceSCM(void);

    HandleSeq ExecutePattern(const std::string &atom_id, const std::string& key, const Handle &patt);

    bool CheckNode(const std::string &atom_id, const std::string &type_name, const std::string
    &node_name);

    HandleSeq FindSimilar(const std::string &atom_id, const std::string &type_name, const std::string
    &node_name);

private:
    void init(void);

    std::shared_ptr<grpc::Channel> _channel;
};

extern "C" {
    void opencog_atom_service_init(void);
}

#include "AtomServiceClient.h"
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/guile/SchemePrimitive.h>

AtomServiceSCM::AtomServiceSCM(void)
        :ModuleWrap("opencog grpc")
{
    static bool is_init = false;
    if (is_init) return;
    is_init = true;
    //if the ATOM_SERVER env variable is defined use that
    grpc::ChannelArguments args;
    args.SetCompressionAlgorithm(GRPC_COMPRESS_GZIP);
    if(const char* atom_env = std::getenv("ATOM_SERVER")) {
        _channel = grpc::CreateCustomChannel(atom_env, grpc::InsecureChannelCredentials(), args);
    } else {
        _channel = grpc::CreateCustomChannel("localhost:50051", grpc::InsecureChannelCredentials(), args);
    }

    module_init();
}

HandleSeq AtomServiceSCM::ExecutePattern(const std::string &atom_id, const std::string& key, const Handle &patt) {
    if(patt->is_executable()){
        HandleSeq res;
        AtomSpace* as = SchemeSmob::ss_get_env_as("exec-pattern");
        AtomServiceClient client(_channel);
        client.ExecutePattern(atom_id, key, patt, as, res);
        return res;
    }
     throw std::runtime_error(patt->to_short_string() + " is not an executable pattern");

}

bool AtomServiceSCM::CheckNode(const std::string &atom_id, const std::string &type_name, const std::string
&node_name) {
    AtomServiceClient client(_channel);
    Handle h = client.CheckNode(atom_id, type_name, node_name);

    return h != Handle::UNDEFINED;
}

HandleSeq AtomServiceSCM::FindSimilar(const std::string &atom_id, const std::string &type_name, const std::string
&node_name) {
    AtomSpace* as = SchemeSmob::ss_get_env_as("exec-pattern");
    AtomServiceClient client(_channel);
    HandleSeq res;
    client.FindSimilar(atom_id, type_name, node_name, res, as);

    return res;
}

void AtomServiceSCM::init(void) {
    define_scheme_primitive("exec-pattern",
            &AtomServiceSCM::ExecutePattern, this, "grpc");

    define_scheme_primitive("check-node",
            &AtomServiceSCM::CheckNode, this, "grpc");

    define_scheme_primitive("find-similar-node",
                            &AtomServiceSCM::FindSimilar, this, "grpc");
}

void opencog_atom_service_init(void) {
    static AtomServiceSCM scm;
}

