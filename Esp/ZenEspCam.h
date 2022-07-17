#ifndef ZenEspCam
#define ZenEspCam
// stuff for the esp32cam board

#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include <EEPROM.h>            // read and write from flash memory
#define EEPROM_SIZE 1          // just useing 1 byte

#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void EspBootReason()
{
  esp_reset_reason_t reset_reason = esp_reset_reason();

  switch (reset_reason)
  {
    case ESP_RST_UNKNOWN:    Dbg("Reset reason can not be determined","",1); break;
    case ESP_RST_POWERON:    Dbg("Reset due to power-on event","",1); break;
    case ESP_RST_EXT:        Dbg("Reset by external pin (not applicable for ESP32)","",1); break;
    case ESP_RST_SW:         Dbg("Software reset via esp_restart","",1); break;
    case ESP_RST_PANIC:      Dbg("Software reset due to exception/panic","",1); break;
    case ESP_RST_INT_WDT:    Dbg("Reset (software or hardware) due to interrupt watchdog","",1); break;
    case ESP_RST_TASK_WDT:   Dbg("Reset due to task watchdog","",1); break;
    case ESP_RST_WDT:        Dbg("Reset due to other watchdogs","",1); break;
    case ESP_RST_DEEPSLEEP:  Dbg("Reset after exiting deep sleep mode","",1); break;
    case ESP_RST_BROWNOUT:   Dbg("Brownout reset (software or hardware)","",1); break;
    case ESP_RST_SDIO:       Dbg("Reset over SDIO","",1); break;
  }

  if (reset_reason == ESP_RST_DEEPSLEEP)
  {
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    switch(wakeup_reason)
    { 
      case ESP_SLEEP_WAKEUP_UNDEFINED:    Dbg("In case of deep sleep: reset was not caused by exit from deep sleep","",1); break;
      case ESP_SLEEP_WAKEUP_ALL:          Dbg("Not a wakeup cause: used to disable all wakeup sources with esp_sleep_disable_wakeup_source","",1); break;
      case ESP_SLEEP_WAKEUP_EXT0:         Dbg("Wakeup caused by external signal using RTC_IO","",1); break;
      case ESP_SLEEP_WAKEUP_EXT1:         Dbg("Wakeup caused by external signal using RTC_CNTL","",1); break;
      case ESP_SLEEP_WAKEUP_TIMER:        Dbg("Wakeup caused by timer","",1); break;
      case ESP_SLEEP_WAKEUP_TOUCHPAD:     Dbg("Wakeup caused by touchpad","",1); break;
      case ESP_SLEEP_WAKEUP_ULP:          Dbg("Wakeup caused by ULP program","",1); break;
      case ESP_SLEEP_WAKEUP_GPIO:         Dbg("Wakeup caused by GPIO (light sleep only)","",1); break;
      case ESP_SLEEP_WAKEUP_UART:         Dbg("Wakeup caused by UART (light sleep only)","",1); break;
    }
  }
}

void CameraInit()
{
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  // OV2640 camera module
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
  config.pixel_format = PIXFORMAT_JPEG;

//  pinMode(4, OUTPUT);
//  digitalWrite(4, LOW);
  
  if (psramFound()) 
  {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } 
  else 
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) 
  {
    FatalError("Camera init failed");
   // ESP.restart();
  }
  EEPROM.begin(EEPROM_SIZE);
}

bool SendPhoto(char * cfname)
{
  Dbg("SendPhoto Sending",cfname,1);
  SrvSendFile(ToString(cfname),"image/jpg");
  return true;
}

void CamHandleForm()
{
  String fname = WebSrv.arg("fname");
  fname.trim();
  Dbg("CamHandleForm Finding",fname,1);
  char cfname[13];
  ToCharArray(fname,cfname);
  SendPhoto(cfname);
}

void ListPhoto()  //ok
{
  SrvSendJsonDirList("/",".jpg","jpgs");
}

bool CheckPhoto(char * fname) // ok
{
  return (FileSize(fname) > 100 );
}

// Capture Photo and Save
bool CapturePhoto(char * fname) 
{
    camera_fb_t * fb = NULL; // pointer
 
    // Take a photo with the camera
    Dbg("Taking a photo...",fname,1);
 //   digitalWrite(4, HIGH);
    fb = esp_camera_fb_get();
 //   digitalWrite(4, LOW);

    if (!fb) 
    {
      Dbg("Camera capture","failed",1);
      return false;
    }
    File file = FileOpenWrite(fname);
    if (!file) 
    {
      Dbg("Failed to open file","in writing mode",1);
      FileClose(file);
      return false;
    }
    else 
    {
      FileWrite(file,fb->buf, fb->len); // payload (image), payload length
      FileClose(file);
      Dbg("The picture has been saved in",fname,0);
      Dbg(" - Size in Bytes",FileSize(fname),1);
    }

    esp_camera_fb_return(fb);

    // check if file has been correctly saved
    return CheckPhoto(fname);
}

void GetPhoto() 
{
  unsigned int PicNum;
  char fname[13];
  PicNum = EEPROM.read(0) + 1;
  memset(fname,char(0),12);
  snprintf(fname,13,"%02i%02i%02i%02i.jpg",year() - 2000,month(),day(),PicNum);
//  string fname = unique timedate stamp.; 
  if (CapturePhoto(fname))
  { 
    EEPROM.write(0, PicNum);
    EEPROM.commit();;
    SendPhoto(fname);  //??????????
  }
  else returnFail("Could not take picture");
}

#endif
