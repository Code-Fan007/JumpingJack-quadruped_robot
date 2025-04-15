#include <M5Atom.h>
#include <Kalman.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WebServer.h>
 
 
WebServer server(80);
const char ssid[] = "servoOffset";  // SSID
const char pass[] = "password";   // password
const IPAddress ip(192, 168, 55, 28);      //　IP address
const IPAddress subnet(255, 255, 255, 0);
 
#define led_pin 27
#define NUM_LEDS 25
 
 
Preferences preferences;
 
//           ID  0   1   2  3   4   5   6   7
int offset[] = {0, 0, 0, 0, 0, 0, 0, 0}; 
 
 
 
//browser
void handleRoot() {
  String temp ="<!DOCTYPE html> \n<html lang=\"ja\">";
  temp +="<head>";
  temp +="<meta charset=\"utf-8\">";
  temp +="<title>servoOffset</title>";
  temp +="<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  temp +="<style>";
  temp +=".container{";
  temp +="  margin: auto;";
  temp +="  text-align: center;";
  temp +="  font-size: 1.2rem;";
  temp +="}";
  temp +="span,.pm{";
  temp +="  display: inline-block;";
  temp +="  border: 1px solid #ccc;";
  temp +="  width: 50px;";
  temp +="  height: 30px;";
  temp +="  vertical-align: middle;";
  temp +="  margin-bottom: 8px;";
  temp +="}";
  temp +="span{";
  temp +="  width: 120px;";
  temp +="}";
  temp +="button{";
  temp +="  width: 100px;";
  temp +="  height: 40px;";
  temp +="  font-weight: bold;";
  temp +="  margin-bottom: 8px;";
  temp +="}";
  temp +="button.on{ background:lime; color:white; }";
  temp +=".column-3{ max-width:330px; margin:auto; text-align:center; display:flex; justify-content:space-between; flex-wrap:wrap; }";
  temp +="</style>";
  temp +="</head>";
  
  temp +="<body>";
  temp +="<div class=\"container\">";
  temp +="<h3>servoOffset</h3>";
 
 
  //ID0 offset
  temp +="ID0<br>";
  temp +="<a class=\"pm\" href=\"/id0M\">-</a>";
  temp +="<span>" + String(offset[0]) + "</span>";
  temp +="<a class=\"pm\" href=\"/id0P\">+</a><br>";
 
  //ID1 offset
  temp +="ID1<br>";
  temp +="<a class=\"pm\" href=\"/id1M\">-</a>";
  temp +="<span>" + String(offset[1]) + "</span>";
  temp +="<a class=\"pm\" href=\"/id1P\">+</a><br>";
 
  //ID2 offset
  temp +="ID2<br>";
  temp +="<a class=\"pm\" href=\"/id2M\">-</a>";
  temp +="<span>" + String(offset[2]) + "</span>";
  temp +="<a class=\"pm\" href=\"/id2P\">+</a><br>";
  
  //ID3 offset
  temp +="ID3<br>";
  temp +="<a class=\"pm\" href=\"/id3M\">-</a>";
  temp +="<span>" + String(offset[3]) + "</span>";
  temp +="<a class=\"pm\" href=\"/id3P\">+</a><br>";
 
  //ID4 offset
  temp +="ID4<br>";
  temp +="<a class=\"pm\" href=\"/id4M\">-</a>";
  temp +="<span>" + String(offset[4]) + "</span>";
  temp +="<a class=\"pm\" href=\"/id4P\">+</a><br>";
 
  //ID5 offset
  temp +="ID5<br>";
  temp +="<a class=\"pm\" href=\"/id5M\">-</a>";
  temp +="<span>" + String(offset[5]) + "</span>";
  temp +="<a class=\"pm\" href=\"/id5P\">+</a><br>";
 
  //ID6 offset
  temp +="ID6<br>";
  temp +="<a class=\"pm\" href=\"/id6M\">-</a>";
  temp +="<span>" + String(offset[6]) + "</span>";
  temp +="<a class=\"pm\" href=\"/id6P\">+</a><br>";
  
  //ID7 offset
  temp +="ID7<br>";
  temp +="<a class=\"pm\" href=\"/id7M\">-</a>";
  temp +="<span>" + String(offset[7]) + "</span>";
  temp +="<a class=\"pm\" href=\"/id7P\">+</a><br>";
  
   
  temp +="</div>";
  temp +="</body>";
  server.send(200, "text/HTML", temp);
}
 
 
 
void handleId0M() {
  if(offset[0] > -511){
    offset[0] -= 2;
    preferences.putInt("offset0", offset[0]);
  }
  handleRoot();
}
 
void handleId0P() {
  if(offset[0] < 511){
    offset[0] += 2;
    preferences.putInt("offset0", offset[0]);
  }
  handleRoot();
}
 
 
void handleId1M() {
  if(offset[1] > -511){
    offset[1] -= 2;
    preferences.putInt("offset1", offset[1]);
  }
  handleRoot();
}
 
void handleId1P() {
  if(offset[1] < 511){
    offset[1] += 2;
    preferences.putInt("offset1", offset[1]);
  }
  handleRoot();
}
 
 
void handleId2M() {
  if(offset[2] > -511){
    offset[2] -= 2;
    preferences.putInt("offset2", offset[2]);
  }
  handleRoot();
}
 
