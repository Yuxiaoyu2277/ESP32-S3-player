#include <Arduino.h>
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>
#include <SPI.h>
#include <SD.h>
#include <WM8978.h>
#include <Audio.h>
#include <Button2.h>
#include <vector>
#include "background.h"  //背景图
#include "music_icon.h"  //音乐图标
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
// 按钮引脚定义
#define SW_MENU 0
#define SW_CONF 4
#define SW_BACK 5
#define SW_RIGHT 1
#define SW_LEFT 7
#define TFT_BCK 40  // 屏幕背光控制引脚

// SD卡引脚定义
#define TF_CLK 13
#define TF_CMD 14
#define TF_CS 42
#define TF_DATA0 21
#define TF_DATA1 6
#define TF_DATA2 48

// IIS初始化引脚定义
#define I2S_DOUT 10
#define I2S_BCLK 9
#define I2S_WS 11
#define I2S_MCLKPIN 41
#define I2S_DIN 12

// IIC引脚定义
#define I2C_SDA 3
#define I2C_SCL 2

#define NS_CON 45
#define RT_A3V3_EN 46

// 创建 TFT 屏幕对象和精灵对象
TFT_eSPI tft = TFT_eSPI(135, 240);
TFT_eSprite sprite = TFT_eSprite(&tft);
U8g2_for_TFT_eSPI u8g2;
Audio audio;
WM8978 dac;
Button2 button;

// 创建Button2对象
Button2 buttonLeft(SW_LEFT);
Button2 buttonRight(SW_RIGHT);
Button2 buttonConf(SW_CONF);
Button2 buttonBack(SW_BACK);

int counter = 0;  // 初始计数值为0

int cursorX = 23;      // 初始光标X坐标
int cursorY = 124;     // 光标的Y坐标固定为124
int cursorWidth = 47;  // 光标宽度
int cursorHeight = 4;  // 光标高度

int targetX = 23;        // 目标光标X坐标
int speed = 4;           // 光标的移动速度
int step_increment = 1;  // 步长增量，用于加速光标移动

// 定义应用状态
enum AppState {
  MAIN_MENU,
  MUSIC_APP
};

AppState currentState = MAIN_MENU;

// 定义音乐应用的状态
enum MusicAppState {
  MUSIC_FILES,    // 音乐文件列表状态
  MUSIC_PLAYBACK  // 音乐播放状态（暂时留空）
};
MusicAppState currentMusicAppState = MUSIC_FILES;  // 初始化为 MUSIC_FILES 状态

TaskHandle_t audioTaskHandle = NULL;  // 用于存储音频任务的句柄


