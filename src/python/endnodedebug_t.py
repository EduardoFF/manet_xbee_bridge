"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class endnodedebug_t(object):
    __slots__ = ["nodeid", "n_xbee_pkts_sent", "n_xbee_bytes_sent", "n_xbee_pkts_rcv", "n_xbee_bytes_rcv", "timestamp", "last_flow_notify_time", "manet_alive"]

    def __init__(self):
        self.nodeid = 0
        self.n_xbee_pkts_sent = 0
        self.n_xbee_bytes_sent = 0
        self.n_xbee_pkts_rcv = 0
        self.n_xbee_bytes_rcv = 0
        self.timestamp = 0
        self.last_flow_notify_time = 0
        self.manet_alive = 0

    def encode(self):
        buf = BytesIO()
        buf.write(endnodedebug_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">BhihiqhB", self.nodeid, self.n_xbee_pkts_sent, self.n_xbee_bytes_sent, self.n_xbee_pkts_rcv, self.n_xbee_bytes_rcv, self.timestamp, self.last_flow_notify_time, self.manet_alive))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != endnodedebug_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return endnodedebug_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = endnodedebug_t()
        self.nodeid, self.n_xbee_pkts_sent, self.n_xbee_bytes_sent, self.n_xbee_pkts_rcv, self.n_xbee_bytes_rcv, self.timestamp, self.last_flow_notify_time, self.manet_alive = struct.unpack(">BhihiqhB", buf.read(24))
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if endnodedebug_t in parents: return 0
        tmphash = (0x2316b30b8f7e4d68) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if endnodedebug_t._packed_fingerprint is None:
            endnodedebug_t._packed_fingerprint = struct.pack(">Q", endnodedebug_t._get_hash_recursive([]))
        return endnodedebug_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)
