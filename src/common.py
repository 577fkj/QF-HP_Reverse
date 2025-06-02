import binascii
import ctypes
import inspect
import traceback
import re
VERSION = 'Ver1.0.5'
ROOT_PATH = 'OutFile'
CACHE_PATH = 'Cache'
byteOrders = {'Native order': '@', 'Native standard': '=', 'Little-endian': '<', 'Big-endian': '>', 'Network order': '!'}

def getSendInfo(info):
    """
    打印网络数据流, 
    :param info: ctypes.create_string_buffer()
    :return : str
    """
    info = binascii.hexlify(info)
    print(info)
    re_obj = re.compile('.{1,2}')
    t = ' '.join(re_obj.findall(str(info).upper()))
    return t

def _async_raise(thread_obj):
    """
    释放进程
    :param thread: 进程对象
    :param exctype:
    :return:
    """
    try:
        tid = thread_obj.ident
        tid = ctypes.c_long(tid)
        exctype = SystemExit
        if not inspect.isclass(exctype):
            exctype = type(exctype)
        res = ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, ctypes.py_object(exctype))
        if res == 0:
            raise ValueError('invalid thread id')
        if res != 1:
            ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, None)
            raise SystemError('PyThreadState_SetAsyncExc failed')
    except Exception as err:
        print(err)

def kill_thread(h_thread, stoptype):
    import inspect
    import ctypes
    try:
        tid = ctypes.c_long(h_thread.ident)
        if not inspect.isclass(stoptype):
            stoptype = type(stoptype)
        res = ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, ctypes.py_object(stoptype))
        if res == 0:
            raise ValueError('invalid thread id')
        if res != 1:
            ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, None)
            raise SystemError('kill_thread failed')
        return res
    except Exception as e:
        print(e)