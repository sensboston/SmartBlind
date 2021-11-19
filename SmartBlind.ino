#define USE_OTA
#define USE_IR
#define USE_WEB_SERVER
#define USE_PREFERENCES
#define USE_ACCELERATION
//#define USE_MQTT
#ifndef USE_MQTT
#define USE_SINRIC_PRO
#endif

#include <WiFi.h>
#include <AccelStepper.h>
#ifdef USE_PREFERENCES
#include <Preferences.h>
#endif

#ifdef USE_OTA
#include <ArduinoOTA.h>
#endif
#ifdef USE_WEB_SERVER
#include <WebServer.h>
#endif
#ifdef USE_IR
#include <IRremote.h>
#endif
#ifdef USE_MQTT
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#else
#ifdef USE_SINRIC_PRO
#include "SinricPro.h"
#include "SinricProBlinds.h"
#endif
#endif

#define IR_RECEIVE_PIN  15

#define DIR_PIN         5
#define STEP_PIN        18
#define MAX_MOTOR_SPEED 2000.0
#define MOTOR_SPEED     980.0
#define ACCELERATION    900.0

#define STANDARD_ROLL   -1
#define INVERSE_ROLL    1
#define MOTOR_ON_RIGHT  -1  
#define MOTOR_ON_LEFT   1
#define STOPPED         0
#define OPENING         1
#define CLOSING         2

#define MAX_DISTANCE    STANDARD_ROLL * MOTOR_ON_LEFT * (int) (1036*9.2)  // 1036 - full turn
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

#define WIFI_HOST       "SmartBlind"
#define WIFI_SSID       "XXXXX"
#define WIFI_PASS       "XXXXX"

#ifdef USE_MQTT
#define IO_SERVER       "io.adafruit.com"
#define IO_SERVERPORT   1883                  
#define IO_USERNAME     "xxxxxxxxxxxx"
#define IO_KEY          "aio_xxxxxxxxxxxxxxxxxxxxxxxx"
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, IO_SERVER, IO_SERVERPORT, IO_USERNAME, IO_KEY);       
Adafruit_MQTT_Subscribe Blind_Control = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME"/feeds/smart-blind");
Adafruit_MQTT_Subscribe *subscription;
#else
#define APP_KEY         "2xx1a817-40a8-xxxx-af26-xxxxxxxxxxxxxx"                                       
#define APP_SECRET      "fxx99618-7a81-4422-be07-xxxxxxxxxxxxxx-b1848b51-xxxx-40bd-xxxx-316ce4acffe8"
#define BLINDS_ID       "XXXXXXXX"
#endif

#ifdef USE_WEB_SERVER
WebServer server(80);
#define GO_BACK server.sendHeader("Location", "/",true); server.send(302, "text/plane","");
#endif

#ifdef USE_PREFERENCES
Preferences preferences;
#endif
unsigned long prevMillis = 0;
bool prevMotorRunning = false;
bool powerState = false;
int blindsPosition = 0;
int currentState = STOPPED;

#ifdef USE_WEB_SERVER
const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html>
<head>
    <title>Smart blind remote control</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <meta name="mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <link rel="icon" href="http://senssoft.com/blind.ico" type="image/x-icon" />
    <link rel="shortcut icon" href="http://senssoft.com/blind.ico" type="image/x-icon" />
    <link rel="apple-touch-icon" href="http://senssoft.com/blind.png" />
    <style>
    table {
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
    }
    div {
        display: grid;
        place-items: center;
        align-content: center;
    }
    a.colorButton {
        display: inline-block;
        padding: 0.7em 1.7em;
        margin: 0 0.3em 0.3em 0;
        border-radius: 0.2em;
        box-sizing: border-box;
        text-decoration: none;
        font-family: 'Roboto', sans-serif;
        font-weight: bold;
        font-size: 1.1em;
        background-color: #3369ff;
        box-shadow: inset 0 -0.6em 1em -0.35em rgba(0, 0, 0, 0.17), inset 0 0.6em 2em -0.3em rgba(255, 255, 255, 0.15), inset 0 0 0em 0.05em rgba(255, 255, 255, 0.12);
        text-align: center;
        position: relative;
        vertical-align: middle;
        width: 100%;
        user-select: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
    }
    a.colorButton:active {
        box-shadow: inset 0 0.6em 2em -0.3em rgba(0, 0, 0, 0.15), inset 0 0 0em 0.05em rgba(255, 255, 255, 0.12);
    }
    @media all and (max-width:30em) {
        a.colorButton {
            display: block;
            margin: 0.4em auto;
        }
    }
    </style>
