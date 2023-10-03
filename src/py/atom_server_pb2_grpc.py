# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
"""Client and server classes corresponding to protobuf-defined services."""
import grpc

import py.atom_server_pb2 as atom__server__pb2


class AtomServerStub(object):
    """Missing associated documentation comment in .proto file."""

    def __init__(self, channel):
        """Constructor.

        Args:
            channel: A grpc.Channel.
        """
        self.ExecutePattern = channel.unary_stream(
                '/AtomServer/ExecutePattern',
                request_serializer=atom__server__pb2.PatternMsg.SerializeToString,
                response_deserializer=atom__server__pb2.AtomMsg.FromString,
                )
        self.CheckNode = channel.unary_unary(
                '/AtomServer/CheckNode',
                request_serializer=atom__server__pb2.AtomRequest.SerializeToString,
                response_deserializer=atom__server__pb2.NodeMsg.FromString,
                )
        self.FindSimilar = channel.unary_stream(
                '/AtomServer/FindSimilar',
                request_serializer=atom__server__pb2.AtomRequest.SerializeToString,
                response_deserializer=atom__server__pb2.NodeMsg.FromString,
                )
        self.FindType = channel.unary_unary(
                '/AtomServer/FindType',
                request_serializer=atom__server__pb2.AtomRequest.SerializeToString,
                response_deserializer=atom__server__pb2.NodeMsg.FromString,
                )


class AtomServerServicer(object):
    """Missing associated documentation comment in .proto file."""

    def ExecutePattern(self, request, context):
        """Returns pattern matcher execution result
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def CheckNode(self, request, context):
        """Check if a node exists on a remote atomspace. Return the OK status if it exists
        Return NOT_FOUND status if it is not found
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def FindSimilar(self, request, context):
        """Find nodes with similar names to a given node. Currently it uses a simple regex and limits the result to 10
        Current implementation gets all nodes of a type and finds those that match the regex. This could use a lot of
        memory if there are many nodes of a particular type, hence this method should be used sparingly.
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def FindType(self, request, context):
        """Given an atom name, return the atom type if it exists
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def add_AtomServerServicer_to_server(servicer, server):
    rpc_method_handlers = {
            'ExecutePattern': grpc.unary_stream_rpc_method_handler(
                    servicer.ExecutePattern,
                    request_deserializer=atom__server__pb2.PatternMsg.FromString,
                    response_serializer=atom__server__pb2.AtomMsg.SerializeToString,
            ),
            'CheckNode': grpc.unary_unary_rpc_method_handler(
                    servicer.CheckNode,
                    request_deserializer=atom__server__pb2.AtomRequest.FromString,
                    response_serializer=atom__server__pb2.NodeMsg.SerializeToString,
            ),
            'FindSimilar': grpc.unary_stream_rpc_method_handler(
                    servicer.FindSimilar,
                    request_deserializer=atom__server__pb2.AtomRequest.FromString,
                    response_serializer=atom__server__pb2.NodeMsg.SerializeToString,
            ),
            'FindType': grpc.unary_unary_rpc_method_handler(
                    servicer.FindType,
                    request_deserializer=atom__server__pb2.AtomRequest.FromString,
                    response_serializer=atom__server__pb2.NodeMsg.SerializeToString,
            ),
    }
    generic_handler = grpc.method_handlers_generic_handler(
            'AtomServer', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))


 # This class is part of an EXPERIMENTAL API.
class AtomServer(object):
    """Missing associated documentation comment in .proto file."""

    @staticmethod
    def ExecutePattern(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_stream(request, target, '/AtomServer/ExecutePattern',
            atom__server__pb2.PatternMsg.SerializeToString,
            atom__server__pb2.AtomMsg.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def CheckNode(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/AtomServer/CheckNode',
            atom__server__pb2.AtomRequest.SerializeToString,
            atom__server__pb2.NodeMsg.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def FindSimilar(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_stream(request, target, '/AtomServer/FindSimilar',
            atom__server__pb2.AtomRequest.SerializeToString,
            atom__server__pb2.NodeMsg.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def FindType(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/AtomServer/FindType',
            atom__server__pb2.AtomRequest.SerializeToString,
            atom__server__pb2.NodeMsg.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)
