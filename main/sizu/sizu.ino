#include <M5Atom.h>
#include <Preferences.h>
#include <Kalman.h>
#include <WiFi.h>
#include <WebServer.h>
 
 
WebServer server(80);
const char ssid[] = "robot0009";  // SSID
const char pass[] = "password";   // password
const IPAddress ip(192, 168, 55, 27);      //　IP address
const IPAddress subnet(255, 255, 255, 0);
 
#define led_pin 27
#define NUM_LEDS 25
 
Preferences preferences;
 
//           ID  0   1   2  3   4   5   6   7
int offset[] = {-16, -6, 10, 20, -34, 10, -18, 34}; 
 
//leg length
float L1 = 50.0, L2 = 65.0;//unit:mm
 
 
Kalman kalmanX, kalmanY;
float kalAngleX, kalAngleDotX, kalAngleY, kalAngleDotY;
unsigned long oldTime = 0, loopTime, nowTime;
float dt;
float accX = 0, accY = 0, accZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;
float theta_X = 0.0, theta_Y = 0.0;
float theta_Xdot = 0.0, theta_Ydot = 0.0;
 
float hX, hY, hXpre, hYpre;
 
int pos = 0;
 
 
int Mode;
#define L 1
#define R 2
#define Imu 3
#define Jump 4
#define Jump2 42
#define Step 5
#define Stretch 6
#define Advance 7
#define Back 8
#define Roll 9
 
String modeBtJump = "off";
String modeBtStep = "off";
String modeBtImu = "off";
String modeBtStretch = "off";
String modeBtAd = "off";
String modeBtBack = "off";
String modeBtL = "off";
String modeBtR = "off";
String modeBtRoll = "off";
 
 
//パラメータ
//-------------------------------------------------
float wakeUpAngle = -1.0;
int period = 80, x = 60, height = 40, upHeight = 10, stride = 12;
float Kp = 0.3, Kd = 0.3;
//-------------------------------------------------
 
 
//加速度センサから傾きデータ取得 [deg]
void get_theta() {
  M5.IMU.getAccelData(&accX,&accY,&accZ);
  //傾斜角導出 単位はdeg
  theta_X =  atan(accY / -accZ) * 57.29578f;
  theta_Y = -atan(-accX / -accZ) * 57.29578f;
 
  if(Mode != Jump2){
    if(pos == 1 && accZ < -0.96) pos = 0;
    if(pos == 0 && accZ > 0.96) pos = 1;
  }
}
 
