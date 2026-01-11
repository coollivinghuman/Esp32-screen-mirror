import asyncio
import websockets
import mss
import cv2
import numpy as np
import time

print("Configure the capture region.")
print("TIP: To mirror the ENTIRE computer screen, set:")
print("    top: 0, left: 0, width: your computer screen width, height: your computer screen height")
print("This refers to your computer, not the ESP32.")

ESP32_IP = input("Enter the IP shown on your ESP32: ")
ESP32_PORT = 81

top = int(input("top: "))
left = int(input("left: "))
width = int(input("width: "))
height = int(input("height: "))

CAPTURE_REGION = {
    "top": top,
    "left": left,
    "width": width,
    "height": height
}

W = 320
H = 240
CHUNK_SIZE = 1024

def frame_to_rgb565(frame):
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    r = (frame[:, :, 0] >> 3).astype(np.uint16)
    g = (frame[:, :, 1] >> 2).astype(np.uint16)
    b = (frame[:, :, 2] >> 3).astype(np.uint16)
    rgb565 = (r << 11) | (g << 5) | b
    return rgb565.astype('<u2').tobytes()

async def main():
    uri = f"ws://{ESP32_IP}:{ESP32_PORT}"
    last = time.time()
    frames = 0

    with mss.mss() as sct:
        async with websockets.connect(uri, ping_interval=None, max_size=None) as ws:
            while True:
                img = np.array(sct.grab(CAPTURE_REGION))[:, :, :3]  
                frame = cv2.resize(img, (W, H), interpolation=cv2.INTER_AREA)
                data = frame_to_rgb565(frame)

                for i in range(0, len(data), CHUNK_SIZE):
                    await ws.send(data[i:i+CHUNK_SIZE])

                frames += 1
                now = time.time()
                if now - last >= 1:
                    print("FPS:", frames)
                    frames = 0
                    last = now

                await asyncio.sleep(0)

asyncio.run(main())
