//
// Created by Abdulrahman Semrie on 6/26/20.
//

#include <cstdlib>
#include <string>
#include <memory>
#include <opencog/guile/SchemeModule.h>
#include <opencog/guile/SchemePrimitive.h>
#include <libguile.h>
#include <AtomServiceClient.h>
using namespace opencog;

class AtomServiceSCM;

//Special case where the returned type is SCM type.
//Note: I had to add this custom class because using SchemePrimitive<SCM, C, Args...> call to member function 'scm_from' is ambiguous
//with candidate functions SCM scm_from(SCM scm) const and SCM scm_from(bool b) const
template<typename C, class... Args>
class CustomSchemePrimitive: public SchemeMethodBase<SCM, C, Args...> {
    typedef SchemeMethodBase<SCM, C, Args...> super;
public:
    CustomSchemePrimitive(const char* module, const char* name,
                          SCM(C::*cb)(Args...), C *data): super(module, name, cb, data){}

protected:
    // Convert any type to SCM
    SCM scm_from(SCM scm) const
    {
        return scm;
    }

    SCM invoke (SCM args) override {
        return scm_from(super::cpp_invoke(args));
    }
};

template<typename C, class... Args>
inline void define_scheme_custom_primitive(const char *name,
                                    SCM (C::*method)(Args...),
                                    C *data,
                                    const char* module= "extension")
{
    new CustomSchemePrimitive(module, name, method, data);
}


class AtomServiceSCM : public ModuleWrap {
public:
    AtomServiceSCM(void);

    HandleSeq ExecutePattern(const std::string &atom_id, const Handle &patt);

    bool CheckNode(const std::string &atom_id, const std::string &type_name, const std::string
    &node_name);

    HandleSeq FindSimilar(const std::string &atom_id, const std::string &type_name, const std::string
    &node_name);

    SCM FindType(const std::string& atom_id, SCM lst);

private:
    void init(void);
    //Re-definition of opencog::SchemeSmob::scm_to_string_list as that one has a private access
    static std::vector<std::string> scm_to_string_list(SCM lst);
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

void AtomServiceSCM::init(void) {
    define_scheme_primitive("exec-pattern",
                            &AtomServiceSCM::ExecutePattern, this, "grpc");

    define_scheme_primitive("check-node",
                            &AtomServiceSCM::CheckNode, this, "grpc");

    define_scheme_primitive("find-similar-node",
                            &AtomServiceSCM::FindSimilar, this, "grpc");

    define_scheme_custom_primitive("find-type", &AtomServiceSCM::FindType, this, "grpc");
}

HandleSeq AtomServiceSCM::ExecutePattern(const std::string &atom_id, const Handle &patt) {
    if(patt->is_executable()){
        HandleSeq res;
        AtomSpace* as = SchemeSmob::ss_get_env_as("exec-pattern");
        AtomServiceClient client(_channel);
        client.ExecutePattern(atom_id, patt, as, res);
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

/**
 * Find the atoms representing the list of string names
 * @param atom_id - the atomspace id
 * @param lst - list of names that we want to search for
 * @return
 */
SCM AtomServiceSCM::FindType(const std::string& atom_id, SCM lst) {
    AtomServiceClient client(_channel);
//    std::string id(scm_to_utf8_string(atom_id));
    std::vector<std::string> names = scm_to_string_list(lst);

    SCM result = SCM_EOL;
    NodeMsg msg;
    for(auto &name: names){
        msg = client.FindType(atom_id, name);
        if(msg.type() == "undefined"){
            result = scm_acons(scm_from_utf8_string(name.c_str()), SCM_BOOL_F, result);
        } else{
           result = scm_acons(scm_from_utf8_string(name.c_str()), scm_from_utf8_string(msg.type().c_str()), result);
        }
    }

    return result;

}


std::vector<std::string> AtomServiceSCM::scm_to_string_list (SCM svalue_list)
{
    std::vector<std::string> out;
    SCM sl = svalue_list;
    while (scm_is_pair(sl)) {
        SCM svalue = SCM_CAR(sl);

        if (not scm_is_null(svalue)) {
            char * v = scm_to_utf8_string(svalue);
            out.emplace_back(v);
        }
        sl = SCM_CDR(sl);
    }
    return out;
}

void opencog_atom_service_init(void) {
    static AtomServiceSCM scm;
}

