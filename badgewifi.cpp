/*
 * Copyright (c) 2020 hackwinkel
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#include <Arduino.h>
#include "badgewifi.h"
// uses SPIFFS so that is included in .h and mounted in setup()

// serial port has already been opened, but we also need access to tft
#include "badgetft.h"
extern Adafruit_ST7789 tftdisplay;

// set if concurrent upload is allowed - configurable item stored in SPIFFS file
int allowconcurrentupload=1;

// a large block of memory to hold the image
byte* uploadbuffer;
/*  
 * wifimode 0: try to join "badge" network, with ssid and password stored in flash OR if unavailable with default credentioals
 * wifimode 1: create a soft AP with 
 *    ssid "BADGExxxx" where xxxx is the last 4 digits of the MAC and
 *    password are all 12 hex digits of the MAC (without semicolons)
 *    
 * returns 1 if sccessful, 0 otherwise
 */
int setupwifi(int wifimode)
{
  int timeout=60;
  char ssid[41];
  char password[41];
  byte mac[6];
  IPAddress localip;

  Serial.println(WiFi.macAddress());
  WiFi.macAddress(mac);
  
  if (wifimode)
  {
    sprintf(ssid,"badge-%02x%02x",mac[4],mac[5]);
    sprintf(password,"%02x%02x%02x%02x%02x%02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    WiFi.softAP(ssid, password);
    localip = WiFi.softAPIP();
    tftdisplay.setCursor(3, 0);
    tftdisplay.setTextColor(ST77XX_WHITE);
    tftdisplay.setTextWrap(true);
    tftdisplay.setTextSize(2);
    tftdisplay.println("   CONFIGURATION");
    tftdisplay.println("--------------------");
    tftdisplay.println("");
    tftdisplay.println("connect with SSID:");
    tftdisplay.setTextSize(3);
    tftdisplay.println(ssid);
    tftdisplay.setTextSize(2);
    tftdisplay.println("");
    tftdisplay.println("using password:");
    tftdisplay.setTextSize(3);
    tftdisplay.println(password);
    tftdisplay.setTextSize(2);
    tftdisplay.println("");
    tftdisplay.println("--------------------");
    tftdisplay.println("");
    tftdisplay.print("http://");
    tftdisplay.println(localip);
    return(1);
  }
  else
  {
    // if a file is is available, use that..
    int filefailed=1;
    File f = SPIFFS.open("/config.txt", "r");
    if (f)
    {
      char cbuffer[65];
      if (f.available())
      {
        int l = f.readBytesUntil('\n', cbuffer, 64);
        cbuffer[l] = 0;
        strcpy(ssid,cbuffer);
        if (f.available())
        {
          l = f.readBytesUntil('\n', cbuffer, 64);
          cbuffer[l] = 0;
          strcpy(password,cbuffer);
          if (f.available())
          {
            l = f.readBytesUntil('\n', cbuffer, 64);
            if (cbuffer[0]=='0') allowconcurrentupload=0;
            else allowconcurrentupload=1;
            filefailed=0;
          }
        }
      }
    }
    if (filefailed)
    {
        Serial.println("no or incomplete configuration file");
        strcpy(ssid,"badge");
        strcpy(password,"hackwinkel");
        allowconcurrentupload=1;
    }
    Serial.println(ssid);
    Serial.println(password);

    WiFi.begin(ssid, password);
    while ((WiFi.status() != WL_CONNECTED)&&(timeout>0))
    {
      delay(1000);
      Serial.print(timeout);
      Serial.print(" ");
      timeout--;
    }
    if (timeout<=0)
    {
      Serial.println("Timeout. Continuing without wifi.");
      tftdisplay.setCursor(0, 10);
      tftdisplay.setTextColor(ST77XX_WHITE);
      tftdisplay.setTextWrap(true);
      tftdisplay.setTextSize(2);
      tftdisplay.println("   UPLOAD IMAGES");
      tftdisplay.println("--------------------");
      tftdisplay.println("");
      tftdisplay.println(" CONNECTION FAILED");
      tftdisplay.println("");
      tftdisplay.println("retry or reconfigure");
      return(0);
    }
    else
    {
      localip=WiFi.localIP();
      Serial.println("Connnected.");
      Serial.print("IP address: ");
      Serial.println(localip);
      tftdisplay.setCursor(3, 0);
      tftdisplay.setTextColor(ST77XX_WHITE);
      tftdisplay.setTextWrap(true);
      tftdisplay.setTextSize(2);
      tftdisplay.println("   UPLOAD IMAGES");
      tftdisplay.println("--------------------");
      tftdisplay.println("");
      tftdisplay.println("visit:");
      tftdisplay.println("");
      tftdisplay.print("http://");
      tftdisplay.println(localip);
      return(1);
    }
  }
}

AsyncWebServer webserver(80);

// dynamic webpage with template
void uploadpages(AsyncWebServerRequest *request)
{
  String filenumber;
  // local lambda function c++ > 11
  auto processor = [&filenumber](const String& var) 
  {
    if (var == "FILENUMBER") return filenumber;
    return String();
  };
  Serial.println("--uploadpages--");  
  int params = request->params();
  int fail=1;
  if (params==1)
  {
    if(request->hasParam("p"))
    {
      AsyncWebParameter* p = request->getParam("p");
      if ((p->value().length()==2)&&(p->value() >= "01")&&(p->value() <= "16"))
      {
        filenumber= p->value();
        request->send(SPIFFS, "/uploadpage.htm", String(), false, processor);
        fail=0;
      }
    }
  }
  if (fail) request->send(SPIFFS, "/failpagepage.htm");
}

void doupload(AsyncWebServerRequest *request)
{
  Serial.println("--doupload--");
  request->send(SPIFFS, "/upload.htm");
}


void uploadhandler(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  String filenumber;
  // local lambda function c++ > 11
  auto processor = [&filenumber](const String& var) 
  {
    if (var == "FILENUMBER") return filenumber;
    return String();
  };
  int buflen=0;
  int nextpos=0;
  int qtables=0;
  int imwidth=0;
  int imheight=0;
  static int fail=1;

  if (uploadbuffer==NULL) { Serial.println("malloc failed"); return; }
  if(!index)
  {
    if ((index+len)<=1000000) memcpy(uploadbuffer+index,data,len); // first call appears to be duplicated
    fail=1;
  }
  if(len)
  {
    if ((index+len)<=1000000) memcpy(uploadbuffer+index,data,len);
  }
  if(final)
  {
    if ((index+len)<=100000)
    {
      memcpy(uploadbuffer+index,data,len); // last transfer appears to be duplicated
      buflen=index+len;
      fail=0;
    }
    if(fail) request->send(SPIFFS,"/failpage.htm"); // I have the feeling this is incorrect if there are >1 file uploads
    else // check the content - is it a modestly sized baseline jpeg?
    {
      if (uploadbuffer[0]!=0xff) fail|=1;
      if (uploadbuffer[1]!=0xd8) fail|=1;
      if (uploadbuffer[2]!=0xff) fail|=2;
      if (uploadbuffer[3]!=0xe0) fail|=2;
      nextpos=4+uploadbuffer[5]+256*uploadbuffer[4];
      if (uploadbuffer[6]!=0x4a) fail|=4;
      if (uploadbuffer[7]!=0x46) fail|=4;
      if (uploadbuffer[8]!=0x49) fail|=4;
      if (uploadbuffer[9]!=0x46) fail|=4;
      if (uploadbuffer[10]!=0x00) fail|=4;
      // now skip any APPn markers;
      while ((uploadbuffer[nextpos]==0xff) && ((uploadbuffer[nextpos+1]&0xf0)==0xe0))
      {
        nextpos=nextpos+2+uploadbuffer[nextpos+3]+256*uploadbuffer[nextpos+2];
      }
      // now skip any quantization tables;
      while ((uploadbuffer[nextpos]==0xff) && (uploadbuffer[nextpos+1]==0xdb))
      {
        qtables++;
        nextpos=nextpos+2+uploadbuffer[nextpos+3]+256*uploadbuffer[nextpos+2];
      }
      if (qtables==0) fail|=8;
      if (uploadbuffer[nextpos]!=0xff) fail|=16;
      if (uploadbuffer[nextpos+1]!=0xc0) fail|=16;
      if (uploadbuffer[nextpos+4]!=0x08) fail|=16;
      imheight=uploadbuffer[nextpos+6]+256*uploadbuffer[nextpos+5];
      imwidth=uploadbuffer[nextpos+8]+256*uploadbuffer[nextpos+7];
      if ((imwidth>1024)||(imheight>1024)) fail|=32;
      Serial.print(imwidth);
      Serial.print(",");
      Serial.println(imheight);
      Serial.println(fail);
    }
    if (fail) { request->send(SPIFFS, "/failpage.htm"); }
    else
    {
      fail=1;
      if(request->hasParam("p",true))
      {
        AsyncWebParameter* p = request->getParam("p",true);
        if ((p->value().length()==2)&&(p->value() >= "01")&&(p->value() <= "16"))
        {
          filenumber= p->value();
          String filename="/"+p->value()+".jpg";
          File df = SPIFFS.open(filename, FILE_WRITE);
          df.write( uploadbuffer, buflen );
          df.close();
          Serial.println("saved file");

          request->send(SPIFFS, "/uploadpage.htm", String(), false, processor);
          fail=0;
        }
      }
      if (fail) request->send(SPIFFS, "/failpage.htm");
    }
  }
}

void setupwebserver(int servermode)
{
  uploadbuffer = (byte*)ps_malloc(1000002);// 1M file size limit

  if (uploadbuffer==NULL) { Serial.println("malloc failed"); }
  if (servermode)
  { 
    webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/setup.html"); });
  }
  else
  {
    webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/upload.html"); });
    webserver.on("/01.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/01.jpg"); });
    webserver.on("/02.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/02.jpg"); });
    webserver.on("/03.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/03.jpg"); });
    webserver.on("/04.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/04.jpg"); });
    webserver.on("/05.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/05.jpg"); });
    webserver.on("/06.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/06.jpg"); });
    webserver.on("/07.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/07.jpg"); });
    webserver.on("/08.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/08.jpg"); });
    webserver.on("/09.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/09.jpg"); });
    webserver.on("/10.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/10.jpg"); });
    webserver.on("/11.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/11.jpg"); });
    webserver.on("/12.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/12.jpg"); });
    webserver.on("/13.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/13.jpg"); });
    webserver.on("/14.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/14.jpg"); });
    webserver.on("/15.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/15.jpg"); });
    webserver.on("/16.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/16.jpg"); });
    webserver.on("/baseline", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/baseline.htm"); });
    webserver.on("/export_option.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/export_option.jpg"); });
    webserver.on("/upload", HTTP_GET, uploadpages);
    webserver.on("/doupload", HTTP_POST, doupload, uploadhandler);  // is doupload ever called?
  }
  webserver.on("/ISO_7010_W008.svg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/ISO_7010_W008.svg"); });
  webserver.on("/ISO_7010_W016.svg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/ISO_7010_W016.svg"); });
  webserver.on("/ISO_7010_W019.svg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/ISO_7010_W019.svg"); });
  webserver.on("/ISO_7010_W035.svg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/ISO_7010_W035.svg"); });

  webserver.begin();
}
