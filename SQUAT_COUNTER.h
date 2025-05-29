#ifndef SQUAT_COUNTER_H
#define SQUAT_COUNTER_H

#include <pgmspace.h>

// 嵌入 HTML 內容，透過 ESP32-CAM WebServer 提供
static const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-Hant">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>深蹲次數辨識</title>
  <style>
    body { margin: 0; display: flex; flex-direction: column; align-items: center; background: #fafafa; font-family: sans-serif; }
    video { display: none; }
    canvas { transform: scaleX(-1); border: 1px solid #ccc; }
    #totalCount { margin: 20px 0 5px; font-size: 2.5rem; color: #333; }
    #detailCount { margin-bottom: 20px; font-size: 1.5rem; color: #555; }
  </style>
  <script src="https://cdn.jsdelivr.net/npm/@mediapipe/pose@0.5/pose.min.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/@mediapipe/camera_utils@0.3/camera_utils.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/@mediapipe/drawing_utils@0.3/drawing_utils.js"></script>
</head>
<body>
  <div id="totalCount">總次數：0</div>
  <div id="detailCount">正確：0，錯誤：0</div>
  <video id="video" width="640" height="480" autoplay muted playsinline></video>
  <canvas id="canvas" width="640" height="480"></canvas>
  <script>
    const video      = document.getElementById('video');
    const canvas     = document.getElementById('canvas');
    const ctx        = canvas.getContext('2d');
    const totalDiv   = document.getElementById('totalCount');
    const detailDiv  = document.getElementById('detailCount');

    // 按鈕未實作，先預設自動啟動偵測
    let detectEnabled = false;

    // 閾值
    const DOWN_THRESHOLD = 90;
    const UP_THRESHOLD   = 160;
    const STABLE_FRAMES  = 5;

    let state          = 'up';
    let minAngle       = 180;
    let correctCount   = 0;
    let incorrectCount = 0;
    let downFrames     = 0;
    let upFrames       = 0;

    function calcAngle(a, b, c) {
      const v1 = {x: a.x - b.x, y: a.y - b.y};
      const v2 = {x: c.x - b.x, y: c.y - b.y};
      const dot = v1.x * v2.x + v1.y * v2.y;
      const m1  = Math.hypot(v1.x, v1.y);
      const m2  = Math.hypot(v2.x, v2.y);
      const cos = dot / (m1 * m2);
      return Math.acos(Math.max(-1, Math.min(1, cos))) * 180 / Math.PI;
    }

    function updateCount() {
      const total = correctCount + incorrectCount;
      totalDiv.textContent  = `總次數：${total}`;
      detailDiv.textContent = `正確：${correctCount}，錯誤：${incorrectCount}`;
    }

    function onResults(results) {
      if (!detectEnabled) return;
      ctx.save();
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      ctx.drawImage(results.image, 0, 0, canvas.width, canvas.height);
      if (results.poseLandmarks) {
        window.drawConnectors(ctx, results.poseLandmarks, window.POSE_CONNECTIONS, {lineWidth: 4, color: '#00FF00'});
        window.drawLandmarks(ctx, results.poseLandmarks, {radius: 3, color: '#FF0000'});
        const lm     = results.poseLandmarks;
        const angleL = calcAngle(lm[23], lm[25], lm[27]);
        const angleR = calcAngle(lm[24], lm[26], lm[28]);
        const angle  = (angleL + angleR) / 2;
        minAngle     = Math.min(minAngle, angle);
        if (angle < DOWN_THRESHOLD) { downFrames++; upFrames = 0; }
        else if (angle > UP_THRESHOLD) { upFrames++; downFrames = 0; }
        else { downFrames = 0; upFrames = 0; }
        if (state === 'up' && downFrames >= STABLE_FRAMES) { state = 'down'; minAngle = 180; }
        if (state === 'down' && upFrames >= STABLE_FRAMES) {
          state = 'up';
          if (minAngle <= DOWN_THRESHOLD) correctCount++;
          else incorrectCount++;
          updateCount();
        }
      }
      ctx.restore();
    }

    async function initCamera() {
      const stream = await navigator.mediaDevices.getUserMedia({video: true});
      video.srcObject = stream;
      await video.play();
      const pose = new window.Pose({ locateFile: f => `https://cdn.jsdelivr.net/npm/@mediapipe/pose@0.5/${f}` });
      pose.setOptions({ modelComplexity: 1, smoothLandmarks: true, minDetectionConfidence: 0.5, minTrackingConfidence: 0.5 });
      pose.onResults(onResults);
      const camera = new window.Camera(video, { onFrame: async () => await pose.send({image: video}), width: 640, height: 480 });
      camera.start();
    }

    window.addEventListener('load', () => {
      initCamera();
      // TODO: 加入按鈕事件，按下後 detectEnabled = true，就會開始計數
    });
  </script>
</body>
</html>
)rawliteral";

#endif // SQUAT_COUNTER_H
