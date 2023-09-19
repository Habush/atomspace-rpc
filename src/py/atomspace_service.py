# Author Abdulrahman S. Omar <xabush@singularitynet.io>
import grpc
import re
from atom_server_pb2_grpc import AtomServerServicer
from atom_server_pb2 import NodeMsg, LinkMsg, AtomMsg
from das.distributed_atom_space import DistributedAtomSpace, QueryOutputFormat
from das.pattern_matcher.pattern_matcher import PatternMatchingAnswer, OrderedAssignment, UnorderedAssignment, \
                                                 CompositeAssignment, Node, Link, Variable, Not, And, Or, LogicalExpression
from atomservice_utils import *

import warnings
warnings.filterwarnings('ignore')



class AtomSpaceService(AtomServerServicer):
    def __init__(self, das):
        self.das = das
        self.db = self.das.db
        self.db.prefetch()

    def ExecutePattern(self, request, context):
        exec_link = link_msg_to_link(request.query)
        rewrite_link = None
        if exec_link.atom_type in ["Get", "Bind", "Meet"]:
            top_ln = exec_link.targets[0]
            if not isinstance(top_ln, LogicalExpression) and top_ln.atom_type == "VariableList":
                pattern = exec_link.targets[1]
            else:
                pattern = top_ln
            if exec_link.atom_type == "Bind":
                rewrite_link = exec_link.targets[-1]

        query_answer = PatternMatchingAnswer()
        matched = pattern.matched(self.db, query_answer)
        if matched:
            for assignment in query_answer.assignments:
                if rewrite_link is None: #Get or MeetLink
                    for _, handle_id in assignment.mapping.items():
                        yield handle_to_atom_msg(handle_id, self.db)
                else:
                    grounded_link = rewrite_link.apply_assignment(assignment, self.db)
                    handle_id = self.db.get_link_handle(grounded_link.atom_type, grounded_link.targets)
                    yield handle_to_atom_msg(handle_id, self.db)



    def CheckNode(self, request, context):
        node = node_msg_to_node(request.atom)
        try:
            handle = self.db.get_node_handle(node.atom_type, node.name)
            _ = self.db.get_node_name(handle) #this will raise an exception if the handle doesn't exist
            return node_to_node_msg(node)
        except:
            context.set_code(grpc.StatusCode.NOT_FOUND)
            context.set_details("Not Found")
            return NodeMsg()

    def FindSimilar(self, request, context):
        node = node_msg_to_node(request.atom)
        regex_pttn = f"{node.name.split(':')[1]}.+$"
        regex = re.compile(regex_pttn)

        handles = self.db.get_all_nodes(node.atom_type)
        k = 0 # only return the first 10 matching atoms
        for h in handles:
            if k > 10: break
            node_name = self.db.get_node_name(h).split(":")[1]
            if regex.match(node_name):
                k += 1
                node_msg = NodeMsg()
                node_msg.name = node_name
                node_msg.type = node.atom_type
                yield node_msg

