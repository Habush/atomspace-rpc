from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class PatternMsg(_message.Message):
    __slots__ = ["atomspace", "query"]
    ATOMSPACE_FIELD_NUMBER: _ClassVar[int]
    QUERY_FIELD_NUMBER: _ClassVar[int]
    atomspace: str
    query: LinkMsg
    def __init__(self, atomspace: _Optional[str] = ..., query: _Optional[_Union[LinkMsg, _Mapping]] = ...) -> None: ...

class NodeMsg(_message.Message):
    __slots__ = ["type", "name"]
    TYPE_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    type: str
    name: str
    def __init__(self, type: _Optional[str] = ..., name: _Optional[str] = ...) -> None: ...

class LinkMsg(_message.Message):
    __slots__ = ["type", "outgoing"]
    TYPE_FIELD_NUMBER: _ClassVar[int]
    OUTGOING_FIELD_NUMBER: _ClassVar[int]
    type: str
    outgoing: _containers.RepeatedCompositeFieldContainer[AtomMsg]
    def __init__(self, type: _Optional[str] = ..., outgoing: _Optional[_Iterable[_Union[AtomMsg, _Mapping]]] = ...) -> None: ...

class AtomMsg(_message.Message):
    __slots__ = ["link", "node"]
    LINK_FIELD_NUMBER: _ClassVar[int]
    NODE_FIELD_NUMBER: _ClassVar[int]
    link: LinkMsg
    node: NodeMsg
    def __init__(self, link: _Optional[_Union[LinkMsg, _Mapping]] = ..., node: _Optional[_Union[NodeMsg, _Mapping]] = ...) -> None: ...

class AtomRequest(_message.Message):
    __slots__ = ["atomspace", "atom"]
    ATOMSPACE_FIELD_NUMBER: _ClassVar[int]
    ATOM_FIELD_NUMBER: _ClassVar[int]
    atomspace: str
    atom: NodeMsg
    def __init__(self, atomspace: _Optional[str] = ..., atom: _Optional[_Union[NodeMsg, _Mapping]] = ...) -> None: ...
