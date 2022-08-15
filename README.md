# CameraWebServerRecorder
Enhancement of @espressif CameraWebServer to add avi video recording to an SD Card 


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

![image](https://user-images.githubusercontent.com/36938190/184581874-a0a66c24-0a92-4854-9117-76b3b94cfffc.png)


Here are the new controls. 
From the top:
1.  How much freespace on your SD card
2.  Debug parameters show   
2.1  framesize    
2.2  quality  
2.3  interval (milliseconds per frame)  
2.4  speedup (muliply record framerate for playing)   
2.5  avi segment length in seconds  
3.  set avi segment length  
4.  set frames per second to record (shows as seconds per frame to slow recording)  
5.  set speedup playback over record rate - this will adjust to playback all videos at 30 fps when you change framerate, but you can readjust it)  
6.  Start and Stop recording  