//Y軸 角速度取得
void get_gyro_data() {
  M5.IMU.getGyroData(&gyroX,&gyroY,&gyroZ);
  theta_Xdot = -gyroX;
  theta_Ydot = gyroY;
}
 
 
//browser
void handleRoot() {
  String temp ="<!DOCTYPE html> \n<html lang=\"ja\">";
  temp +="<head>";
  temp +="<meta charset=\"utf-8\">";
  temp +="<title>robot0009</title>";
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
  temp +="<h3>robot0009</h3>";
 
  //button
  temp +="<div class=\"column-3\">";
  temp +="<button class=\"" + modeBtImu + "\" type=\"button\" ><a href=\"/imu\">IMU</a></button><br>";
  temp +="<button class=\"" + modeBtAd + "\" type=\"button\" ><a href=\"/ad\">Advance</a></button><br>";
  temp +="<button class=\"" + modeBtJump + "\" type=\"button\" ><a href=\"/Jump\">Jump</a></button><br>";
  temp +="<button class=\"" + modeBtL + "\" type=\"button\" ><a href=\"/L\">L</a></button><br>";
  temp +="<button class=\"" + modeBtStep + "\" type=\"button\" ><a href=\"/step\">Step</a></button><br>";
  temp +="<button class=\"" + modeBtR + "\" type=\"button\" ><a href=\"/R\">R</a></button><br>";
  temp +="<button class=\"" + modeBtStretch + "\" type=\"button\" ><a href=\"/stretch\">Stretch</a></button><br>";
  temp +="<button class=\"" + modeBtBack + "\" type=\"button\" ><a href=\"/back\">Back</a></button><br>";
  temp +="<button class=\"" + modeBtRoll + "\" type=\"button\" ><a href=\"/Roll\">Roll</a></button><br>";
  temp +="</div>";
 
  //wakeUpAngle
  temp +="wakeUpAngle (deg)<br>";
  temp +="<a class=\"pm\" href=\"/wakeUpAngleM\">-</a>";
  temp +="<span>" + String(wakeUpAngle) + "</span>";
  temp +="<a class=\"pm\" href=\"/wakeUpAngleP\">+</a><br>";
 
 
  //period
  temp +="period (msec)<br>";
  temp +="<a class=\"pm\" href=\"/periodM\">-</a>";
  temp +="<span>" + String(period) + "</span>";
  temp +="<a class=\"pm\" href=\"/periodP\">+</a><br>";
 
  //x
  temp +="x (mm)<br>";
  temp +="<a class=\"pm\" href=\"/xM\">-</a>";
  temp +="<span>" + String(x) + "</span>";
  temp +="<a class=\"pm\" href=\"/xP\">+</a><br>";
  
  //height
  temp +="height (mm)<br>";
  temp +="<a class=\"pm\" href=\"/heightM\">-</a>";
  temp +="<span>" + String(height) + "</span>";
  temp +="<a class=\"pm\" href=\"/heightP\">+</a><br>";
 
  //upHeight
  temp +="upHeight (mm)<br>";
  temp +="<a class=\"pm\" href=\"/upHeightM\">-</a>";
  temp +="<span>" + String(upHeight) + "</span>";
  temp +="<a class=\"pm\" href=\"/upHeightP\">+</a><br>";
 
  //stride
  temp +="stride (mm)<br>";
  temp +="<a class=\"pm\" href=\"/strideM\">-</a>";
  temp +="<span>" + String(stride) + "</span>";
  temp +="<a class=\"pm\" href=\"/strideP\">+</a><br>";
 
 
  //Kp
  temp +="Kp<br>";
  temp +="<a class=\"pm\" href=\"/KpM\">-</a>";
  temp +="<span>" + String(Kp) + "</span>";
  temp +="<a class=\"pm\" href=\"/KpP\">+</a><br>";
 
  //Kd
  temp +="Kd<br>";
  temp +="<a class=\"pm\" href=\"/KdM\">-</a>";
  temp +="<span>" + String(Kd) + "</span>";
  temp +="<a class=\"pm\" href=\"/KdP\">+</a><br>";
   
  temp +="</div>";
  temp +="</body>";
  server.send(200, "text/HTML", temp);
}
 
void handleL() {
  if(modeBtL == "off"){
    modeBtL = "on";
    modeBtR = "off";
    modeBtJump = "off";
    modeBtStep = "off";
    modeBtStretch = "off";
    modeBtAd = "off";
    modeBtBack = "off";
    modeBtImu = "off";
    modeBtRoll = "off";
 
    Mode = L;
  }else{
    modeBtL = "off";
 
    Mode = 0;
  }
  handleRoot();
}
 
void handleR() {
  if(modeBtR == "off"){
    modeBtL = "off";
    modeBtR = "on";
    modeBtJump = "off";
    modeBtStep = "off";
    modeBtStretch = "off";
    modeBtAd = "off";
    modeBtBack = "off";
    modeBtImu = "off";
    modeBtRoll = "off";
 
    Mode = R;
  }else{
    modeBtR = "off";
 
    Mode = 0;
  }
  handleRoot();
}
 
void handleJump() {
  if(modeBtJump == "off" && pos == 0){
    modeBtL = "off";
    modeBtR = "off";
    modeBtJump = "on";
    modeBtStep = "off";
    modeBtStretch = "off";
    modeBtAd = "off";
    modeBtBack = "off";
    modeBtImu = "off";
    modeBtRoll = "off";
 
    Mode = Jump;
  }else{
    modeBtJump = "off";
 
    Mode = 0;
  }
  handleRoot();
}
 
