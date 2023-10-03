# Author Abdulrahman S. Omar <xabush@singularitynet.io>

from py.atom_server_pb2 import NodeMsg, LinkMsg, AtomMsg
from das.pattern_matcher.pattern_matcher import PatternMatchingAnswer, OrderedAssignment, UnorderedAssignment, CompositeAssignment, Node, Link, Variable, Not, And, Or
def node_msg_to_node(node_msg):
    #check if the type name contains the 'Node' suffix as das doesn't support it
    is_node_suffix = str.find(node_msg.type, "Node") != -1
    if is_node_suffix:
        node_msg.type = node_msg.type[:-4]
    # if node_msg.type in ["Variable", "Not", "And", "Or"]:
    #     return Node(node_msg.type) (T Name)
    if node_msg.type == "Variable":
        return Variable(node_msg.name)
    elif node_msg.type in ["Not", "And", "Or"]: # TODO fixme
        return Node(node_msg.type, "")
    else:
        return Node(node_msg.type, f"{node_msg.type}:{node_msg.name}")

def logical_expr_msg_to_logical_expr(logical_expr_msg):
    outgoing = []
    for atom_msg in logical_expr_msg.outgoing:
        if atom_msg.WhichOneof("atom") == "node":
            outgoing.append(node_msg_to_node(atom_msg.node))
        elif atom_msg.WhichOneof("atom") == "link":
            outgoing.append(link_msg_to_link(atom_msg.link))

    if logical_expr_msg.type == "AndLink":
        return And(outgoing)
    elif logical_expr_msg.type == "OrLink":
        return Or(outgoing)
    elif logical_expr_msg.type == "NotLink":
        return Not(outgoing)

def link_msg_to_link(link_msg):
    outgoing = []
    for atom_msg in link_msg.outgoing:
        if atom_msg.WhichOneof("atom") == "node":
            outgoing.append(node_msg_to_node(atom_msg.node))
        elif atom_msg.WhichOneof("atom") == "link":
            if atom_msg.link.type in ["AndLink", "OrLink", "NotLink"]:
                outgoing.append(logical_expr_msg_to_logical_expr(atom_msg.link))
            else:
                outgoing.append(link_msg_to_link(atom_msg.link))

    #check if the type name contains the 'Link' suffix as das doesn't support it
    is_link_suffix = str.find(link_msg.type, "Link") != -1
    if is_link_suffix:
        link_msg.type = link_msg.type[:-4]
    return Link(link_msg.type, ordered=True, targets=outgoing)

def atom_msg_to_atom(atom_msg):
    if atom_msg.WhichOneof("atom") == "node":
        return node_msg_to_node(atom_msg.node)
    elif atom_msg.WhichOneof("atom") == "link":
        return link_msg_to_link(atom_msg.link)



def node_to_node_msg(node):
    node_msg = NodeMsg()
    node_msg.type = node.atom_type
    node_msg.name = node.name.split(":")[1]
    return node_msg


def link_to_link_msg(link):
    link_msg = LinkMsg()
    link_msg.type = link.atom_type
    for target in link.outgoing:
        if isinstance(target, Node):
            link_msg.outgoing.append(AtomMsg(node=node_to_node_msg(target)))
        elif isinstance(target, Link):
            if target.type == "VariableList": continue # ignore variable lists
            link_msg.outgoing.append(AtomMsg(link=link_to_link_msg(target)))
    return link_msg

def atom_to_atom_msg(atom):
    atom_msg = AtomMsg()
    if isinstance(atom, Node):
        atom_msg.node.CopyFrom(node_to_node_msg(atom))
    elif isinstance(atom, Link):
        atom_msg.link.CopyFrom(link_to_link_msg(atom))
    return atom_msg

def handle_to_atom_msg(handle_id, db):
    atom_dict = db.get_atom_as_dict(handle_id)
    if "template" in atom_dict:  # it is Link
        return AtomMsg(link=link_dict_to_link_msg(atom_dict, db))

    else:  # it is Node
        return AtomMsg(node=node_dict_to_node_msg(atom_dict, db))

def link_dict_to_link_msg(link_dict, db):
    link_msg = LinkMsg()
    link_msg.type = link_dict["type"]
    for target in link_dict["targets"]:
        #Assume it is a node first
        try:
            node_name = db.get_node_name(target)
            node_type = db.get_node_type(target)
            node_msg = NodeMsg()
            node_msg.type = node_type
            node_msg.name = node_name.split(":")[1]
            link_msg.outgoing.append(AtomMsg(node=node_msg))
        except ValueError: # then it is a link
            link_dict = db.get_atom_as_dict(target)
            link_msg.outgoing.append(AtomMsg(link=link_dict_to_link_msg(link_dict, db)))
    return link_msg

def node_dict_to_node_msg(node_dict, db):
    node_msg = NodeMsg()
    node_msg.type = node_dict["type"]
    node_msg.name = node_dict["name"].split(":")[1]
    return node_msg