</head>
<body style="touch-action: pan-x pan-y;">
    <table style="height:75%; width:75%; ">
        <tr style="height: 33%;">
            <td>
                <a class="colorButton" onclick="window.location.replace('/command?cmd=up');" style="background-color:#E5E7E9">  <span style="font-size:2em; border-right:1px solid rgba(0,0,0,0.5); padding-right:0.3em; margin-right:0.3em; vertical-align:middle">▲&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Up</a>
            </td>
        </tr>
        <tr style="height: 33%;">
            <td>
                <a class="colorButton" onclick="window.location.replace('/command?cmd=down');" style="background-color:#E5E7E9">  <span style="font-size:2em; border-right:1px solid rgba(0,0,0,0.5); padding-right:0.3em; margin-right:0.3em; vertical-align:middle">&nbsp;▼&nbsp;&nbsp;&nbsp;&nbsp;</span>&nbsp;&nbsp;&nbsp;Down</a>
            </td>
        </tr>
        <tr style="height: 33%;">
            <td>
                <a class="colorButton" onclick="window.location.replace('/command?cmd=stop');" style="background-color:#E5E7E9; color:#C40000;">  <span style="color: #00000; font-size:2em; border-right:1px solid rgba(0,0,0,0.5); padding-right:0.3em; margin-right:0.3em; vertical-align:middle; color:#C40000;">■&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>&nbsp;&nbsp;&nbsp;Stop</a>
            </td>
        </tr>
    </table>
</body>
</html>)rawliteral";
#endif

void stopMotor()
{
    Serial.println("STOP command");
    stepper.setSpeed(0);
    stepper.stop();
    currentState = STOPPED;
#ifdef USE_PREFERENCES    
    preferences.putLong("pos", stepper.currentPosition());
#endif
    delay(500);
}

void closeBlind()
{
    Serial.println("CLOSE BLIND command");
    if (currentState == OPENING) stopMotor();
    stepper.moveTo(MAX_DISTANCE);
#ifndef USE_ACCELERATION    
    stepper.setSpeed(MOTOR_SPEED);
#endif    
    currentState = CLOSING;
}

void openBlind()
{
    Serial.println("OPEN BLIND command");
    if (currentState == CLOSING) stopMotor();
    stepper.moveTo(0);
#ifndef USE_ACCELERATION    
    stepper.setSpeed(MOTOR_SPEED);
#endif    
    currentState = OPENING;    
}

void handleHTTPCommand()
{
    if (server.args() == 1)
    {
        String cmd = server.arg(0);
        Serial.print("Got HTTP command: "); Serial.println(cmd);
        if (cmd=="up") openBlind();
        else if (cmd=="down") closeBlind();
        else if (cmd=="stop") stopMotor();
    }
    GO_BACK
}

#ifndef USE_MQTT
bool onPowerState(const String &deviceId, bool &state) 
{
  powerState = state;
  return true;
}

bool onRangeValue(const String &deviceId, int &position) {
  Serial.printf("onRangeValue:  Device %s set position to %d\r\n", deviceId.c_str(), position);
  if (position == 0) closeBlind(); else openBlind();
  return true;
}

bool onAdjustRangeValue(const String &deviceId, int &positionDelta) {
  blindsPosition += positionDelta;
  Serial.printf("onAdjustRangeValue:  Device %s position changed about %i to %d\r\n", deviceId.c_str(), positionDelta, blindsPosition);
  positionDelta = blindsPosition; // calculate and return absolute position
  return true;
}
#endif