void handleStep() {
  if(modeBtStep == "off"){
    modeBtL = "off";
    modeBtR = "off";
    modeBtJump = "off";
    modeBtStep = "on";
    modeBtStretch = "off";
    modeBtAd = "off";
    modeBtBack = "off";
    modeBtImu = "off";
    modeBtRoll = "off";
 
    Mode = Step;
  }else{
    modeBtStep = "off";
 
    Mode = 0;
  }
  handleRoot();
}
 
 
void handleImu() {
  if(modeBtImu == "off"){
    modeBtL = "off";
    modeBtR = "off";
    modeBtJump = "off";
    modeBtStep = "off";
    modeBtStretch = "off";
    modeBtAd = "off";
    modeBtBack = "off";
    modeBtImu = "on";
    modeBtRoll = "off";
 
    Mode = Imu;
  }else{
    modeBtImu = "off";
 
    Mode = 0;
  }
  handleRoot();
}
 
void handleStretch() {
  if(modeBtStretch == "off"){
    modeBtL = "off";
    modeBtR = "off";
    modeBtJump = "off";
    modeBtStep = "off";
    modeBtStretch = "on";
    modeBtAd = "off";
    modeBtBack = "off";
    modeBtImu = "off";
    modeBtRoll = "off";
 
    Mode = Stretch;
  }else{
    modeBtStretch = "off";
 
    Mode = 0;
  }
  handleRoot();
}
 
void handleAd() {
  if(modeBtAd == "off"){
    modeBtL = "off";
    modeBtR = "off";
    modeBtJump = "off";
    modeBtStep = "off";
    modeBtStretch = "off";
    modeBtAd = "on";
    modeBtBack = "off";
    modeBtImu = "off";
    modeBtRoll = "off";
 
    Mode = Advance;
  }else{
    modeBtAd = "off";
 
    Mode = 0;
  }
  handleRoot();
}
 
void handleBack() {
  if(modeBtBack == "off"){
    modeBtL = "off";
    modeBtR = "off";
    modeBtJump = "off";
    modeBtStep = "off";
    modeBtStretch = "off";
    modeBtAd = "off";
    modeBtBack = "on";
    modeBtImu = "off";
    modeBtRoll = "off";
 
    Mode = Back;
  }else{
    modeBtBack = "off";
 
    Mode = 0;
  }
  handleRoot();
}
 
void handleRoll() {
  if(modeBtRoll == "off"){
    modeBtL = "off";
    modeBtR = "off";
    modeBtJump = "off";
    modeBtStep = "off";
    modeBtStretch = "off";
    modeBtAd = "off";
    modeBtBack = "off";
    modeBtImu = "off";
    modeBtRoll = "on";
 
    Mode = Roll;
  }else{
    modeBtRoll = "off";
 
    Mode = 0;
  }
  handleRoot();
}
 
 
void handleWakeUpAngleM() {
  if(wakeUpAngle > -90.0){
    wakeUpAngle -= 0.5;
    preferences.putFloat("wakeUpAngle", wakeUpAngle);
  }
  handleRoot();
}
void handleWakeUpAngleP() {
  if(wakeUpAngle <= 90.0){
    wakeUpAngle += 0.5;
    preferences.putFloat("wakeUpAngle", wakeUpAngle);
  }
  handleRoot();
}
 
 
 
void handleKpM() {
  if(Kp > 0){
    Kp -= 0.1;
    preferences.putFloat("Kp", Kp);
  }
  handleRoot();
}
void handleKpP() {
  if(Kp <= 10){
    Kp += 0.1;
    preferences.putFloat("Kp", Kp);
  }
  handleRoot();
}
 
