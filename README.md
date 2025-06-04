# QF-HP Reverse Engineering

https://oshwhub.com/dhx233/pcb-heng-wen-jia-re-tai

https://oshwhub.com/sheep_finder/pcb-heng-wen-jia-re-tai

```
目前事情也是告一段落了，让假开源项目变成了真开源。
如果一个开源项目因为被倒卖而设置一堆限制，那你为什么要开源？

过程：
    在好几个月前我就看到了这个项目，对他的假开源和激活方式非常不爽，购买了ESP12F开发板在2025-6-2晚上进行了激活码逆向。
    破解完成后再2025-6-3凌晨就把激活码算法上传至Github，中午在闲鱼发布了商品，随后项目开发者在群内发布我的闲鱼信息，以及我的其他平台账号信息，在群内进行辱骂，侮辱等。
    并在嘉立创、B站等平台公开我的个人账号，引导网友进行网曝、人肉搜索等操作。

目前事情已经结束，挂我信息无所谓，我打开免打扰就是了，又不影响我正常生活。
```

## Firmware source code

- [Gitee](https://gitee.com/deng-hongxiang/qf_hp_software)

## Activation

- [activation_code.py](tools/activation_code.py)

## Joke

```
激活码机制说明：

因有人利用规则谎称给朋友做送人其他尺寸等，致加热台V2版本已经出现在闲鱼上，因此规则做出以下调整：

激活码免费，单人单加热板尺寸限1台，总限2台。只能自己做，任何从咸鱼等正规平台购买个人卖家的主板、料包、成品或做给朋友等非自己手动制作者不提供激活
证明材料以下条件全部都要：
1）嘉立创客编截图必须是截图
2）提供所有器件购买订单截图（阻容可以不用），包括加热板等所有器件
3）复刻（焊接）过程照片一张,实物PCB上必须带有清晰可见的且与客编截图一致的丝印
4）从QQ群私聊AA激活码启总（你也是他人的光呀）即可，不用加好友，QQ账号不能是低等级小号（从嘉立创直接私聊提供材料也可以，但是回复周期不保证，短则几天，长则几个月）以上条件缺一不可，我也不想麻烦，奈何咸鱼泛滥~）
5）任何不看开源文档材料要求、没礼貌的纯伸手党，恕不搭理
6）要求的所有材料少一个都不回复
```

## Thanks

- [ghidra](https://github.com/NationalSecurityAgency/ghidra)
- [esp8266-elf](https://github.com/bucienator/esp8266-elf)
- [SVD-Loader-Ghidra](https://github.com/leveldown-security/SVD-Loader-Ghidra)
