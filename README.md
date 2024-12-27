# 时光磁带（ESP32 S3 MP3播放器）
- - <img src="Readme image/3Dimage.png" alt="Description" width="10000">
# [中文版](README.md) / [English Version](README.en.md)
## 目录
- [概述](#概述)
- [特性](#特性)
- [硬件型号](#硬件型号)
- [原理图](#原理图)
- [如何编译软件部分](#如何编译软件部分)
- [固件](#固件)
- [代码](#代码)
- [贡献](#贡献)
- [联系](#联系)
## 概述
本项目是一个提供硬件及软件的指南，用于构建基于ESP32 S3微控制器的MP3播放器。该设备拥有磁带复古外形的外壳，具备音乐播放、时间显示。

## 特性
该MP3播放器基于ESP32 S3微控制器，集成了2.4 GHz Wi-Fi和蓝牙5 (LE)，配备1.14英寸TFT LCD屏幕显示界面、BM8563 RTC芯片离线时间功能、WM8978 DAC高保真音频输出、NS4150B功率放大器增强音质、ETA6002充电芯片支持快速充电，同时支持USB Type-C充电和数据传输、SD/TF卡扩展存储（最高32G）、3.5mm耳机插孔以及高品质超薄扬声器，满足多样化的音乐播放需求。
## 硬件型号
- ESP32 S3：主要处理单元，集成Wi-Fi和蓝牙。
- TFT LCD（1.14英寸）：显示模块，用于用户界面。
- BM8563 RTC：实时时钟芯片，用于离线时间。
- WM8978 DAC：音频转换芯片，用于高保真声音。
- NS4150B PA：功率放大器芯片，用于音频输出。
- ETA6002 充电：锂电池充电芯片，用于锂电池充电。
- USB Type-C端口：用于充电和数据传输。
- SD/TF卡读卡器：用于存储扩展。
- 耳机插孔：3.5mm音频输出。
- 扬声器：提供音频播放。
- 按钮：具有5个按钮对UI进行交互控制。
- 开关：拥有1个开关控制电源开启关闭。

## 原理图 
- <img src="Readme image/Schematic.png" alt="Description" width="10000">
### 主要原理图解析
- **复位按钮**：EN引脚是ESP32 S3核心的复位引脚，采用按钮形式下拉接地该引脚可复位。按钮增加电容和电阻是为了去除电平毛刺，避免出现错误复位。
  > <img src="Readme image/ENbutton.png" alt="Description" width="200">
- **操作按钮**：IO_0为ESP32 S3的切换下载模式的功能按钮也可以是作为GUI操作反馈，其他按钮为GUI操作反馈。
  > <img src="Readme image/ENbutton2.png" alt="Description" width="200">
- ESP32 S3 引脚分配表:
> 核心MCU接线说明：
> 如图所示，这是引脚的分配，核心有两个滤波电容靠近核心，是为了过滤电源线的干扰，保证核心供电稳定。其中引脚分配如下：

| **引脚编号** | **功能描述**      | **备注**                           |
|--------------|-------------------|------------------------------------|
| IO45         | NS_CON           | 对NS4150B的音频输出是否静音进行控制（高电平为允许声音输出）|
| IO47         | IN               | 检测3.5MM耳机是否插入，如插入就是低电平                  |
| IO40         | TF_BLK           | 通过PWM信号进行控制屏幕亮度                             |
| IO46         | RT_A3V3_EN       | 音频的电压稳定芯片的开关（上拉给予3V3电压为开启）         |
| IO9          | I2S_BCK          | 音频接口（I2S）位时钟线                                 |
| IO10         | I2S_WS           | 音频接口（I2S）音频左右声数据切换|
| IO11         | I2S_DOUT         | 音频接口（I2S）WM8978芯片数据至ESP32 S3|
| IO12         | I2S_DIN          | 音频接口（I2S）数据至WN8978芯片 |
| IO41         | I2S_MCLK         | 音频接口（I2S）额外频率时钟参考线线|
| IO15         | SPI_CS           | SPI接口                           |
| IO16         | SPI_RST          | SPI接口                           |
| IO17         | SPI_DC           | SPI接口                           |
| IO18         | SPI_SCLK         | SPI接口                           |
| IO39         | SPI_SDA/MOSI     | SPI接口                           |
| IO13         | TF_CLK           | TF卡/SD接口                          |
| IO14         | TF_MOSI          | TF/SD卡接口                          |
| IO7          | TF_CS            | TF/SD卡接口                          |
| IO48         | TF_DATA2         | TF/SD卡接口                          |
| IO2          | TF_DATA1         | TF/SD卡接口                          |
| IO21         | SW_UP            | 按键                        |
| IO20         | SW_DOWN          | 按键                        |
| IO19         | SW_RIGHT         | 按键                        |
| IO8          | SW_LEFT          | 按键                        |
| IO0          | IO0              | 按键/切换下载模式                  |
| IO4          | ADC_BAT          | 电池电量检测                      |
| IO2          | I2C_SCL          | I2C接口                           |
| IO3          | I2C_SDA          | I2C接口                           |
| IO8          | RTC_INT          | 实时时钟中断接口                  |
| IO19          | USB_D-          | USB数据/type C物理接口连接                 |
| IO20          | USB_D+          | USB数据/type C物理接口连接                |
- 其中仍然未用到的引脚有TXD0、RXD0原本是用于调试接口；但因为ESP32 S3支持USB日志调试，因此烧录固件可以不用单独的自动下载电路来进行烧录固件。其次关于IO37、IO36、IO35为芯片flash引脚所占用的，所以为了谨慎起见不进行使用。如果想要了解更多，请参考乐鑫官方的ESP32 S3数据手册，有你想知道的一切！
> <img src="Readme image/ESP32S3.png" alt="引脚分配图" width="600">
- **WM8978 DAC**: A3V3 是提供给模拟电路部分的电源。通过 C48（100nF）和 C3（10uF）电容进行滤波，3V3 为数字部分供电，通过 C49 和 C50（100nF 和 10uF）进行滤波，SPKVDD 为扬声器供电，通过 C19 和 C20（220uF）进行电源滤波。
 I2C_SCL 和 I2C_SDA 引脚（引脚 16 和 17）通过 I2C 接口控制 WM8978 的配置，EAR_LOUT1 和 EAR_ROUT1 是左右声道的耳机输出引脚，他们经过钽电容输出到耳机接口（注意建议使用钽电容，假如因为成本可以换成陶瓷电容）。SPK+ 和 SPK- 是扬声器的输出引脚，支持单声道扬声器，分别通过 AGND 接地，构成差分驱动的扬声器信号。
> <img src="Readme image/WM8978.png" alt="引脚分配图" width="600">
- **NS4150B和3.5MM**：该部分电路为音频放大器以及3.5MM接口电路，在3.5MM接口上R3、C15为滤波和限流是为了上拉该引脚的电平，当有耳机插入时耳机的线路会把引脚接地，这样就可以检测是否有耳机插入。当插入时就可以切换音频输出的模式。注意：IN检测电路部分我使用了一个切换焊盘，这个切换焊盘用于测试用途，默认需要把2-3点连接，可采用0R电阻进行连接。在NS4150B芯片上NS_CON为音频输出是否静音进行控制（高电平为允许声音输出），芯片的3号4号引脚是DAC的模拟音频输入，8号和5号是放大后的音频输出
> <img src="Readme image/speaker_headphone.png" alt="引脚分配图" width="600">
- **BM8563**：RTC模块通过一个32.768kHz的晶振器提供时钟信号，并通过I2C接口与主控制器通信。RTC模块的电源引脚通过一个10uF的电容进行滤波，而地引脚接地。XC6206P332MR电源管理芯片负责为主RTC模块供电，它从电池（VBAT）获取电源并通过内部稳压器输出稳定的RTC_3V3电压。INT引脚为中断引脚，如果该引脚被拉低，就会触发ESP32 S3的中断，用于定时任务策略。整个设计中，多个接地电容用于稳定电源和减少噪声，确保RTC模块能在主电源断开时继续运行。
> <img src="Readme image/BM8563.png" alt="引脚分配图" width="400">
- **TF/SD 卡**:该电路图是一个SD/TF卡电路，通过10kΩ上拉电阻将TF_DATA2、TF_CS、TF_MOSI、TF_MISO、TF_DATA1和TF_CLK等信号线连接到3.3V电源，确保信号传输稳定。所有信号线引出后，使用者可以使用SDIO模式或者SPI模式访问储存卡。
> <img src="Readme image/SD.png" alt="引脚分配图" width="400">
- **ETA6002**:电路通过VBUS引脚接收USB电源输入，并利用电容进行滤波和稳定。SW引脚通过2.2uH电感和2.2k电阻控制充电芯片的开关操作。VBAT引脚连接到电池正极同时VBAT会利用ADC进行监测电池电压判断电量。SYS_OUT引脚提供稳定的系统输出电压给LDO，除此之外LED和2.2k电阻指示充电状态。NTC引脚用于热管理（但因为不用所以直接接地），ISET引脚通过470电阻设置充电电流（2A）。
> <img src="Readme image/ETS6002.png" alt="引脚分配图" width="400">
## 如何开始硬件部分
- 本项目的PCB设计文件在Schematic内，里面提供了PCB的制版文件，可以通过**嘉立创**进行制作你的电路板。
- 除此之外在Schematic内提供ibom的HTML页面，方便您进行购买相关元器件。
### PCB模型预览

### 硬件部分注意事项
暂无（等候补充）
## 如何编译软件部分
- 本项目使用**Arduino IDE 2.3.4**版本进行编译，确保使用乐鑫官方的SDK版本为**2.0.14**（注意因为3.0.X版本会让工程编译产生未知的错误，为了避免请下载安装对应的SDK版本）
- 开发板的flash容量的分配16M Flash (6.25M APP/6.25MB LittleFS)，因此需要自行修改，具体修改需要对Arduino安装目录进行删除和修改文件，这里提供方法：
> 打开文件路径
```
C:\Users\（你的windows账户用户名）\AppData\Roaming\arduino-ide\Local Storage\leveldb
```
- 把关于.ldb格式文件进行删除，例如：000062.ldb（编号是随机的，你并不一定是这个文件名，只需要看文件格式进行删除）
> 打开文件路径
```
C:\Users\（你的windows账户用户名）\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.14
```
- 打开boards.txt文件，寻找esp32s3.menu.PartitionScheme关键字信息，并在末尾添加并保存
```boards.txt
esp32s3.menu.PartitionScheme.myMP3=MP3 16M Flash (6.25M APP/6.25MB LittleFS)
esp32s3.menu.PartitionScheme.myMP3.build.partitions=big
esp32s3.menu.PartitionScheme.myMP3.upload.maximum_size=6553600
```
> 打开文件路径
```
C:\Users\你的windows账户用户名）\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.14\tools\partitions
```
- 新建添加big.csv文件，并写入以下内容为flash的分区进行配置
```plaintext
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x5000,
otadata,  data, ota,     0xe000,  0x2000,
app0,     app,  ota_0,   0x10000, 0x640000,
app1,     app,  ota_1,   0x650000,0x640000,
spiffs,   data, spiffs,  0xc9000,0x360000,
coredump, data, coredump,0xFF0000,0x10000,
```
### 开发板配置信息

| 项目                     | 详情                                      |
|--------------------------|-------------------------------------------|
| 开发板                   | ESP32S3 Dev Module                     |
| 端口                    | COM4                                     |
| USB CDC On Boot             | Enabled                                  |
| CPU Frequency            | 240MHz (WiFi)                             |
| Core Debug Level           | None                                    |
| USB DFU On Boot            | Disabled                                |
| Erase All Flash Before Sketch Upload | Disabled                               |
| Events Run On             | Core 1                                   |
| Flash Mode                | QIO 80MHz                                |
| Flash Size               | 16MB (128Mb)                             |
| JTAG Adapter            | Integrated USB JTAG                       |
| Arduino Runs On          | Core 1                                   |
| USB Firmware MSC On Boot   | Disabled                               |
| Partition Scheme          | MP3 16M Flash (6.25M APP/6.25MB LittleFS) |
| PSRAM                   | OPI PSRAM                                 |
| Upload Mode              | UART0 / Hardware CDC                  |
| Upload Speed             | 921600                                  |
| USB Mode                | Hardware CDC and JTAG                 |
| 编程器                  | Esptool                                  |

### Arduino IDE 和 SDK 版本信息

| 项目                     | 详情                                      |
|--------------------------|-------------------------------------------|
| Arduino IDE 版本           | 2.3.4                                   |
| SDK 版本                | 2.0.14                                   |

### 进行编译
确保编译环境一致之后，请下载GitHub本项目中的src文件夹，里面的library是所需的第三方库，这些第三方库我都进行了修改，因此和原生的第三方库都不会进行兼容。按照开源协议，我需要把自己更改的库开放出来，并注明出处。
- 打开文件路径
```text
C:\Users\（你的windows账户用户名）\Documents\Arduino\libraries
```
- 把本项目的libraries复制到你所打开本地的文件路径（注意如果本地文件路径相同名字相同的话，请在复制粘贴之前先删除本地文件路径相同的第三方库，避免造成冲突与不兼容）
- 最后请下载GitHub本项目中的ESP32_MP3_PLAYER文件夹，打开**ESP32_MP3_PLAYER.ino**，点击上传编译到板子
- 如果发生编译错误，请通过联系的联系方式告知我！使用QQ联系维护者2277236944或发送Email至Oliver.yuchunto@gmail.com
## 固件
本项目的固件是开源的，可以在本仓库的主分支中找到。为.bin格式文件，请自行google怎么直接烧录固件。
## 代码
## 贡献
欢迎贡献！如果您发现任何问题或有改进建议，请提出问题或提交拉取请求。或者直接联系我进行沟通。
## 许可证
本项目根据GNU通用公共许可证协议，只要您遵守许可证的条款，就可以自由使用、修改和分发代码，但出处必须！必须！必须！注明本项目guthub来源网址链接或注明作者：一条优秀的闲鱼呀！

## 联系
> 如有任何问题或咨询
- **推荐加入QQ群聊**：253739646
- **个人QQ**：2277236944
- **发送Email**：Oliver.yuchunto@gmail.com