void handleKdM() {
  if(Kd > 0){
    Kd -= 0.1;
    preferences.putFloat("Kd", Kd);
  }
  handleRoot();
}
void handleKdP() {
  if(Kd <= 10){
    Kd += 0.1;
    preferences.putFloat("Kd", Kd);
  }
  handleRoot();
}
 
 
 
 
void handlePeriodM() {
  if(period > 5){
    period -= 5;
    preferences.putInt("period", period);
  }
  handleRoot();
}
void handlePeriodP() {
  if(period <= 3000){
    period += 5;
    preferences.putInt("period", period);
  }
  handleRoot();
}
 
 
void handlexM() {
  if(x > -615){
    x -= 5;
    preferences.putInt("x", x);
  }
  handleRoot();
}
void handlexP() {
  if(x < 515){
    x += 5;
    preferences.putInt("x", x);
  }
  handleRoot();
}
 
 
void handleHeightM() {
  if(height > -120){
    height -= 5;
    preferences.putInt("height", height);
  }
  handleRoot();
}
void handleHeightP() {
  if(height <= 120){
    height += 5;
    preferences.putInt("height", height);
  }
  handleRoot();
}
 
void handleUpHeightM() {
  if(upHeight > 0){
    upHeight -= 2;
    preferences.putInt("upHeight", upHeight);
  }
  handleRoot();
}
void handleUpHeightP() {
  if(upHeight <= 500){
    upHeight += 2;
    preferences.putInt("upHeight", upHeight);
  }
  handleRoot();
}
 