std::vector<String> listFilesInDirectory(const char* directory) {
  std::vector<String> fileNames;  // 用于存储文件名的列表

  File dir = SD.open(directory);  // 打开指定目录
  if (!dir) {
    Serial.println("Failed to open directory!");
    return fileNames;  // 如果目录打不开，返回空列表
  }

  // 遍历目录中的所有文件
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;  // 如果没有更多文件，退出循环
    }

    if (!entry.isDirectory()) {
      fileNames.push_back(entry.name());  // 如果是文件，加入文件名列表
    }
    entry.close();  // 关闭文件
  }

  dir.close();       // 关闭目录
  return fileNames;  // 返回文件名列表
}
// 更新计数器函数
void updateCounter() {
  buttonLeft.loop();
  buttonRight.loop();
  if (buttonLeft.isPressed()) {  // 左按钮按下，减少
    counter--;
    if (counter < 0) {
      counter = 2;  // 如果小于0，变为2
    }
    delay(200);  // 防止按钮抖动，增加延迟
  }

  if (buttonRight.isPressed()) {  // 右按钮按下，增加
    counter++;
    if (counter > 2) {
      counter = 0;  // 如果大于2，变为0
    }
    delay(200);  // 防止按钮抖动，增加延迟
  }
}
// 横向或纵向平滑移动的动画函数
// 参数：current_pos 当前坐标, target_pos 目标坐标, speed 移动速度
// 返回：动画是否完成（true: 完成，false: 未完成）
bool animateMovement(int& current_pos, int target_pos, int& speed, int step_increment) {
  if (current_pos == target_pos)
    return true;
  int step = (target_pos - current_pos) > 0 ? speed : -speed;
  current_pos += step;
  if ((step > 0 && current_pos > target_pos) || (step < 0 && current_pos < target_pos)) {
    current_pos = target_pos;
  }
  speed += step_increment;
  return false;  // 动画仍在进行中
}
void displayVerticalList(const std::vector<String>& list, int startX, int startY, int lineHeight, int spacing, int selectedIndex) {
  const int itemsPerPage = 5;                                                         // 每页最多显示的项目数
  int currentPage = selectedIndex / itemsPerPage;                                     // 当前页码
  int totalPages = (list.size() + itemsPerPage - 1) / itemsPerPage;                   // 总页数
  int startIndex = currentPage * itemsPerPage;                                        // 当前页的起始索引
  int endIndex = std::min(startIndex + itemsPerPage, static_cast<int>(list.size()));  // 当前页的结束索引
  static unsigned long lastScrollTime = 0;                                            // 滚动的时间控制
  static int scrollSpeed = 1;                                                         // 滚动的速度
  static int scrollStepIncrement = 1;                                                 // 滚动的步长增量
  static int scrollOffset = 0;                                                        // 滚动的偏移量
  static int lastSelectedIndex = -1;                                                  // 记录上一次选中的索引
  // 检查是否需要重置滚动状态
  if (selectedIndex != lastSelectedIndex) {
    scrollOffset = 0;                   // 重置滚动偏移量
    lastScrollTime = 0;                 // 重置滚动计时
    scrollSpeed = 1;                    // 重置滚动速度
    lastSelectedIndex = selectedIndex;  // 更新选中的索引
  }
  sprite.fillSprite(TFT_WHITE);  // 清空背景
  int yPosition = startY;        // 起始Y坐标
  // 遍历当前页的项目
  for (int i = startIndex; i < endIndex; i++) {
    String text = list[i];
    int textWidth = u8g2.getUTF8Width(text.c_str());  // 获取文字宽度
    // 如果是当前选中的项，绘制高亮背景
    if (i == selectedIndex) {
      sprite.fillRoundRect(startX - 5, yPosition - (lineHeight / 2), 220, lineHeight, 8, TFT_ORANGE);  // 长圆矩形背景
    } else {
      sprite.fillRoundRect(startX - 5, yPosition - (lineHeight / 2), 220, lineHeight, 8, TFT_LIGHTGREY);  // 普通背景
    }
    // 如果文字过长且当前项被选中，处理滚动显示
    if (textWidth > 200 && i == selectedIndex) {  // 假设显示区域最大宽度为 200
      // 每 300ms 滚动一次
      if (millis() - lastScrollTime > 400) {
        int targetScrollOffset = textWidth - 200;  // 目标滚动偏移量
        if (animateMovement(scrollOffset, targetScrollOffset, scrollSpeed, scrollStepIncrement)) {
          // 当滚动到达目标位置时，重置滚动参数
          scrollOffset = 0;
          scrollSpeed = 1;
        }
        lastScrollTime = millis();
      }
      // 计算文字的显示起始位置
      int textX = startX - scrollOffset;
      if (textX + textWidth > startX) {
        u8g2.setCursor(textX, yPosition + (lineHeight / 4));  // 设置文字位置
        u8g2.print(text);                                     // 打印文字
      }
    } else {
      // 如果文字宽度小于显示区域或当前项未被选中，正常显示
      u8g2.setCursor(startX, yPosition + (lineHeight / 4));  // 设置文字位置
      u8g2.print(text);                                      // 打印文字
    }
    yPosition += lineHeight + spacing;  // 更新Y坐标，增加间距
  }
  // 显示页码信息
  String pageInfo = "Page " + String(currentPage + 1) + " / " + String(totalPages);
  u8g2.setCursor(45, 135);  // 设置页码显示位置
  u8g2.print(pageInfo);

  sprite.pushSprite(0, 0);  // 更新显示
}
String formatTime(uint32_t seconds) {
  int minutes = seconds / 60;
  int remainingSeconds = seconds % 60;
  String formattedTime = String(minutes) + ":" + (remainingSeconds < 10 ? "0" : "") + String(remainingSeconds);
  return formattedTime;
}
// 音频处理任务
void audioTask(void* pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1);  // 每毫秒唤醒一次

  while (true) {
    audio.loop();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}
