//
// Created by Abdulrahman Semrie on 8/29/2023.
//

#include "AtomServiceUtils.h"

NameServer& AtomServiceUtils::namer = nameserver();

Handle AtomServiceUtils::FromNodeMsg(const NodeMsg &node) {
    Type type = namer.getType(node.type());
    return createNode(type, std::move(node.name()));
}

Handle AtomServiceUtils::FromLinkMsg(const LinkMsg &link) {
    HandleSeq outgoing;
    for (auto &atom: link.outgoing()) {
        if (atom.has_node()) {
            outgoing.push_back(FromNodeMsg(atom.node()));
        } else {
            outgoing.push_back(FromLinkMsg(atom.link()));
        }
    }
    Type type = namer.getType(link.type());
    return createLink(std::move(outgoing), type);
}

Handle AtomServiceUtils::FromAtomMsg(const AtomMsg &atom) {
    if (atom.has_node()) {
        return FromNodeMsg(atom.node());
    } else {
        return FromLinkMsg(atom.link());
    }
}

AtomMsg AtomServiceUtils::ToAtomMsg(const Handle &patt) {
    AtomMsg atomMsg;
    if (patt->is_node()) {
        atomMsg.mutable_node()->CopyFrom(ToNodeMsg(patt));
    } else {
        atomMsg.mutable_link()->CopyFrom(ToLinkMsg(patt));
    }
    return atomMsg;
}

NodeMsg AtomServiceUtils::ToNodeMsg(const Handle& node) {
    NodeMsg nodeMsg;
    nodeMsg.set_type(namer.getTypeName(node->get_type()));
    nodeMsg.set_name(node->get_name());

    return nodeMsg;
}

LinkMsg AtomServiceUtils::ToLinkMsg(const Handle& link) {
    LinkMsg linkMsg;
    linkMsg.set_type(namer.getTypeName(link->get_type()));
    for(auto& h : link->getOutgoingSet()){
        AtomMsg* atomMsg = linkMsg.add_outgoing();
        if(h->is_node()){
            NodeMsg* nodeMsg = atomMsg->mutable_node();
            nodeMsg->set_name(h->get_name());
            nodeMsg->set_type(namer.getTypeName(h->get_type()));
        }
        else{
            LinkMsg linkMsg1 = AtomServiceUtils::ToLinkMsg(h);
            atomMsg->mutable_link()->CopyFrom(linkMsg1);
        }
    }

    return linkMsg;
}