void handleStrideM() {
  if(stride > 0){
    stride -= 2;
    preferences.putInt("stride", stride);
  }
  handleRoot();
}
void handleStrideP() {
  if(stride <= 40){
    stride += 2;
    preferences.putInt("stride", stride);
  }
  handleRoot();
}
 
 
//Core0
void browser(void *pvParameters) {
  M5.dis.setBrightness(20);
  
  for (;;){
    server.handleClient();
 
    //LED表示
    M5.dis.clear();
      
    if(kalAngleX > 20.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i, 0xff0000);
      }
    }else if(kalAngleX <= 20.0 && kalAngleX > 12.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i, 0x0000ff);
      }
    }else if(kalAngleX <= 12.0 && kalAngleX > 4.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i + 5, 0x0000ff);
      }
    }else if(kalAngleX <= 4.0 && kalAngleX > 1.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i + 10, 0x0000ff);
      }
    }else if(abs(kalAngleX) <= 1.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i + 10, 0x00ff00);
      }
    }else if(kalAngleX >= -4.0 && kalAngleX < -1.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i + 10, 0x0000ff);
      }
    }else if(kalAngleX >= -12.0 && kalAngleX < -4.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i + 15, 0x0000ff);
      }
    }else if(kalAngleX >= -20.0 && kalAngleX < -12.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i + 20, 0x0000ff);
      }
    }else if(kalAngleX < -20.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i + 20, 0xff0000);
      }
    }
      
    if(kalAngleY > 20.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i * 5, 0xff0000);
      }
    }else if(kalAngleY <= 20.0 && kalAngleY > 12.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i * 5, 0x0000ff);
      }
    }else if(kalAngleY <= 12.0 && kalAngleY > 4.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i * 5 + 1, 0x0000ff);
      }
    }else if(kalAngleY <= 4.0 && kalAngleY > 1.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i * 5 + 2, 0x0000ff);
      }
    }else if(abs(kalAngleY) <= 1.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i * 5 + 2, 0x00ff00);
      }
    }else if(kalAngleY >= -4.0 && kalAngleY < -1.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i * 5 + 2, 0x0000ff);
      }
    }else if(kalAngleY >= -12.0 && kalAngleY < -4.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i * 5 + 3, 0x0000ff);
      }
    }else if(kalAngleY >= -20.0 && kalAngleY < -12.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i * 5 + 4, 0x0000ff);
      }
    }else if(kalAngleY < -20.0){
      for(int i = 0; i < 5; i++){
        M5.dis.drawpix(i * 5 + 4, 0xff0000);
      }
    }
 
    delay(50);
      
    disableCore0WDT();
  }
}
 
 
void setup() {
  M5.begin(true, true, true); //SerialEnable, bool I2CEnable, DisplayEnable
  
  Serial1.begin(1000000, SERIAL_8N1, -1, 19);
  
  M5.IMU.Init();
 
  //フルスケールレンジ
  M5.IMU.SetAccelFsr(M5.IMU.AFS_2G);
  M5.IMU.SetGyroFsr(M5.IMU.GFS_250DPS);
 
  get_theta();
  kalmanX.setAngle(theta_X);
  kalmanY.setAngle(theta_Y);
 
  delay(200); 
  
 
  //parameter memory
  preferences.begin("parameter", false);
 
  wakeUpAngle = preferences.getFloat("wakeUpAngle", wakeUpAngle);
 
  Kp = preferences.getFloat("Kp", Kp);
  Kd = preferences.getFloat("Kd", Kd);
  
  period = preferences.getInt("period", period);
  x = preferences.getInt("x", x);
  height = preferences.getInt("height", height);
  upHeight = preferences.getInt("upHeight", upHeight);
  stride = preferences.getInt("stride", stride);
 
 
  WiFi.softAP(ssid, pass);           
  delay(100);
  WiFi.softAPConfig(ip, ip, subnet);
  
  IPAddress myIP = WiFi.softAPIP();
  
 
  server.on("/", handleRoot); 
  
  server.on("/Jump", handleJump);
  server.on("/step", handleStep);
  server.on("/imu", handleImu);
  server.on("/stretch", handleStretch);
  server.on("/ad", handleAd);
  server.on("/back", handleBack);
  server.on("/L", handleL);
  server.on("/R", handleR);
  server.on("/Roll", handleRoll);
 
 
  server.on("/wakeUpAngleM", handleWakeUpAngleM);
  server.on("/wakeUpAngleP", handleWakeUpAngleP);
 
  server.on("/KpM", handleKpM);
  server.on("/KpP", handleKpP);
  server.on("/KdM", handleKdM);
  server.on("/KdP", handleKdP);
  
  server.on("/periodM", handlePeriodM);
  server.on("/periodP", handlePeriodP);
  server.on("/xM", handlexM);
  server.on("/xP", handlexP);
  server.on("/heightM", handleHeightM);
  server.on("/heightP", handleHeightP);
  server.on("/upHeightM", handleUpHeightM);
  server.on("/upHeightP", handleUpHeightP);
  server.on("/strideM", handleStrideM);
  server.on("/strideP", handleStrideP);
  
  server.begin(); 
 
  //browser task
  xTaskCreatePinnedToCore(
    browser
    ,  "browser"   // A name just for humans
    ,  4096  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  0);
}
 
 
void loop() {
  nowTime = micros();
  loopTime = nowTime - oldTime;
  oldTime = nowTime;
  
  dt = (float)loopTime / 1000000.0; //sec
 
  get_theta();
  get_gyro_data();
  
  //カルマンフィルタ 姿勢 傾き
  kalAngleX = kalmanX.getAngle(theta_X, theta_Xdot, dt);
  kalAngleY = kalmanY.getAngle(theta_Y, theta_Ydot, dt);
  
  //カルマンフィルタ 姿勢 角速度
  kalAngleDotX = kalmanX.getRate();
  kalAngleDotY = kalmanY.getRate();
 
  //Serial.println(kalAngleX);
 
 
  if(Mode == Jump){ //ジャンプ
    fRIK(20, 35);
    fLIK(20, 35);
    rLIK(-25, 95);
    rRIK(-25, 95);
    delay(500);
 
    fRIK(-35, 100);
    fLIK(-35, 100);
    delay(160);
    
    
    rRIK(107, -42);
    rLIK(107, -42);
    fRIK(-70, 90);
    fLIK(-70, 90);
    delay(100);
 
    rRIK(80, 30);
    rLIK(80, 30);
    
    Mode = Jump2;
 
    fRIK(-85, 40);
    fLIK(-85, 40);
 
  }else if(Mode == Jump2){
    if(kalAngleX < 0.0 && kalAngleX > wakeUpAngle){
      fRIK(60, 40);
      fLIK(60, 40);
      rRIK(60, 40);
      rLIK(60, 40);
      
      Mode = 0;
    }
  }else if(Mode == Stretch){ //屈伸
    float tim,tt;
    unsigned long time_mSt= millis();
 
    tim=0;    
    while(tim<period * 8){
      tim=millis()-time_mSt;
      tt=float(tim * 2 * PI / (period * 8));
      fRIK(x, height + upHeight * sin(tt));
      fLIK(x, height + upHeight * sin(tt));
      rLIK(x, height + upHeight * sin(tt));
      rRIK(x, height + upHeight * sin(tt));
    }
  }else if(Mode == Step){ //足踏み
    float tim,tt;
    unsigned long time_mSt= millis();
 
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x, height + upHeight * (0.5 * cos(2*tt) - 0.5));
      fLIK(x, height);
      rLIK(x, height + upHeight * (0.5 * cos(2*tt) - 0.5));
      rRIK(x, height);
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x, height + upHeight * (-0.5 * cos(2*tt) - 0.5));
      fLIK(x, height);
      rLIK(x, height + upHeight * (-0.5 * cos(2*tt) - 0.5));
      rRIK(x, height);
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x, height);
      fLIK(x, height + upHeight * (0.5 * cos(2*tt) - 0.5));
      rLIK(x, height);
      rRIK(x, height + upHeight * (0.5 * cos(2*tt) - 0.5));
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x, height);
      fLIK(x, height + upHeight * (-0.5 * cos(2*tt) - 0.5));
      rLIK(x, height);
      rRIK(x, height + upHeight * (-0.5 * cos(2*tt) - 0.5));
    }
  }else if(Mode == Advance){ //前進
    float tim,tt;
    unsigned long time_mSt= millis();
 
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x - stride * cos(tt), height + upHeight * (0.5 * cos(2*tt) - 0.5));
      fLIK(x + stride * cos(tt), height);
      rLIK(x + stride * cos(tt), height + upHeight * (0.5 * cos(2*tt) - 0.5));
      rRIK(x - stride * cos(tt), height);
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x + stride * sin(tt), height + upHeight * (-0.5 * cos(2*tt) - 0.5));
      fLIK(x - stride * sin(tt), height);
      rLIK(x - stride * sin(tt), height + upHeight * (-0.5 * cos(2*tt) - 0.5));
      rRIK(x + stride * sin(tt), height);
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x + stride * cos(tt), height);
      fLIK(x - stride * cos(tt), height + upHeight * (0.5 * cos(2*tt) - 0.5));
      rLIK(x - stride * cos(tt), height);
      rRIK(x + stride * cos(tt), height + upHeight * (0.5 * cos(2*tt) - 0.5));
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x - stride * sin(tt), height);
      fLIK(x + stride * sin(tt), height + upHeight * (-0.5 * cos(2*tt) - 0.5));
      rLIK(x + stride * sin(tt), height);
      rRIK(x - stride * sin(tt), height + upHeight * (-0.5 * cos(2*tt) - 0.5));
    }
  }else if(Mode == Back){ //後進
    float tim,tt;
    unsigned long time_mSt= millis();
 
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x + stride * cos(tt), height + upHeight * (0.5 * cos(2*tt) - 0.5));
      fLIK(x - stride * cos(tt), height);
      rLIK(x - stride * cos(tt), height + upHeight * (0.5 * cos(2*tt) - 0.5));
      rRIK(x + stride * cos(tt), height);
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x - stride * sin(tt), height + upHeight * (-0.5 * cos(2*tt) - 0.5));
      fLIK(x + stride * sin(tt), height);
      rLIK(x + stride * sin(tt), height + upHeight * (-0.5 * cos(2*tt) - 0.5));
      rRIK(x - stride * sin(tt), height);
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x - stride * cos(tt), height);
      fLIK(x + stride * cos(tt), height + upHeight * (0.5 * cos(2*tt) - 0.5));
      rLIK(x + stride * cos(tt), height);
      rRIK(x - stride * cos(tt), height + upHeight * (0.5 * cos(2*tt) - 0.5));
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(x + stride * sin(tt), height);
      fLIK(x - stride * sin(tt), height + upHeight * (-0.5 * cos(2*tt) - 0.5));
      rLIK(x - stride * sin(tt), height);
      rRIK(x + stride * sin(tt), height + upHeight * (-0.5 * cos(2*tt) - 0.5));
    }
  }else if(Mode == R){ //R
    float tim,tt;
    unsigned long time_mSt= millis();
 
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(10, 40 + 10 * (0.5 * cos(2*tt) - 0.5));
      fLIK(10 + 10 * cos(tt), 40);
      rLIK(10 + 10 * cos(tt), 40 + 10 * (0.5 * cos(2*tt) - 0.5));
      rRIK(10, 40);
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(10, 40 + 10 * (-0.5 * cos(2*tt) - 0.5));
      fLIK(10 - 10 * sin(tt), 40);
      rLIK(10 - 10 * sin(tt), 40 + 10 *(-0.5 * cos(2*tt) - 0.5));
      rRIK(10, 40);
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(10, 40);
      fLIK(10 - 10 * cos(tt), 40 + 10 * (0.5 * cos(2*tt) - 0.5));
      rLIK(10 - 10 * cos(tt), 40);
      rRIK(10, 40 + 10 * (0.5 * cos(2*tt) - 0.5));
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(10, 40);
      fLIK(10 + 10 * sin(tt), 40 + 10 * (-0.5 * cos(2*tt) - 0.5));
      rLIK(10 + 10 * sin(tt), 40);
      rRIK(10, 40 + 10 * (-0.5 * cos(2*tt) - 0.5));
    }
  }else if(Mode == L){ //L
    float tim,tt;
    unsigned long time_mSt= millis();
 
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(10 - 10 * cos(tt), 40 + 10 * (0.5 * cos(2*tt) - 0.5));
      fLIK(10, 40);
      rLIK(10, 40 + 10 * (0.5 * cos(2*tt) - 0.5));
      rRIK(10 - 10 * cos(tt), 40);
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(10 + 10 * sin(tt), 40 + 10 * (-0.5 * cos(2*tt) - 0.5));
      fLIK(10, 40);
      rLIK(10, 40 + 10 *(-0.5 * cos(2*tt) - 0.5));
      rRIK(10 + 10 * sin(tt), 40);
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(10 + 10 * cos(tt), 40);
      fLIK(10, 40 + 10 * (0.5 * cos(2*tt) - 0.5));
      rLIK(10, 40);
      rRIK(10 + 10 * cos(tt), 40 + 10 * (0.5 * cos(2*tt) - 0.5));
    }
 
    time_mSt= millis();
    tim=0;    
    while(tim<period){
      tim=millis()-time_mSt;
      tt=float(tim * PI / 2 / period);
      fRIK(10 - 10 * sin(tt), 40);
      fLIK(10, 40 + 10 * (-0.5 * cos(2*tt) - 0.5));
      rLIK(10, 40);
      rRIK(10 - 10 * sin(tt), 40 + 10 * (-0.5 * cos(2*tt) - 0.5));
    }
  }else if(Mode == Roll){ //反転
    fRIK(50, 30);
    fLIK(50, 30);
    rRIK(50, 30);
    rLIK(50, 30);
    delay(500);
 
    fRIK(10, 45);
    fLIK(10, 45);
    rRIK(0, 80);
    rLIK(0, 80);
    delay(500);
 
    fRIK(10, 90);
    fLIK(10, 90);  
    delay(150);
 
    fRIK(70, -50);
    fLIK(70, -50);
    delay(200);
    
    rRIK(70, -50);
    rLIK(70, -50);
    delay(600);
    
    Mode = 0;
  }else if(Mode == Imu){
    hX = hXpre + Kp / 100.0 * kalAngleX + Kd / 100.0 * kalAngleDotX;
    hY = hYpre + Kp / 100.0 * kalAngleY + Kd / 100.0 * kalAngleDotY;
 
    hXpre = hX;
    hYpre = hY;
 
    if(pos == 1){ //反転時
      hX = -hX; 
      hY = -hY;
    }
 
    fRIK(x, height - hX + hY);
    fLIK(x, height - hX - hY);
    rRIK(x, height + hX + hY);
    rLIK(x, height + hX - hY);
  }else{ //初期姿勢
    fRIK(x, height);
    fLIK(x, height);
    rRIK(x, height);
    rLIK(x, height);
  }
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
  //delayMicroseconds(500);
}
 
 
 
