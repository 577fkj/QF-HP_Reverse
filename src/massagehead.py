from ctypes import *
from common import *
import struct
M_ALL = 'M_ALL'
M_ENGINE = 'M_ENGINE'
M_DOWNLOAD_DEBUG = 'M_DOWNLOAD_DEBUG'
M_SETTING = 'M_SETTING'
M_FILE_MANAGER = 'M_FILE_MANAGER'
M_PICTURE = 'M_PICTURE'
M_VIDEO_TOOL = 'M_VIDEO_TOOL'
M_SRCEEN_SHARE = 'M_SRCEEN_SHARE'
M_HELP = 'M_HELP'
A_CLOSE_UART = 'A_CLOSE_UART'
A_OPEN_UART = 'A_OPEN_UART'

class ModuleType:

    class ConstError(TypeError):
        """M_DOWNLOAD_DEBUG"""
    MODULE_TYPE_UNKNOW = 0
    MODULE_TYPE_DOWNLOADER_TOOL = 1
    MODULE_TYPE_SH_SETTINGS = 2

    def __setattr__(self, name, value):
        raise self.ConstError(f"Can't rebind const {name}")
MT = ModuleType()

class ActionType:

    class ConstError(TypeError):
        """M_DOWNLOAD_DEBUG"""
    AT_UNKNOWN = 0
    AT_FREE_STATUS = 1
    AT_DIR_CREATE = 2
    AT_DIR_REMOVE = 3
    AT_DIR_RENAME = 4
    AT_DIR_LIST = 5
    AT_FILE_CREATE = 6
    AT_FILE_WRITE = 7
    AT_FILE_READ = 8
    AT_FILE_REMOVE = 9
    AT_FILE_RENAME = 10
    AT_FILE_GET_INFO = 11
    AT_SETTING_SET = 12
    AT_SETTING_GET = 13
    AT_MAXSIZE = 14

    def __setattr__(self, name, value):
        raise self.ConstError(f"Can't rebind const {name}")
AT = ActionType()

class ValueType:

    class ConstError(TypeError):
        """M_DOWNLOAD_DEBUG"""
    VALUE_TYPE_UNKNOWN = 0
    VALUE_TYPE_INT = 1
    VALUE_TYPE_UCHAR = 2
    VALUE_TYPE_STRING = 3
    VALUE_TYPE_MC = 4
    VALUE_TYPE_SN = 5
    VALUE_TYPE_MAXSIZE = 6

    def __setattr__(self, name, value):
        raise self.ConstError(f"Can't rebind const {name}")
VT = ValueType()

class MsgHead_TT(Structure):
    _fields_ = [('header_mark', c_byte * 2), ('from_who', c_byte), ('to_who', c_byte), ('msg_len', c_uint)]

class MsgHead:
    """
    网络通信的消息头
    """

    def __init__(self, from_who=0, to_who=0, action_type=AT.AT_UNKNOWN):
        self.header_mark = 8995
        self.msg_len = 0
        self.from_who = from_who
        self.to_who = to_who
        self.action_type = action_type
        self.fmt = '1H1H1B1B1B'

    def __dir__(self):
        return ['header_mark', 'msg_len', 'from_who', 'to_who', 'action_type']

    def decode(self, network_data, byteOrder='!'):
        """
        消息的解码，子类可以继承无需重写
        """
        members = [attr for attr in self.__dir__() if not callable(getattr(self, attr)) and (not attr.startswith('__')) and (not attr.startswith('fmt'))]
        size = struct.Struct(self.fmt).size
        get_data = struct.unpack(byteOrder + self.fmt, network_data[:size])
        for attr, value in zip(members, get_data):
            setattr(self, attr, value)
        return size

    def encode(self, byteOrder='='):
        """
        消息的编码，子类可以继承可以不重写
        """
        members = [attr for attr in self.__dir__() if not callable(getattr(self, attr))]
        params = [getattr(self, param) for param in members]
        return struct.pack(byteOrder + self.fmt, *params)

class SettingMsg(MsgHead):

    def __init__(self, action_type=AT.AT_SETTING_GET):
        MsgHead.__init__(self, MT.MODULE_TYPE_DOWNLOADER_TOOL, MT.MODULE_TYPE_SH_SETTINGS, action_type)
        self.key = b''
        self.type = b''
        self.value = b''

    def decode(self, network_data, byteOrder='!'):
        """
        消息的解码
        """
        size = super().decode(network_data, byteOrder)
        self.left_info = network_data[size:]
        print(self.left_info)
        return size

    def encode(self, byteOrder='='):
        """
        消息的编码，子类可以继承可以不重写
        """
        info = self.key + b'\x00' + self.type + self.value + b'\r\n'
        self.msg_len = struct.Struct(self.fmt).size + len(info)
        data = super().encode(byteOrder)
        data = data + info
        return data

    def __dir__(self):
        super_param = super().__dir__()
        return super_param

def dump_dict(obj):
    info = {}
    for k, v in obj._fields_:
        av = getattr(obj, k)
        if type(v) == type(Structure):
            print(av)
        elif type(v) == type(Array):
            av = cast(av, c_char_p).value.decode()
        info[k] = av
    return info