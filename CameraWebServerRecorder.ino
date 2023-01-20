/*
 * Orignial from espressif, part of arduino-esp32
 * 
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/Camera/CameraWebServer
 * 
 * forked, or copied about Aug 1, 2022
 * - didn't want to fork the entire arduino-esp32 for this example
 * 
*/

/*

  CameraWebServerRecorder
 
  https://github.com/jameszah/CameraWebServerRecorder
  
  
  This is a modified version of the espressif ESP32-CAM CameraWebServer example for Arduino, which lets you fiddle  
  with all the parameters on the ov2640 camera providing a web browser interface to make the changes, and view the stream,
  ot stills jpeg captures.
  
  The modification is to add a video recorder facilty.  You can set the frame rate to record, the "speedup" to you can play a 
  timelapse at 30 fps on your computer, and a avi segment length, so you end the movie from time to time, so you won't have a  2 GB 
  avi file to edit later.  You can break it into chunks, each with its own index.

  My original idea was add all the parameters to control the camera to https://github.com/jameszah/ESP32-CAM-Video-Recorder-junior,
  but I left this as primarily the espressif demo program, and added a simple sd recording facility.  No semaphores, and multitak
  communications, other than setting a global variable to start stop the recording task, using all the parameters you have already
  set in the espressif streaming/snapshot control panel.
  
  If you put a file called "secret.txt" on your SD, with 2 lines giving your SSID name, and SSID password, then it will
  join your router and you can access from your phone or computer.
  If there is no file, it will start in AP mode and you can connect your phone or computer the "ESP32-CAM", password "123456789",
  to access all the functionality.  It will create a secret.txt for you with the SSID name "AP ESP32-CAM".  You can edit that 
  later to a different AP name a password.
  This AP funtionality is useful it you are setting up a camera away from a router, and you can connect with your phone, choose
  your parameters, start the recording, and walk away.  When you return the SD will be full of great video!

  The avi recordings seem quite amenable to haveing parameters changed during the recording, including framesize!

  It turns off streaming when you start recording, but you can turn it back on, with some loss of sd recording speed.
  If you are timelapse recording like 1 fps, it doesn't really matter.
  (The junior program below, can record and stream 2 channels without reducing recording rate.) 

  The filenames of the recordings are just the date and time (2022-08-14_11.45.49.avi).  
  Date and time in AP mode is acheived by automatically sending unix time when you click the Start Record button since
  your phone/computer should know the time.  Timezones have not been implemented, so its all GMT.

  If you have no SD card installed, it will show 0 freespace on the SD, and continue with other functionality.
  When the SD runs out of diskspce, recording will stop - no deleting old stuff implemented.  (my other program have it)
  
  You can also control this CamWebServer with simple urls, if you want to configure and start it with motioneye (or 
  equivalent) as follows,

  http://192.168.1.67/control?var=brightness&val=2
  http://192.168.1.67/control?var=contrast&val=-2
  http://192.168.1.67/control?var=saturation&val=2
  http://192.168.1.67/control?var=special_effect&val=1
  http://192.168.1.67/control?var=awb&val=1
  http://192.168.1.67/control?var=awb_gain&val=1
  http://192.168.1.67/control?var=ae_level&val=-1
  http://192.168.1.67/control?var=agc&val=0
  http://192.168.1.67/control?var=gainceiling&val=1
  http://192.168.1.67/control?var=ae_level&val=-2
  http://192.168.1.67/control?var=agc&val=1
  http://192.168.1.67/control?var=bpc&val=0
  http://192.168.1.67/control?var=raw_gma&val=0
  http://192.168.1.67/control?var=lenc&val=0
  http://192.168.1.67/control?var=hmirror&val=1
  http://192.168.1.67/control?var=dcw&val=0
  http://192.168.1.67/control?var=vflip&val=1
  http://192.168.1.67/control?var=hmirror&val=0
  http://192.168.1.67/control?var=dcw&val=1
  http://192.168.1.67/control?var=colorbar&val=1
  http://192.168.1.67/status 
  http://192.168.1.67/capture 
  http://192.168.1.67:81/stream - streaming using different port 81, so the web still responds on port 80
  
  ... and CamWebServerRecorder adds these:

  http://192.168.1.67/control?var=interval&val=1000      -- 1000 milliseconds between frames
  http://192.168.1.67/control?var=seglen&val=1800        -- avi file is closed and new file started every 1800 seconds
  http://192.168.1.67/control?var=speedup&val=30         -- play 1 fps recording at 30fps when you play the avi
  http://192.168.1.67/startrecord
  http://192.168.1.67/stoprecord 
  
  
  The avi.cpp part of this code is based on heavily modified and simplfied subset of:
  https://github.com/jameszah/ESP32-CAM-Video-Recorder-junior
  which is a heavily modifed and simplfied rework of:
  https://github.com/jameszah/ESP32-CAM-Video-Recorder
  
  Those two programs have various ways to control the recording, and view live streams, and download the video to you computer 
  or phone, or upload snapshots and video to Telegram, etc.

  The changes to the original files:
  CamWebServerRecorder.ino - mount the SD card, the AP stuff, start the recorder task
  app_httpd.cpp - the esp32 side code to handle the new paramters
  camera_index.h - this has the gziped html and javascript from espressif expanded (and easily editable),
                   and inclused all the changes to the web display for new parameters and controls
  avi.cpp - all the origninal simplfied code to record a mjpeg avi
  camer_pins.h - no changes
  
  Work-In-Progress .... but other things to do.
  - only works for ov2640 camera
  - webpage does report SD freespace, but no info on recordings, including whether it is recording (if you reload the page)
  - One-Click-Installer not done -- maybe tomorrow
  - didn't put in mdns - so you have to figure out your ip from serial monitor, or port authority
    (it should respond on 80 and 81, which would give you a clue, but has a defualt name like esp32-F41450
    
  James Zahary
  Aug 14, 2022
  Version 55.10 - in my mysterious numbering scheme
  Free coffee https://ko-fi.com/jameszah/
  jamzah.plc@gmail.com
  
  jameszah/CameraWebServerRecorder is licensed under the
   GNU General Public License v3.0

Arduino 1.8.19
Arduino-ESP32 2.0.4
Board: AI-Thinker ESP32-CAM
Huge APP

Using library WiFi at version 2.0.0 in folder: C:\ArduinoPortable\arduino-1.8.19\portable\packages\esp32\hardware\esp32\2.0.4\libraries\WiFi 
Using library SD_MMC at version 2.0.0 in folder: C:\ArduinoPortable\arduino-1.8.19\portable\packages\esp32\hardware\esp32\2.0.4\libraries\SD_MMC 
Using library FS at version 2.0.0 in folder: C:\ArduinoPortable\arduino-1.8.19\portable\packages\esp32\hardware\esp32\2.0.4\libraries\FS 
"C:\\ArduinoPortable\\arduino-1.8.19\\portable\\packages\\esp32\\tools\\xtensa-esp32-elf-gcc\\gcc8_4_0-esp-2021r2-patch3/bin/xtensa-esp32-elf-size" -A "C:\\Users\\James\\AppData\\Local\\Temp\\arduino_build_818851/CameraWebServerRecorder.55.10.ino.elf"
Sketch uses 1589429 bytes (50%) of program storage space. Maximum is 3145728 bytes.
Global variables use 68992 bytes (21%) of dynamic memory, leaving 258688 bytes for local variables. Maximum is 327680 bytes.


*/
#include "esp_camera.h"
#include <WiFi.h>

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
// ** Espressif Internal Boards **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD

#include "camera_pins.h"

extern TaskHandle_t the_camera_loop_task;
extern void the_camera_loop (void* pvParameter);
esp_err_t init_sdcard();

void startCameraServer();

const char* def_ssid = "ESP32-CAM";
const char* def_pass = "123456789";

#include <SD_MMC.h>
void start_wifi() {

  String junk;

  String cssid;
  String cssid2;
  String cpass;
  char ssidch[32];
  char ssidch2[32];
  char passch[64];

  File config_file = SD_MMC.open("/secret.txt", "r");
  if (config_file) {

    Serial.println("Reading secret.txt");
    cssid = config_file.readStringUntil(' ');
    cssid2 = config_file.readStringUntil(' ');
    junk = config_file.readStringUntil('\n');
    cpass = config_file.readStringUntil(' ');
    junk = config_file.readStringUntil('\n');
    config_file.close();

    cssid.toCharArray(ssidch, cssid.length() + 1);
    cssid2.toCharArray(ssidch2, cssid2.length() + 1);
    cpass.toCharArray(passch, cpass.length() + 1);
    
    if (String(cssid) == "ap" || String(cssid) == "AP") {
      WiFi.softAP(ssidch2, passch);

      IPAddress IP = WiFi.softAPIP();
      Serial.println("Using AP mode: ");
      Serial.print(ssidch2); Serial.print(" / "); Serial.println(passch);
      Serial.print("AP IP address: ");
      Serial.println(IP);
    } else {
      Serial.println(ssidch);
      Serial.println(passch);
      WiFi.begin(ssidch, passch);
      WiFi.setSleep(false);

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("");
      Serial.println("Using Station mode: ");
      Serial.print(cssid); Serial.print(" / "); Serial.println(cpass);
      Serial.println("WiFi connected");

      Serial.print("Camera Ready! Use 'http://");
      Serial.print(WiFi.localIP());
      Serial.println("' to connect");
    }
  } else {
    Serial.println("Failed to open config.txt - writing a default");

    // lets make a simple.txt config file
    File new_simple = SD_MMC.open("/secret.txt", "w");
    new_simple.println("ap ESP32-CAM // your ssid - ap mean access point mode, put Router123 station mode");
    new_simple.println("123456789    // your ssid password");
    new_simple.close();

    WiFi.softAP(def_ssid, def_pass);

    IPAddress IP = WiFi.softAPIP();
    Serial.println("Using AP mode: ");
    Serial.print(def_ssid); Serial.print(" / "); Serial.println(def_pass);
    Serial.print("AP IP address: ");
    Serial.println(IP);

  }
}


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println("-----------------------------------------------------------");
  Serial.println("https://github.com/jameszah/CameraWebServerRecorder 55.10\n");
  Serial.println("-----------------------------------------------------------");
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 3;      //jz 2->3 - add another frame for the avi recording
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif


  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

  esp_err_t  card_err = init_sdcard();
  if (card_err != ESP_OK) {
    Serial.printf("SD Card init failed with error 0x%x", card_err);
  }

  start_wifi();

  startCameraServer();

  // 5000 stack, prio 5 same at http streamer, core 1
  xTaskCreatePinnedToCore( the_camera_loop, "the_camera_loop", 5000, NULL, 5, &the_camera_loop_task, 1);

  delay(100);
}

void loop() {

  delay(1000);
}