//-------------------------------------------
//IK
//-------------------------------------------
void fRIK(float x, float z){
  float phi,ld, th1, th2;
  
  ld = sqrt(x*x + z*z);
  phi = atan2(x, z);
 
  th1 = 90.0 - (phi + acos((L1*L1 - L2*L2 +ld*ld)/(2*L1*ld))) * 180.0 / PI;
  th2 = acos((ld*ld - L1*L1 - L2*L2)/(2*L1*L2)) * 180.0 / PI;
 
  if(pos == 0){
    th2 = constrain(th2, -120.0, 120.0);
    th1 = constrain(th1, -80.0, 120.0);
    servo_write(6, th2);
    servo_write(4, th1);
  }else{
    th2 = constrain(th2, -120.0, 120.0);
    th1 = constrain(th1, -120.0, 80.0);
    servo_write(6, -th2);
    servo_write(4, -th1);
  }
}
 
void fLIK(float x, float z){
  float phi,ld, th1, th2;
  
  ld = sqrt(x*x + z*z);
  phi = atan2(x, z);
 
  th1 = 90.0 - (phi + acos((L1*L1 - L2*L2 +ld*ld)/(2*L1*ld))) * 180.0 / PI;
  th2 = acos((ld*ld - L1*L1 - L2*L2)/(2*L1*L2)) * 180.0 / PI;
 
  if(pos == 0){
    th2 = constrain(th2, -120.0, 120.0);
    th1 = constrain(th1, -80.0, 120.0);
    servo_write(7, -th2);
    servo_write(5, -th1);
  }else{
    th2 = constrain(th2, -120.0, 120.0);
    th1 = constrain(th1, -120.0, 80.0);
    servo_write(7, th2);
    servo_write(5, th1);
  }
}
 
