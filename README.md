
# 深蹲次數辨識 ESP32-CAM 專案

本專案結合 ESP32-CAM 與 MediaPipe Pose，利用瀏覽器前端偵測深蹲姿勢並計算次數，並預留按鈕與 LCD 整合介面。

---

## 目錄

1. [功能概述](https://chatgpt.com/c/6837cdca-aff0-8004-a9aa-cf2c530daa5c?model=o4-mini-high#%E5%8A%9F%E8%83%BD%E6%A6%82%E8%BF%B0)
2. [專案結構](https://chatgpt.com/c/6837cdca-aff0-8004-a9aa-cf2c530daa5c?model=o4-mini-high#%E5%B0%88%E6%A1%88%E7%B5%90%E6%A7%8B)
3. [硬體需求](https://chatgpt.com/c/6837cdca-aff0-8004-a9aa-cf2c530daa5c?model=o4-mini-high#%E7%A1%AC%E9%AB%94%E9%9C%80%E6%B1%82)
4. [軟體需求](https://chatgpt.com/c/6837cdca-aff0-8004-a9aa-cf2c530daa5c?model=o4-mini-high#%E8%BB%9F%E9%AB%94%E9%9C%80%E6%B1%82)
5. [安裝與部署](https://chatgpt.com/c/6837cdca-aff0-8004-a9aa-cf2c530daa5c?model=o4-mini-high#%E5%AE%89%E8%A3%9D%E8%88%87%E9%83%A8%E7%BD%B2)
6. [使用說明](https://chatgpt.com/c/6837cdca-aff0-8004-a9aa-cf2c530daa5c?model=o4-mini-high#%E4%BD%BF%E7%94%A8%E8%AA%AA%E6%98%8E)
7. [將來擴充](https://chatgpt.com/c/6837cdca-aff0-8004-a9aa-cf2c530daa5c?model=o4-mini-high#%E5%B0%87%E4%BE%86%E6%93%B4%E5%85%85)
8. [參考資料](https://chatgpt.com/c/6837cdca-aff0-8004-a9aa-cf2c530daa5c?model=o4-mini-high#%E5%8F%83%E8%80%83%E8%B3%87%E6%96%99)

---

## 功能概述

* **深蹲次數辨識** ：根據 MediaPipe Pose 監測膝關節角度，統計正確與錯誤深蹲次數，並顯示總次數。
* **按鈕觸發** ：預留按鈕事件，按下按鈕後啟動偵測計數。
* **LCD 顯示** ：未來可將計數結果輸出至 LCD。

## 專案結構

```text
├─ include/
│   └─ SquatCounter.h    # HTML 字串及前端邏輯
├─ src/
│   └─ main.cpp          # ESP32-CAM 主程式
├─ README.md             # 專案說明檔
└─ platformio.ini / Arduino .ino # 專案設定
```

## 硬體需求

* ESP32-CAM 模組（含 OV2640 鏡頭）
* Micro USB 轉 TTL 轉接線（程式燒錄）
* ESP32-CAM 電源供應（5V）
* 外接按鈕 ×1
* LCD 顯示器（如 I2C 1602 或 TFT）
* 連接線材、麵包板等

## 軟體需求

* Arduino IDE 或 PlatformIO
* ESP32 Core for Arduino
* `AsyncTCP`、`ESPAsyncWebServer` 套件

前端：

* 透過瀏覽器載入 MediaPipe JS SDK（無需本機安裝）

## 安裝與部署

1. **加入標頭檔**
   * 將 `include/SquatCounter.h` 放置專案 `include/` 目錄。
2. **設定 WebServer**
   ```cpp
   #include <SquatCounter.h>
   #include <ESPAsyncWebServer.h>
   AsyncWebServer server(80);

   void setup() {
     // 初始化 WiFi、相機...
     server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
       req->send_P(200, "text/html", index_html);
     });
     server.begin();
   }

   void loop() {}
   ```
3. **上傳程式**
   * 使用 Arduino IDE 或 PlatformIO 燒錄至 ESP32-CAM。
4. **連接相機**
   * 透過瀏覽器 (手機或電腦) 打開 ESP32-CAM IP 位址，即可看到深蹲計數介面。

## 使用說明

1. 開啟瀏覽器，進入 `http://<ESP32-IP>/`。
2. 等待 MediaPipe 模型載入。
3. 按下專案中實作的按鈕（未來加入）以啟動計數偵測。
4. 深蹲動作時，畫面上會顯示：
   * **總次數**
   * **正確次數** ：膝蓋角度小於設定閾值視為合格。
   * **錯誤次數** ：未達到下蹲深度視為失敗。

## 將來擴充

* **按鈕整合** ：在 `main.cpp` 中偵測按鈕腳位，按下後透過 WebSocket 或 HTTP 觸發 `detectEnabled = true`。
* **LCD 顯示** ：使用 I2C 或 SPI 與 LCD 模組連接，將 `correctCount`、`incorrectCount`、總次數輸出至螢幕。
* **參數調校** ：可透過 HTTP 介面調整下蹲／站立閾值與穩定幀數。
* **行動裝置相容性** ：優化前端 UI，縮放 Canvas，提升手機瀏覽體驗。

## 參考資料

* [MediaPipe Pose JavaScript](https://google.github.io/mediapipe/solutions/pose.html)
* [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

---

> **作者** ：Your Name
>
> **日期** ：2025-05-31
>
