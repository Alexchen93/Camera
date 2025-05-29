#ifndef CAMERA_INDEX_H
#define CAMERA_INDEX_H

#include <pgmspace.h>    // PROGMEM 定義所在

// 把 HTML 存到 flash
static const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-Hant">
<head>
  <meta charset="UTF-8">
  <title>ESP32Cam Mediapipe</title>
  <style>canvas, video{max-width:100%;display:block;margin:0 auto;}</style>
</head>
<body>
  <video id="input_video" autoplay muted playsinline></video>
  <canvas id="output_canvas"></canvas>

  <script type="module">
    import { Holistic } from 'https://cdn.jsdelivr.net/npm/@mediapipe/holistic@0.5/holistic.js';
    import { drawConnectors, drawLandmarks } from 'https://cdn.jsdelivr.net/npm/@mediapipe/drawing_utils@0.4/drawing_utils.js';

    const videoElement = document.getElementById('input_video');
    videoElement.src = `http://${window.location.hostname}/stream`;
    videoElement.crossOrigin = 'anonymous';

    const canvasElement = document.getElementById('output_canvas');
    const ctx = canvasElement.getContext('2d');

    const holistic = new Holistic({
      locateFile: (file) =>
        `https://cdn.jsdelivr.net/npm/@mediapipe/holistic@0.5/${file}`
    });
    holistic.setOptions({
      modelComplexity: 1,
      smoothLandmarks: true,
      minDetectionConfidence: 0.5,
      minTrackingConfidence: 0.5
    });
    holistic.onResults(onResults);

    videoElement.addEventListener('loadedmetadata', () => {
      canvasElement.width = videoElement.videoWidth;
      canvasElement.height = videoElement.videoHeight;
      requestAnimationFrame(processFrame);
    });

    function processFrame() {
      holistic.send({ image: videoElement }).then(() => {
        requestAnimationFrame(processFrame);
      });
    }

    function onResults(results) {
      ctx.clearRect(0, 0, canvasElement.width, canvasElement.height);
      ctx.drawImage(results.image, 0, 0);
      if (results.poseLandmarks) {
        drawConnectors(ctx, results.poseLandmarks, Holistic.POSE_CONNECTIONS, {
          lineWidth: 4
        });
        drawLandmarks(ctx, results.poseLandmarks, { lineWidth: 2 });
      }
    }
  </script>
</body>
</html>
)rawliteral";

#endif // CAMERA_INDEX_H
