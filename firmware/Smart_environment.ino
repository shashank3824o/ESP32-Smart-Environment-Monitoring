#define BLYNK_TEMPLATE_ID "TMPL3mp8AbEzn"
#define BLYNK_TEMPLATE_NAME "Smart Environment Monitoring"
#define BLYNK_AUTH_TOKEN "tpiP5-OEM4cRt7Ju6gXWpOIE0CWN2xHn"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
bool tempAlertSent = false;
// Enter your Wi-Fi/hotspot details here.
// Keep these private; do not share screenshots containing passwords or tokens.
const char* ssid = "Recharge kra";
const char* password = "87654321";

// Pins
#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ135_PIN 34
#define BUZZER_PIN 25
#define LED_PIN 18

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

float temperature = NAN;
float humidity = NAN;
int mq135Raw = 0;
unsigned long lastRead = 0;

void handleData() {
  String json = "{";
  json += "\"temperature\":";
  json += isnan(temperature) ? "null" : String(temperature, 1);
  json += ",\"humidity\":";
  json += isnan(humidity) ? "null" : String(humidity, 1);
  json += ",\"mq135\":" + String(mq135Raw);
  json += ",\"wifi\":";
  json += (WiFi.status() == WL_CONNECTED) ? "true" : "false";
  json += ",\"uptime\":" + String(millis() / 1000);
  json += "}";
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", json);
}