void rRIK(float x, float z){
  float phi,ld, th1, th2;
  
  ld = sqrt(x*x + z*z);
  phi = atan2(x, z);
 
  th1 = 90.0 - (phi + acos((L1*L1 - L2*L2 +ld*ld)/(2*L1*ld))) * 180.0 / PI;
  th2 = acos((ld*ld - L1*L1 - L2*L2)/(2*L1*L2)) * 180.0 / PI;
 
  if(pos == 0){
    th2 = constrain(th2, -120.0, 120.0);
    th1 = constrain(th1, -80.0, 120.0);
    servo_write(3, -th2);
    servo_write(1, -th1);
  }else{
    th2 = constrain(th2, -120.0, 120.0);
    th1 = constrain(th1, -120.0, 80.0);
    servo_write(3, th2);
    servo_write(1, th1);
  }
}
 
void rLIK(float x, float z){
  float phi,ld, th1, th2;
  
  ld = sqrt(x*x + z*z);
  phi = atan2(x, z);
 
  th1 = 90.0 - (phi + acos((L1*L1 - L2*L2 +ld*ld)/(2*L1*ld))) * 180.0 / PI;
  th2 = acos((ld*ld - L1*L1 - L2*L2)/(2*L1*L2)) * 180.0 / PI;
 
  if(pos == 0){
    th2 = constrain(th2, -120.0, 120.0);
    th1 = constrain(th1, -80.0, 120.0);
    servo_write(2, th2);
    servo_write(0, th1);
  }else{
    th2 = constrain(th2, -120.0, 120.0);
    th1 = constrain(th1, -120.0, 80.0);
    servo_write(2, -th2);
    servo_write(0, -th1);
  }
}