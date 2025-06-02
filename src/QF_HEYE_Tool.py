global main_app_rules
import sys
import os
import time
import threading
import re
import yaml
import requests
import traceback
import serial
import serial.tools.list_ports
from PyQt5.Qt import QWidget, QApplication
from PyQt5 import uic, QtCore
from PyQt5.QtWidgets import QMessageBox, QApplication, QMainWindow, QFileDialog
from PyQt5.QtCore import Qt
import massagehead as mh
import esptool
from download import Ui_SanilHeaterTool
import common
SH_SN = None
if SH_SN == None and os.path.exists('SnailHeater_SN.py'):
    import SnailHeater_SN as SH_SN
    print('激活模块已添加')
COLOR_RED = '<span style=" color: #ff0000;">%s</span>'
BAUD_RATE = 921600
INFO_BAUD_RATE = 115200
cur_dir = os.getcwd()
cfg_fp = open('config.yaml', 'r', encoding='utf-8')
win_cfg = yaml.load(cfg_fp, Loader=yaml.SafeLoader)['windows_tool']
tool_open_url = win_cfg['tool_open_url'] if 'tool_open_url' in win_cfg.keys() else 'https://github.com/ClimbSnail'
tool_name = win_cfg['tool_name'] if 'tool_name' in win_cfg.keys() else '未命名工具'
info_url_0 = win_cfg['info_url_0'] if 'info_url_0' in win_cfg.keys() else ''
info_url_1 = win_cfg['info_url_1'] if 'info_url_1' in win_cfg.keys() else ''
qq_info = win_cfg['qq_info'].split(',') if 'qq_info' in win_cfg.keys() else ['', '']
activate = win_cfg['activate'] if 'activate' in win_cfg.keys() else True
empty_burn_enable = win_cfg['empty_burn_enable'] if 'empty_burn_enable' in win_cfg.keys() else True
firmware_info_list = win_cfg['firmware_info_list'] if 'firmware_info_list' in win_cfg.keys() else []
main_app_addr = win_cfg['main_app_addr'] if 'main_app_addr' in win_cfg.keys() else ''
main_app_rules = win_cfg['main_app_rules'] if 'main_app_rules' in win_cfg.keys() else ''
temp_sn_recode_path = win_cfg['temp_sn_recode_path'] if 'temp_sn_recode_path' in win_cfg.keys() else cur_dir
cfg_fp.close()
GET_ID_INFO = 'get_id\r\n'
GET_ID_INFO_OK = 'get_id_ok \\S*'
GET_SN_INFO = 'get_sn\r\n'
GET_SN_INFO_OK = 'get_sn_ok \\S*'
SET_SN_INFO = b'set_sn %s\r\n'
SET_SN_INFO_OK = 'set_sn_ok'
default_wallpaper_280 = os.path.join(cur_dir, './base_data/Wallpaper_280x240.lsw')