void handleRoot() {
  String html = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 Smart Environment Monitor</title>
<style>
:root{color-scheme:dark}
*{box-sizing:border-box}
body{margin:0;padding:20px;font-family:Arial,sans-serif;background:#0f172a;color:#e5e7eb}
main{max-width:1000px;margin:auto}
h1{margin:4px 0 6px;color:#7dd3fc;text-align:center}
.sub{text-align:center;color:#94a3b8;margin:0 0 22px}
.cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(210px,1fr));gap:14px}
.card,.chartbox{background:#1e293b;border:1px solid #334155;border-radius:14px;padding:18px;margin-bottom:14px}
.label{font-size:14px;color:#cbd5e1}
.value{font-size:30px;font-weight:700;margin:12px 0 4px;color:#86efac;overflow-wrap:anywhere}
.note{font-size:12px;color:#94a3b8}
.status{display:inline-block;padding:5px 10px;border-radius:20px;background:#334155;font-size:12px}
h2{font-size:17px;margin:0 0 10px}
canvas{width:100%;height:190px;display:block}
.footer{text-align:center;color:#94a3b8;font-size:12px;padding:8px}
</style>
</head>
<body>
<main>
<h1>Smart Environment Monitor</h1>
<p class="sub">ESP32 live sensor dashboard · updates every 2 seconds</p>
<div class="cards">
 <section class="card"><div class="label">Temperature</div><div class="value" id="temp">--</div><div class="note">DHT11 · °C</div></section>
 <section class="card"><div class="label">Humidity</div><div class="value" id="hum">--</div><div class="note">DHT11 · %</div></section>
 <section class="card"><div class="label">MQ-135 raw reading</div><div class="value" id="mq">--</div><div class="note">Raw ADC value, not AQI/ppm</div></section>
 <section class="card"><div class="label">ESP32 connection</div><div class="value" style="font-size:22px" id="wifi">Checking…</div><div class="note" id="uptime">Uptime: --</div></section>
</div>
<div class="chartbox"><h2>Temperature history</h2><canvas id="tempChart"></canvas></div>
<div class="chartbox"><h2>Humidity history</h2><canvas id="humChart"></canvas></div>
<div class="chartbox"><h2>MQ-135 raw reading history</h2><canvas id="mqChart"></canvas></div>
<p class="footer">Graph history is held in this browser tab and resets when the page is reloaded. MQ-135 raw readings need calibration before any air-quality classification.</p>
</main>
<script>
const historyLimit=60;
const histories={temp:[],hum:[],mq:[]};
function addPoint(key,value){
  if(value===null || !Number.isFinite(value)) return;
  histories[key].push(value);
  if(histories[key].length>historyLimit) histories[key].shift();
}
function drawChart(id,key,label){
  const canvas=document.getElementById(id);
  const rect=canvas.getBoundingClientRect();
  const dpr=window.devicePixelRatio||1;
  canvas.width=Math.max(300,Math.floor(rect.width*dpr));
  canvas.height=Math.floor(190*dpr);
  const ctx=canvas.getContext('2d');
  const w=canvas.width,h=canvas.height;
  ctx.clearRect(0,0,w,h);
  const pad=36*dpr, right=12*dpr, top=16*dpr, bottom=25*dpr;
  const vals=histories[key];
  ctx.font=`${11*dpr}px Arial`;
  ctx.strokeStyle='#334155';ctx.fillStyle='#94a3b8';ctx.lineWidth=1*dpr;
  for(let i=0;i<=4;i++){
    const y=top+(h-top-bottom)*i/4;
    ctx.beginPath();ctx.moveTo(pad,y);ctx.lineTo(w-right,y);ctx.stroke();
  }
  if(!vals.length){
    ctx.fillStyle='#94a3b8';ctx.fillText('Waiting for readings…',pad, h/2);
    return;
  }
  let min=Math.min(...vals),max=Math.max(...vals);
  if(min===max){min-=1;max+=1;}
  const range=max-min;
  ctx.fillStyle='#94a3b8';
  for(let i=0;i<=4;i++){
    const v=max-range*i/4;
    const y=top+(h-top-bottom)*i/4;
    ctx.fillText(v.toFixed(key==='mq'?0:1),2,y+4*dpr);
  }
  const plotW=w-pad-right,plotH=h-top-bottom;
  ctx.strokeStyle=key==='temp'?'#38bdf8':key==='hum'?'#a78bfa':'#4ade80';
  ctx.lineWidth=2.2*dpr;ctx.beginPath();
  vals.forEach((v,i)=>{
    const x=pad+(vals.length===1?plotW/2:i*plotW/(historyLimit-1));
    const y=top+(max-v)/range*plotH;
    if(i===0)ctx.moveTo(x,y);else ctx.lineTo(x,y);
  });
  ctx.stroke();
  const last=vals[vals.length-1];
  ctx.fillStyle='#e2e8f0';
  ctx.fillText('Latest: '+last.toFixed(key==='mq'?0:1),pad,h-5*dpr);
}
function redraw(){
  drawChart('tempChart','temp','Temperature');
  drawChart('humChart','hum','Humidity');
  drawChart('mqChart','mq','MQ-135');
}
async function update(){
  try{
    const response=await fetch('/data',{cache:'no-store'});
    if(!response.ok) throw new Error('HTTP '+response.status);
    const d=await response.json();
    document.getElementById('temp').textContent=d.temperature===null?'Sensor error':d.temperature.toFixed(1)+' °C';
    document.getElementById('hum').textContent=d.humidity===null?'Sensor error':d.humidity.toFixed(1)+' %';
    document.getElementById('mq').textContent=d.mq135;
    document.getElementById('wifi').textContent=d.wifi?'Connected':'Offline';
    document.getElementById('uptime').textContent='Uptime: '+d.uptime+' s';
    addPoint('temp',d.temperature);addPoint('hum',d.humidity);addPoint('mq',d.mq135);
    redraw();
  }catch(e){
    document.getElementById('wifi').textContent='Dashboard data unavailable';
  }
}
window.addEventListener('resize',redraw);
update();setInterval(update,2000);
</script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  analogReadResolution(12);
  ledcAttach(LED_PIN, 5000, 8);

  dht.begin();
  Wire.begin(21, 22);

  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Environment Monitor");
    display.println("Starting...");
    display.display();
  } else {
    Serial.println("OLED initialization failed!");
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("Dashboard IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed.");
  }

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  // Configure Blynk without replacing the existing local web dashboard.
  Blynk.config(BLYNK_AUTH_TOKEN);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connecting to Blynk...");
    if (Blynk.connect(3000)) {
      Serial.println("Blynk connected!");
    } else {
      Serial.println("Blynk not connected yet; will retry in loop.");
    }
  }
}

void loop() {
  server.handleClient();

  // Keep Blynk alive and retry connection periodically.
  if (WiFi.status() == WL_CONNECTED) {
    if (Blynk.connected()) {
      Blynk.run();
    } else {
      static unsigned long lastBlynkRetry = 0;
      if (millis() - lastBlynkRetry >= 5000) {
        lastBlynkRetry = millis();
        Blynk.connect(100);
      }
    }
  }

  if (millis() - lastRead >= 2000) {
    lastRead = millis();

    float newTemp = dht.readTemperature();
    float newHumidity = dht.readHumidity();

    if (!isnan(newTemp)) temperature = newTemp;
    if (!isnan(newHumidity)) humidity = newHumidity;

    // Average 10 ADC samples to smooth small fluctuations.
    long total = 0;
    for (int i = 0; i < 10; i++) {
      total += analogRead(MQ135_PIN);
      delay(2);
    }
    mq135Raw = total / 10;

    // Send sensor values to Blynk datastreams (V0, V1, V2).
    if (Blynk.connected()) {
      if (!isnan(temperature)) Blynk.virtualWrite(V0, temperature);
      if (!isnan(humidity)) Blynk.virtualWrite(V1, humidity);
      Blynk.virtualWrite(V2, mq135Raw);
    }
    // Send a Blynk notification when temperature exceeds 35 C.
if (Blynk.connected() && !isnan(temperature)) {
  if (temperature > 35.0 && !tempAlertSent) {
    Blynk.logEvent(
      "high_temp",
      "Alert! Temperature has exceeded 35 C."
    );

    tempAlertSent = true;
    Serial.println("High temperature alert triggered!");
  }

  // Allow another alert after temperature returns to normal.
  if (temperature <= 35.0) {
    tempAlertSent = false;
  }
}

    // Temperature alert above 30 C.
    if (!isnan(temperature) && temperature > 30) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }

    // LED brightness follows raw MQ-135 reading (demonstration only).
    int brightness = map(mq135Raw, 0, 4095, 0, 255);
    ledcWrite(LED_PIN, brightness);

    // OLED display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("SMART ENV MONITOR");

    display.setCursor(0, 16);
    display.print("Temp: ");
    if (isnan(temperature)) display.println("--");
    else {
      display.print(temperature, 1);
      display.println(" C");
    }

    display.setCursor(0, 28);
    display.print("Humidity: ");
    if (isnan(humidity)) display.println("--");
    else {
      display.print(humidity, 1);
      display.println(" %");
    }

    display.setCursor(0, 40);
    display.print("MQ135 Raw: ");
    display.println(mq135Raw);

    display.setCursor(0, 52);
    display.print("WiFi: ");
    display.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Offline");
    display.display();

    Serial.print("Temp: ");
    if (isnan(temperature)) Serial.print("Sensor Error");
    else Serial.print(temperature, 1);
    Serial.print(" C | Humidity: ");
    if (isnan(humidity)) Serial.print("Sensor Error");
    else Serial.print(humidity, 1);
    Serial.print(" % | MQ135 raw: ");
    Serial.println(mq135Raw);
  }
}
