void setup() {
  Serial.begin(115200);
  pinMode(TF_CS, OUTPUT);
  digitalWrite(TF_CS, HIGH);
  SPI.begin(TF_CLK, TF_DATA0, TF_CMD);
  SPI.setFrequency(8000000);
  if (!SD.begin(TF_CS)) {
    Serial.println("SD Card initialization failed!");
  } else {
    Serial.println("SD Card initialized.");
  }
  pinMode(NS_CON, OUTPUT);
  pinMode(RT_A3V3_EN, OUTPUT);
  digitalWrite(NS_CON, HIGH);
  digitalWrite(RT_A3V3_EN, HIGH);
  dac.begin(I2C_SDA, I2C_SCL);
  audio.setPinout(I2S_BCLK, I2S_WS, I2S_DOUT, I2S_DIN, I2S_MCLKPIN);
  dac.setSPKvol(0);
  dac.setHPvol(55, 55);

  tft.init();
  tft.setRotation(3);  // 屏幕设置
  u8g2.begin(tft);
  u8g2.setDisplay(&sprite);
  xTaskCreatePinnedToCore(audioTask, "AudioTask", 4096, NULL, 1, NULL, 0);
}
void loop() {
  audio.loop();  // 启动音频播放循环
  switch (currentState) {
    case MAIN_MENU:
      main_menu();
      break;
    case MUSIC_APP:
      music_app();
      break;
  }
}
// 主菜单显示函数
void main_menu() {
  updateCounter();  // 更新计数器
  sprite.createSprite(240, 135);
  sprite.fillSprite(TFT_WHITE);  // 清空屏幕
  sprite.pushImage(0, 0, MIANBACKGROUND_WIDTH, MIANBACKGROUND_HEIGHT, mianbackground);
  // 更新目标光标位置
  if (counter == 0) {
    targetX = 23;  // 选项 0
  } else if (counter == 1) {
    targetX = 97;  // 选项 1
  } else if (counter == 2) {
    targetX = 171;  // 选项 2
  }
  // 使用 animateMovement 平滑移动光标
  if (animateMovement(cursorX, targetX, speed, step_increment)) {
    // 当光标到达目标位置时，动画完成
  }
  sprite.fillRect(cursorX, cursorY, cursorWidth, cursorHeight, TFT_ORANGE);  // 绘制橙色光标
  sprite.pushSprite(0, 0);                                                   // 更新显示
  // 检测确认按钮按下且counter ==1时，切换到music_app
  buttonConf.loop();
  if (counter == 1 && buttonConf.isPressed()) {
    currentState = MUSIC_APP;
    delay(200);  // 防抖
  }
}
void music_app() {
  static int selectedIndex = 0;        // 当前选中的项索引
  static bool hasListedFiles = false;  // 标记是否已经列出文件
  static std::vector<String> files;    // 文件列表
  static bool isPlaying = false;       // 播放状态
  static uint32_t lastUpdateTime = 0;  // 上次更新时间

  switch (currentMusicAppState) {
    case MUSIC_FILES:
      {
        // 检测返回按钮按下，返回主菜单
        buttonLeft.loop();
        buttonRight.loop();
        buttonConf.loop();
        buttonBack.loop();  // 检测返回按钮
        if (buttonBack.isPressed()) {
          currentState = MAIN_MENU;  // 切换到主菜单状态
          delay(200);                // 防抖延迟
          return;                    // 跳出 music_app()，直接返回主菜单
        }
        if (!hasListedFiles) {
          const char* folder = "/music";         // 指定目录
          files = listFilesInDirectory(folder);  // 获取文件列表

          Serial.println("Music Files:");
          for (size_t i = 0; i < files.size(); i++) {
            Serial.println(files[i]);  // 打印文件名到串口
          }
          hasListedFiles = true;  // 文件列表只获取一次
        }
        // 检测按钮输入，更新 selectedIndex
        if (buttonLeft.isPressed()) {
          selectedIndex--;
          if (selectedIndex < 0) {
            selectedIndex = files.size() - 1;  // 循环到最后一个
          }
          delay(200);  // 防抖延迟
        }
        if (buttonRight.isPressed()) {
          selectedIndex++;
          if (selectedIndex >= files.size()) {
            selectedIndex = 0;  // 循环到第一个
          }
          delay(200);  // 防抖延迟
        }
        // 检测确认按钮按下，切换到 MUSIC_PLAYBACK
        if (buttonConf.isPressed()) {
          currentMusicAppState = MUSIC_PLAYBACK;
          audio.connecttoFS(SD, ("/music/" + files[selectedIndex]).c_str());
          isPlaying = true;
          lastUpdateTime = millis();
          delay(200);

          // 创建音频处理任务
          if (audioTaskHandle == NULL) {
            xTaskCreatePinnedToCore(audioTask, "AudioTask", 4096, NULL, 1, &audioTaskHandle, 0);
          }
        }

        // 显示文件列表到屏幕
        sprite.createSprite(240, 135);
        sprite.fillSprite(TFT_WHITE);  // 清空屏幕
        u8g2.setFont(all_font);        // 设置自定义字体
        u8g2.setFontMode(1);           // 设置字体透明背景
        u8g2.setForegroundColor(0x0000);
        // 使用 displayVerticalList 显示文件列表
        int startX = 10;
        int startY = 20;
        int lineHeight = 20;
        int spacing = 2.5;  // 设置选项间距为 5 像素
        displayVerticalList(files, startX, startY, lineHeight, spacing, selectedIndex);

        sprite.pushSprite(0, 0);  // 更新显示
        break;
      }
    case MUSIC_PLAYBACK:
      {
        buttonLeft.loop();
        buttonRight.loop();
        buttonConf.loop();
        buttonBack.loop();  // 检测返回按钮
        if (buttonBack.isPressed()) {
          currentMusicAppState = MUSIC_FILES;
          audio.stopSong();  // 停止播放
          delay(200);

          // 删除音频处理任务
          if (audioTaskHandle != NULL) {
            vTaskDelete(audioTaskHandle);
            audioTaskHandle = NULL;
          }
        }
        // 检测确认按钮按下，切换播放/暂停状态
        if (buttonConf.isPressed()) {
          isPlaying = !isPlaying;  // 切换播放状态
          audio.pauseResume();     // 切换播放/暂停
          delay(200);  // 防抖延迟
        }
        // 获取歌曲文件名、总时间和当前播放时间
        String currentSong = files[selectedIndex];              // 获取当前播放的歌曲名
        uint32_t totalDuration = audio.getAudioFileDuration();  // 获取音频文件的总时长
        uint32_t currentTime = audio.getAudioCurrentTime();     // 获取当前播放时间

        // 格式化总时间和当前时间
        String totalTime = formatTime(totalDuration);
        String currentPlayingTime = formatTime(currentTime);

        // 显示播放界面
        sprite.createSprite(240, 135);
        sprite.fillSprite(TFT_WHITE);
        u8g2.setFont(all_font);
        u8g2.setFontMode(1);
        u8g2.setForegroundColor(0x0000);
        sprite.pushImage(0, 0, MUSICBACKGROUND_WIDTH, MUSICBACKGROUND_HEIGHT, musicbackground);  // 绘制音乐界面背景图
        // 显示 "播放中：" 或 "暂停中：" 文本
        if (isPlaying) {
          u8g2.setCursor(20, 50);
          u8g2.print("播放中：");
          sprite.pushImage(96, 87, CONTROL_PLAY_WIDTH, CONTROL_PLAY_HEIGHT, control_play);  // 绘制播放图标
        } else {
          u8g2.setCursor(20, 50);
          u8g2.print("暂停中：");
          sprite.pushImage(96, 87, CONTROL_STOP_WIDTH, CONTROL_STOP_HEIGHT, control_stop);  // 绘制暂停图标
        }
        // 显示歌曲名称
        u8g2.setCursor(87, 50);
        u8g2.print(currentSong);

        // 显示总时间
        u8g2.setCursor(185, 98);
        u8g2.print(totalTime);  // 显示总时间

        // 显示当前播放时间
        u8g2.setCursor(0, 98);
        u8g2.print(currentPlayingTime);  // 显示当前时间

        sprite.pushSprite(0, 0);  // 更新显示
        break;
      }
  }
}