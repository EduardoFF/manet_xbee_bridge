"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class pose_t(object):
    __slots__ = ["robotid", "position", "orientation", "velocity"]

    def __init__(self):
        self.robotid = 0
        self.position = [ 0 for dim0 in range(3) ]
        self.orientation = [ 0 for dim0 in range(4) ]
        self.velocity = 0

    def encode(self):
        buf = BytesIO()
        buf.write(pose_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">B", self.robotid))
        buf.write(struct.pack('>3i', *self.position[:3]))
        buf.write(struct.pack('>4h', *self.orientation[:4]))
        buf.write(struct.pack(">h", self.velocity))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != pose_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return pose_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = pose_t()
        self.robotid = struct.unpack(">B", buf.read(1))[0]
        self.position = struct.unpack('>3i', buf.read(12))
        self.orientation = struct.unpack('>4h', buf.read(8))
        self.velocity = struct.unpack(">h", buf.read(2))[0]
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if pose_t in parents: return 0
        tmphash = (0x23a1414b9b549cfd) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if pose_t._packed_fingerprint is None:
            pose_t._packed_fingerprint = struct.pack(">Q", pose_t._get_hash_recursive([]))
        return pose_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