class DownloadController(object):

    def __init__(self):
        self.progress_bar_time_cnt = 0
        self.ser = None
        self.progress_bar_timer = QtCore.QTimer()
        self.progress_bar_timer.timeout.connect(self.schedule_display_time)
        self.download_thread = None

    def run(self):
        """
        下载页面的主界面生成函数
        :return:
        """
        self.app = QApplication(sys.argv)
        self.win_main = QWidget()
        self.form = Ui_SanilHeaterTool()
        self.form.setupUi(self.win_main)
        _translate = QtCore.QCoreApplication.translate
        self.win_main.setWindowTitle(_translate('SanilHeaterTool', tool_name + common.VERSION))
        self.form.Infolabel_0.setTextInteractionFlags(Qt.TextSelectableByMouse)
        self.form.Infolabel_1.setTextInteractionFlags(Qt.TextSelectableByMouse)
        self.form.QQInfolabel.setTextInteractionFlags(Qt.TextSelectableByMouse)
        self.form.QQInfolabel_2.setTextInteractionFlags(Qt.TextSelectableByMouse)
        self.form.OpenUrl.setTextInteractionFlags(Qt.TextSelectableByMouse)
        self.form.ComComboBox.clicked.connect(self.scan_com)
        self.form.FirmwareComboBox.clicked.connect(self.scan_firmware)
        self.form.QueryPushButton.clicked.connect(self.query_button_click)
        self.form.ActivatePushButton.clicked.connect(self.act_button_click)
        self.form.UpdatePushButton.clicked.connect(self.update_button_click)
        self.form.CanclePushButton.clicked.connect(self.cancle_button_click)
        self.form.VerInfolabel.setStyleSheet('color: red')
        self.form.QQInfolabel.setText(_translate('SanilHeaterTool', qq_info[0]))
        self.form.QQInfolabel_2.setText(_translate('SanilHeaterTool', qq_info[1]))
        self.form.Infolabel_0.setText(_translate('SanilHeaterTool', info_url_0))
        self.form.Infolabel_1.setText(_translate('SanilHeaterTool', info_url_1))
        self.form.OpenUrl.setText(_translate('SanilHeaterTool', tool_open_url))
        self.form.UICLineEdit.setReadOnly(True)
        print('activate', activate)
        self.form.QueryPushButton.setEnabled(activate)
        self.form.ActivatePushButton.setEnabled(activate)
        self.form.ClearModeMethodRadioButton.setEnabled(empty_burn_enable)
        self.win_main.show()
        sys.exit(self.app.exec_())

    def scan_com(self):
        """
        搜索串口
        """
        self.print_log('搜索串口号...')
        self.form.ComComboBox.clear()
        com_obj_list = list(serial.tools.list_ports.comports())
        com_list = []
        for com_obj in com_obj_list:
            com_num = com_obj[0]
            info = com_obj[1].split('(')
            com_info = com_obj[1].split('(')[0].strip()
            com_list.append(com_num + ' -> ' + com_info)
        if com_list == []:
            com_list = ['未识别到']
        self.form.ComComboBox.addItems(com_list)

    def scan_firmware(self):
        """
        搜索固件
        """
        self.print_log('搜索同目录下的可用固件...')
        self.form.FirmwareComboBox.clear()
        list_file = os.listdir('./')
        firmware_path_list = []
        for file_name in list_file:
            if main_app_rules in file_name:
                firmware_path_list.append(file_name.strip())
        if len(firmware_path_list) == 0:
            firmware_path_list = ['未找到固件']
        self.form.FirmwareComboBox.addItems(firmware_path_list)

    def getSafeCom(self):
        """
        获取安全的串口
        :return: Com / None
        """
        if self.ser != None:
            return
        select_com = self.form.ComComboBox.currentText().split(' -> ')[0].strip()
        com_list = [com_obj[0] for com_obj in list(serial.tools.list_ports.comports())]
        if select_com not in com_list:
            self.print_log(COLOR_RED % '错误提示：' + '无法检测到指定串口设备，先确认 CH340 驱动是否正常或尝试 typec 调换方向。\n')
            return
        return select_com

    def act_button_click(self):
        self.print_log('正在激活设备...')
        select_com = self.getSafeCom()
        if select_com == None:
            self.print_log(COLOR_RED % '激活操作异常，激活中止...')
            return
        self.ser = serial.Serial(select_com, INFO_BAUD_RATE, timeout=10)
        act_ret = False
        if self.ser.is_open:
            search_info = SET_SN_INFO % bytes(self.form.SNLineEdit.text().strip(), encoding='utf8')
            SET_SN_INFO
            print(search_info)
            self.ser.write(search_info)
            time.sleep(1)
            if self.ser.in_waiting:
                try:
                    STRGLO = self.ser.read(self.ser.in_waiting)
                    print('\nSTRGLO = ', STRGLO)
                    match_info = re.findall(SET_SN_INFO_OK, STRGLO.decode('utf8'))
                    if match_info != []:
                        act_ret = True
                except Exception as err:
                    print(str(traceback.format_exc()))
            if act_ret == True:
                self.print_log('激活成功')
            else:
                self.print_log('激活失败')
        self.ser.close()
        del self.ser
        self.ser = None

    def query_button_click(self):
        """
        获取用户识别码 显示在用户识别码的信息框里
        获取激活码 显示在激活码的信息框里
        :return: None
        """
        self.print_log('获取机器码（用户识别码）...')
        machine_code = self.get_machine_code()
        self.form.UICLineEdit.setText(machine_code)
        self.print_log('\n获取本地激活码（SN）...')
        sn = self.get_sn()
        self.form.SNLineEdit.setText(sn)

        try:
            if sn != '':
                sn_record = open(temp_sn_recode_path, 'a', encoding='utf-8')
                sn_record.write(machine_code + '\t' + sn + '\n')
                sn_record.close()
        except Exception as err:
            print(str(traceback.format_exc()))
            self.print_log('获取异常异常')

    def update_button_click(self):
        """
        按下 刷机 按键后触发的检查、刷机操作
        :return: None
        """
        self.print_log('准备更新固件...')
        self.form.UpdateModeMethodRadioButton.setEnabled(False)
        self.form.ClearModeMethodRadioButton.setEnabled(False)
        self.form.UpdatePushButton.setEnabled(False)
        firmware_path = self.form.FirmwareComboBox.currentText().strip()
        mode = '更新式' if self.form.UpdateModeMethodRadioButton.isChecked() else '清空式'
        select_com = self.getSafeCom()
        if select_com == None or firmware_path == '':
            if firmware_path == '':
                self.print_log(COLOR_RED % '错误提示：' + '未查询到固件文件！')
            self.form.UpdatePushButton.setEnabled(True)
            self.form.UpdateModeMethodRadioButton.setEnabled(True)
            self.form.ClearModeMethodRadioButton.setEnabled(empty_burn_enable and True)
            return False
        self.print_log('串口号：' + COLOR_RED % select_com)
        self.print_log('固件文件：' + COLOR_RED % firmware_path)
        self.print_log('刷机模式：' + COLOR_RED % mode)
        all_time = 0
        if mode == '清空式':
            all_time += 24
        else:
            all_time += 5
        if firmware_info_list != None and firmware_info_list != []:
            file_list = [bin_obj['filepath'] for bin_obj in firmware_info_list]
            file_list.append(firmware_path)
            for filepath in file_list:
                all_time = all_time + os.path.getsize(filepath) * 10 / BAUD_RATE
        self.print_log('刷机预计需要：' + COLOR_RED % (str(all_time)[0:5] + 's'))
        self.download_thread = threading.Thread(target=self.down_action, args=(mode, select_com, firmware_path))
        self.progress_bar_timer.start(int(all_time / 0.1))
        self.download_thread.setDaemon(True)
        self.download_thread.start()

    def down_action(self, mode, select_com, firmware_path):
        """
        下载操作主体
        :param mode:下载模式
        :param select_com:串口号
        :param firmware_path:固件文件路径
        :return:None
        """
        try:
            if self.ser != None:
                return
            self.ser = 1
            self.progress_bar_time_cnt = 1
            if mode == '清空式':
                self.print_log('正在清空主机数据...')
                cmd = ['--port', select_com, 'erase_flash']
                try:
                    esptool.main(cmd)
                    self.print_log('完成清空！')
                except Exception as e:
                    self.print_log(COLOR_RED % '错误：通讯异常。')
            cmd = ['--port', select_com, '--baud', str(BAUD_RATE), '--after', 'hard_reset', 'write_flash', main_app_addr, firmware_path]
            if firmware_info_list != None and firmware_info_list != []:
                for bin_obj in firmware_info_list:
                    cmd.append(bin_obj['addr'])
                    cmd.append(bin_obj['filepath'])
            print('cmd = ' + str(cmd))
            self.print_log('开始刷写固件...')
            try:
                esptool.main(cmd)
            except Exception as e:
                self.print_log(COLOR_RED % '错误：通讯异常。')
                return False
            self.ser = None
            self.print_log(COLOR_RED % '刷机结束！')
        except Exception as err:
            self.ser = None
            self.print_log(COLOR_RED % '未释放资源，请15s后再试。如无法触发下载，拔插type-c接口再试。')
            print(err)
        self.progress_bar_time_cnt = 0
        self.form.UpdatePushButton.setEnabled(True)
        self.form.UpdateModeMethodRadioButton.setEnabled(True)
        self.form.ClearModeMethodRadioButton.setEnabled(empty_burn_enable and True)

    def cancle_button_click(self):
        """
        取消下载固件
        :return: None
        """
        self.print_log('手动停止更新固件...')
        if self.download_thread != None:
            try:
                common._async_raise(self.download_thread)
                self.download_thread = None
            except Exception as err:
                print(err)
        self.scan_com()
        self.progress_bar_time_cnt = 0
        self.form.progressBar.setValue(0)
        self.form.UpdatePushButton.setEnabled(True)
        self.form.UpdateModeMethodRadioButton.setEnabled(True)
        self.form.ClearModeMethodRadioButton.setEnabled(empty_burn_enable and True)

    def get_firmware_version(self):
        """
        获取最新版
        """
        new_ver = None
        try:
            self.print_log('联网查询最新固件版本...')
            response = requests.get(get_firmware_new_ver_url, timeout=3)
            if 'SnailHeater_v' in response.text.strip() or 'SH_SW_v' in response.text.strip():
                new_ver = response.text.strip()
                self.form.VerInfolabel.setText('最新固件版本 ' + str(new_ver))
                self.print_log('最新固件版本 ' + COLOR_RED % str(new_ver))
            else:
                self.print_log(COLOR_RED % '最新固件版本查询异常')
        except Exception as err:
            print(str(traceback.format_exc()))
            self.print_log(COLOR_RED % '联网异常')
        return new_ver

    def get_machine_code(self):
        """
        查询机器码
        """
        select_com = self.getSafeCom()
        if select_com == None:
            return
        self.ser = serial.Serial(select_com, INFO_BAUD_RATE, timeout=10)
        machine_code = '查询失败'
        if self.ser.is_open:
            search_info = bytes(GET_ID_INFO, encoding='utf8')
            print(search_info)
            self.print_log('write start')
            self.ser.write(search_info)
            self.print_log('write OK')
            time.sleep(1)
            if self.ser.in_waiting:
                try:
                    STRGLO = self.ser.read(self.ser.in_waiting).decode('utf8')
                    print(STRGLO)
                    machine_code = re.findall(GET_ID_INFO_OK, STRGLO)[0].split(' ')[-1]
                except Exception as err:
                    machine_code = '查询失败'
                print(machine_code)
            if machine_code == '查询失败':
                self.print_log(COLOR_RED % '机器码查询失败')
            else:
                self.print_log('机器码查询成功')
        self.ser.close()
        del self.ser
        self.ser = None
        return machine_code

    def get_sn(self):
        """
        查询SN
        """
        select_com = self.getSafeCom()
        if select_com == None:
            return
        self.ser = serial.Serial(select_com, INFO_BAUD_RATE, timeout=10)
        sn = ''
        if self.ser.is_open:
            search_info = bytes(GET_SN_INFO, encoding='utf8')
            print(search_info)
            self.ser.write(search_info)
            time.sleep(1)
            if self.ser.in_waiting:
                try:
                    STRGLO = self.ser.read(self.ser.in_waiting).decode('utf8')
                    print(STRGLO)
                    sn = re.findall(GET_SN_INFO_OK, STRGLO)[0].split(' ')[-1]
                except Exception as err:
                    sn = ''
                print(sn)
            if sn == '':
                self.print_log(COLOR_RED % 'SN查询失败')
            else:
                self.print_log('SN查询成功')
        self.ser.close()
        del self.ser
        self.ser = None
        return sn

    def print_log(self, info):
        self.form.LogInfoTextBrowser.append(info + '\n')
        QApplication.processEvents()

    def esp_reboot(self):
        """
        重启芯片(控制USB-TLL的rst dst引脚)
        :return:
        """
        time.sleep(0.1)
        select_com = self.getSafeCom()
        if select_com == None:
            return
        self.ser = serial.Serial(select_com, BAUD_RATE, timeout=10)
        self._setDTR(False)
        self._setRTS(True)
        time.sleep(0.1)
        self._setDTR(True)
        self._setRTS(False)
        time.sleep(0.05)
        self._setDTR(False)
        self.ser.close()
        del self.ser
        self.ser = None

    def schedule_display_time(self):
        if self.progress_bar_time_cnt > 0 and self.progress_bar_time_cnt < 99:
            self.progress_bar_time_cnt += 1
        self.form.progressBar.setValue(self.progress_bar_time_cnt)

    def UpdatePushButton_show_message(self):
        """
        警告拔掉AC220V消息框
        :return: None
        """
        self.mbox = QMessageBox(QMessageBox.Warning, '重要提示', COLOR_RED % '刷机一定要拔掉220V电源线！')
        do = self.mbox.addButton('确定', QMessageBox.YesRole)
        cancle = self.mbox.addButton('取消', QMessageBox.NoRole)
        self.mbox.setIcon(2)
        do.clicked.connect(self.update_button_click)
        self.mbox.show()

def main():
    app = QApplication(sys.argv)
    download_ui = uic.loadUi('download.ui')
    download_ui.show()
    app.exec_()
if __name__ == '__main__':
    QApplication.setHighDpiScaleFactorRoundingPolicy(Qt.HighDpiScaleFactorRoundingPolicy.PassThrough)
    QtCore.QCoreApplication.setAttribute(Qt.AA_EnableHighDpiScaling)
    QApplication.setAttribute(Qt.AA_EnableHighDpiScaling)
    downloader = DownloadController()
    downloader.run()