void handleId2P() {
  if(offset[2] < 511){
    offset[2] += 2;
    preferences.putInt("offset2", offset[2]);
  }
  handleRoot();
}
 
void handleId3M() {
  if(offset[3] > -511){
    offset[3] -= 2;
    preferences.putInt("offset3", offset[3]);
  }
  handleRoot();
}
 
void handleId3P() {
  if(offset[3] < 511){
    offset[3] += 2;
    preferences.putInt("offset3", offset[3]);
  }
  handleRoot();
}
 
 
void handleId4M() {
  if(offset[4] > -511){
    offset[4] -= 2;
    preferences.putInt("offset4", offset[4]);
  }
  handleRoot();
}
 
void handleId4P() {
  if(offset[4] < 511){
    offset[4] += 2;
    preferences.putInt("offset4", offset[4]);
  }
  handleRoot();
}
 
 
void handleId5M() {
  if(offset[5] > -511){
    offset[5] -= 2;
    preferences.putInt("offset5", offset[5]);
  }
  handleRoot();
}
 
void handleId5P() {
  if(offset[5] < 511){
    offset[5] += 2;
    preferences.putInt("offset5", offset[5]);
  }
  handleRoot();
}
 
 
void handleId6M() {
  if(offset[6] > -511){
    offset[6] -= 2;
    preferences.putInt("offset6", offset[6]);
  }
  handleRoot();
}
 
void handleId6P() {
  if(offset[6] < 511){
    offset[6] += 2;
    preferences.putInt("offset6", offset[6]);
  }
  handleRoot();
}
 
 
void handleId7M() {
  if(offset[7] > -511){
    offset[7] -= 2;
    preferences.putInt("offset7", offset[7]);
  }
  handleRoot();
}
 
void handleId7P() {
  if(offset[7] < 511){
    offset[7] += 2;
    preferences.putInt("offset7", offset[7]);
  }
  handleRoot();
}
 
 
 
void setup() {
  M5.begin(true, true, true); //SerialEnable, bool I2CEnable, DisplayEnable
  
  Serial1.begin(1000000, SERIAL_8N1, -1, 19);
  
 
  //parameter memory
  preferences.begin("parameter", false);
 
 
  offset[0] =preferences.getInt("offset0", offset[0]);
  offset[1] =preferences.getInt("offset1", offset[1]);
  offset[2] =preferences.getInt("offset2", offset[2]);
  offset[3] =preferences.getInt("offset3", offset[3]);
  offset[4] =preferences.getInt("offset4", offset[4]);
  offset[5] =preferences.getInt("offset5", offset[5]);
  offset[6] =preferences.getInt("offset6", offset[6]);
  offset[7] =preferences.getInt("offset7", offset[7]);
 
 
  WiFi.softAP(ssid, pass);           
  delay(100);
  WiFi.softAPConfig(ip, ip, subnet);
  
  IPAddress myIP = WiFi.softAPIP();
  
 
  server.on("/", handleRoot); 
  
 
  server.on("/id0M", handleId0M);
  server.on("/id0P", handleId0P);
  server.on("/id1M", handleId1M);
  server.on("/id1P", handleId1P);
  server.on("/id2M", handleId2M);
  server.on("/id2P", handleId2P);
  server.on("/id3M", handleId3M);
  server.on("/id3P", handleId3P);
  server.on("/id4M", handleId4M);
  server.on("/id4P", handleId4P);
  server.on("/id5M", handleId5M);
  server.on("/id5P", handleId5P);
  server.on("/id6M", handleId6M);
  server.on("/id6P", handleId6P);
  server.on("/id7M", handleId7M);
  server.on("/id7P", handleId7P);
 
  
  server.begin(); 
}
 
 
void loop() {
  servo_write(0, 0);
  servo_write(1, 0);
  servo_write(2, 0);
  servo_write(3, 0);
  servo_write(4, 0);
  servo_write(5, 0);
  servo_write(6, 0);
  servo_write(7, 0);
 
  server.handleClient();
 
  delay(300);
}
 
 
//参考 https://qiita.com/Ninagawa123/items/7b79c5f5117dd1470ac9
void scs_moveToPos(byte id, int position) {
  // コマンドパケットを作成
  byte message[13];
  message[0] = 0xFF;  // ヘッダ
  message[1] = 0xFF;  // ヘッダ
  message[2] = id;    // サーボID
  message[3] = 9;     // パケットデータ長
  message[4] = 3;     // コマンド（3は書き込み命令）
  message[5] = 42;    // レジスタ先頭番号
  message[6] = (position >> 8) & 0xFF; // 位置情報バイト上位
  message[7] = position & 0xFF; // 位置情報バイト下位
  message[8] = 0x00;  // 時間情報バイト下位
  message[9] = 0x00;  // 時間情報バイト上位
  message[10] = 0x00; // 速度情報バイト下位
  message[11] = 0x00; // 速度情報バイト上位
 
  // チェックサムの計算
  byte checksum = 0;
  for (int i = 2; i < 12; i++) {
    checksum += message[i];
  }
  message[12] = ~checksum; // チェックサム
 
  // コマンドパケットを送信
  for (int i = 0; i < 13; i++) {
    Serial1.write(message[i]);
  }
}
 
 
void servo_write(int ch, float ang){
  int sig = 511 + offset[ch] + int((512.0 / 150.0) * ang);
  scs_moveToPos(ch, sig);
}