void setup() 
{
    Serial.begin(115200);

#ifdef USE_PREFERENCES
    preferences.begin("smartblind", false);
    long pos = preferences.getLong("pos", 0);
#else    
    long pos = 0;
#endif    
    stepper.setCurrentPosition(pos);
    stepper.setMaxSpeed(MAX_MOTOR_SPEED);
#ifdef USE_ACCELERATION
    stepper.setAcceleration(ACCELERATION);    
#endif

#ifdef USE_IR  
    // Start IR receiver service
    IrReceiver.begin(IR_RECEIVE_PIN);
#endif    

    // connect to WiFi
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(WIFI_HOST);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    int i=0;
    while (WiFi.status() != WL_CONNECTED && i++ < 30) { 
        delay(200); 
        Serial.print("."); 
    }
    
    if (WiFi.status() == WL_CONNECTED) 
    {
        Serial.print("IP address: "); Serial.println(WiFi.localIP());
#ifdef USE_OTA
        ArduinoOTA
            .onStart([]() {
                String type;
                if (ArduinoOTA.getCommand() == U_FLASH) type = "sketch"; else type = "filesystem";
                Serial.println("Start updating " + type);
            })
            .onEnd([]() { Serial.println("\nEnd"); })
            .onProgress([](unsigned int progress, unsigned int total) { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
            .onError([](ota_error_t error) { Serial.printf("Error[%u]: ", error); });
    
        ArduinoOTA.setHostname(WIFI_HOST);
        ArduinoOTA.begin();
    }
#endif    

#ifdef USE_WEB_SERVER
    server.on("/", []() { server.send(200, "text/html", index_html); } );
    server.on("/command", handleHTTPCommand);
    server.on("/favicon.ico", []() { server.sendHeader("Location", "http://senssoft.com/blind.ico",true); server.send(302, "text/plane",""); });
    server.onNotFound( [](){ GO_BACK });
    server.begin();
#endif    

#ifdef USE_MQTT
    // Subscribe for Adafruit IO feed
    mqtt.subscribe(&Blind_Control);
    mqtt.connect();
#else
#ifdef USE_SINRIC_PRO
    // get a new Blinds device from SinricPro
    SinricProBlinds &myBlinds = SinricPro[BLINDS_ID];
    myBlinds.onPowerState(onPowerState);
    myBlinds.onRangeValue(onRangeValue);
    myBlinds.onAdjustRangeValue(onAdjustRangeValue);

    // setup SinricPro
    SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
    SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
    SinricPro.begin(APP_KEY, APP_SECRET);
#endif    
#endif
}

void loop() 
{
    // Check WiFi status every 5 minutes
    if(!stepper.isRunning() && (millis()-prevMillis > 5*60*1000))
    {
        if (WiFi.status() != WL_CONNECTED)
        {
#ifdef USE_PREFERENCES            
            preferences.putLong("pos", stepper.currentPosition()); 
            preferences.end();
#endif            
            ESP.restart();
        }
        else prevMillis = millis();
    }

#ifdef USE_OTA    
    ArduinoOTA.handle();
#endif    

#ifdef USE_IR
    if (IrReceiver.decode())
    {
        if (IrReceiver.decodedIRData.numberOfBits == 32)
        {
            Serial.println(IrReceiver.decodedIRData.decodedRawData);
            switch (IrReceiver.decodedIRData.decodedRawData)
            {
                // close blind: TV - ">>", big LED remote - "☼↓", small LED remote - "+"
                case 0xA45BFB04: /*case 0xA25DFF00: case 0xB54AFF00:*/  closeBlind(); break;
                // open blind:  TV - "<<", big LED remote - "↑☼", small LED remote - "-"
                case 0xB04FFB04: /*case 0xA35CFF00: case 0xAD52FF00:*/  openBlind(); break;
                // stop motor:  TV - "SLEEP", big LED remote - "►│", small LED remote - "A7"
                case 0xE41BFB04: /* case 0xBE41FF00: case 0xBD42FF00: */ stopMotor(); break;
            }
        }
        IrReceiver.resume();
    }
#endif    

    if (currentState != STOPPED)
    {
#ifdef USE_ACCELERATION
        stepper.run();
#else
        stepper.runSpeedToPosition();
#endif
    }    

    // Save position if motor stopped running
    bool motorRunning = stepper.isRunning();
    if (prevMotorRunning != motorRunning)
    {
        prevMotorRunning = motorRunning;
#ifdef USE_PREFERENCES        
        if (!motorRunning) preferences.putLong("pos", stepper.currentPosition());
#endif        
    }

    if (WiFi.status() == WL_CONNECTED) 
    {
#ifdef USE_WEB_SERVER        
        server.handleClient();
#endif        

        if (!stepper.isRunning())
        {
#ifdef USE_MQTT
            if (mqtt.connected())
            {
                while ((subscription = mqtt.readSubscription(250))) 
                {
                    if (subscription == &Blind_Control)
                    { 
                        String cmd = String ((char*)Blind_Control.lastread);
                        cmd.toLowerCase();
                        Serial.println(cmd);
                        if ( (cmd.indexOf("open") > 0) || (cmd.indexOf("light") > 0) || (cmd.indexOf("up") > 0) ) openBlind();
                        else if ( (cmd.indexOf("close") > 0) || (cmd.indexOf("dark") > 0) || (cmd.indexOf("down") > 0) ) closeBlind();
                    }
                }
            }
            else mqtt.connect();
#else
#ifdef USE_SINRIC_PRO
            SinricPro.handle();
#endif            
#endif
        }
    } 
}
