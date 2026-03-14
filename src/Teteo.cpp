#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ModbusIP_ESP8266.h>
#include <WiFi.h>
#include <LiquidCrystal.h>
#include <Balanza.h>
#include <Cronometro.h>

//Datos Wifi
const char* ssid = "culito";
const char* password = "15012003";

//--- Registros Modbus---/
#define REG_VOLTAGE       100 //Voltaje en mV
#define REG_MIN           101 //Minutos del timer
#define REG_SEG           102 //Segundos del timer
#define REG_GR            103 //Peso en gramos
#define ValorTare         104 //Valor del Tare 
#define ConsignaCogia     105 //Setpoint del sistema 
#define Warning           106 //Alarma 
#define Culito            200 //Control de timer
#define Tare              201 //Tare de la balanza
#define Menudeo           202 //Estado del relay para el menudeo
#define DuaLipa           203 //Seleccion de modo de control del setpoint (0: Manual (con un potenciómetro), 1: Automático (Desde el SCADA))
#define nojoda            204 //Control de la luz de alarma

// Variables Globales
bool ActivoMenudeo = false;
unsigned long lastDiscoUpdate = 0;
long currentHue = 0;
int setpoint = 0;
int alarma = 0;


//Configuracion Cogia de pines

#define ADC_PIN       4    
#define LED_PIN       48     
#define TIMER_BUTTON  45    //Inicio del Temporizador
#define Consigna      5     //Pin del potenciómetro de consigna
#define RelayPin      14    // Salida a 110V
#define Tare_Button   2

// --- Pantalla LCD ---
const int rs = 38, d4 = 39, d5 = 40, d6 = 41, d7 = 42;
const int Grande = 21;
LiquidCrystal lcdGrande(rs, Grande, d4, d5, d6, d7);

// --- Led RGB ---
Adafruit_NeoPixel pixels(1, LED_PIN, NEO_GRB + NEO_KHZ800);

// --- Modbus ---
ModbusIP mb;

// --- OBJETOS A PARTIR DE LIBRERIAS ---
Balanza
 balanza(ADC_PIN, 0.4287);
Cronometro cronometro(TIMER_BUTTON);

//Configuración para ip fija
IPAddress local_IP(192, 168, 137, 69);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);

//--- Inicio de la computadora cogia ---
void setup() {
  Serial.begin(115200);
  delay(3000);
   Serial.printf("\n---⚡ SISTEMA SEXUAL INICIADO ⚡---\n");
 // Inicio del Hardware
  pixels.begin();
  pixels.setBrightness(20);
  pixels.show();
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, LOW); // Por seguridad, el relay inicia apagado
  // Boton de tare
  pinMode(Tare_Button, INPUT_PULLUP);

  // Configuraciones ADC
  analogReadResolution(12);       
  analogSetAttenuation(ADC_11db);

//Inicio de las LCD
  lcdGrande.begin(16, 2); lcdGrande.print("Inciciando");
 lcdGrande.setCursor(0, 1); 
 lcdGrande.print("La cuestion...");
  

// Inicio de Modulos
Serial.printf("⚖️ Calibrando Balanza...");
balanza.inicio();
cronometro.iniciar();


 // INTENTO CON IP FIJA
   Serial.printf("\nConectando a la red WiFi con IP fija...");
   Serial.println(local_IP);
   WiFi.config(local_IP, gateway, subnet, primaryDNS);
   WiFi.setSleep(false);
        WiFi.begin(ssid, password);
        int intentos = 0;
        bool conectado = false;
        while(intentos < 20){
            if(WiFi.status() == WL_CONNECTED){
                conectado = true;
                break;
            }
            delay(500);
            Serial.print(".");
            intentos++;
        }

        // Sistema automatico por si la vainita no agarrra --- //
        if (!conectado) {
          Serial.printf("\n\nEsa mierda no agarró 🫩");
          Serial.printf("\n🔄 ACTIVANDO MODO AUTOMÁTICO (DHCP)...");
          WiFi.disconnect();
          delay(1000);

        //Limpieza en el wifi, para que la cosita acepte lo que asigne el host
        WiFi.config(IPAddress(0,0,0,0), IPAddress(0,0,0,0), IPAddress(0,0,0,0));
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(" 🖕 ");
        }
         Serial.printf("\n✨ ¡CONEXIÓN AUTOMÁTICA ÉXITOSA!");
         Serial.printf("\n📶 IP Asignada: %s", WiFi.localIP().toString().c_str());
        } else {
          Serial.printf("\n\n✨ ¡CONEXIÓN FIJA ÉXITOSA!");
          Serial.printf("\n📶 Usando IP cogia: %s", WiFi.localIP().toString().c_str());
        }
        Serial.printf("\n--- SISTEMA SEXUAL LISTO PARA USAR ---\n\n");
      lcdGrande.clear();
      lcdGrande.print("IP: ");
      lcdGrande.print(WiFi.localIP());
      delay(1000);
        // Configuración Modbus
        mb.server();
    mb.addHreg(REG_VOLTAGE, 0);      //100
    mb.addHreg(REG_MIN, 0);          //101
    mb.addHreg(REG_SEG, 0);          //102
    mb.addHreg(REG_GR, 0);           //103
    mb.addHreg(ValorTare, 0);        //104
    mb.addHreg(ConsignaCogia, 0);    //105
    mb.addHreg(Warning,0);           //106
    mb.addCoil(Culito, false);       //200
    mb.addCoil(Tare, false);         //201
    mb.addCoil(Menudeo, false);      //202
    mb.addCoil(DuaLipa, false);      //203
    mb.addCoil(nojoda,false);        //204
  
    lcdGrande.clear();
}

void loop() {
  mb.task();
  cronometro.actualizar();
  int peso = balanza.ObtenerPeso();

  //Sincronizacion con el SCADA violao y el boton cogio
bool estadocogio = cronometro.isRunning();
bool estadoviolao = mb.Coil(Culito);
static bool culo = false;
// Caso 1 : Inicio del timer desde el boton
if (estadocogio != culo) {
  mb.Coil(Culito, estadocogio);
  culo = estadocogio;
    Serial.println(estadocogio ? "Cuenta sexual iniciada 😈" : "Fin del Round 😮‍💨");
    // Caso 2: inicio del timer desde el SCADA
} else if (estadoviolao != culo) {
  if(estadoviolao) {
  cronometro.Start();
} else {
  cronometro.Stop();
}
  mb.Coil(Culito, estadoviolao);
  culo = estadoviolao;
    Serial.println(estadoviolao ? "Cuenta sexual iniciada por telepatía 😈" : "Fin del Round por telepatía 😮‍💨");
} 

    
        //--- Lectura del tare y su logica cogia ---
        // Desde Scada
        if (mb.Coil(Tare)) {
          balanza.tare();
          mb.Coil(Tare, false);
          Serial.printf("Tare ajustado a %d gramos\n", balanza.ObtenerTareVisual());
        }
        // Desde Botón
        if (digitalRead(Tare_Button) == LOW){
          delay(50);
          if (digitalRead(Tare_Button) == LOW){
            balanza.tare();
            mb.Coil(Tare,false);
            Serial.printf("Tare ajustado a %d gramos\n", balanza.ObtenerTareVisual());
            while (digitalRead(Tare_Button) == LOW);
          }
        }
// Setpoint Local/Remoto
int pesoactual = peso;
if (mb.Coil(DuaLipa)==false) {
  //modo local, se lee el potenciómetro y se manda al SCADA
  int lecturasetpoint = analogRead(Consigna);
  setpoint = map(lecturasetpoint, 0, 4095, 0, 1000); // Mapeo de el valor del potenciómetro a un rango de peso
} else {
  //modo remoto, se ajusta el valor desde el scada
  setpoint = mb.Hreg(ConsignaCogia);
}
// --- Lógica de Histéresis Puyada ---
int margen = 20; // Gramos de holgura 
// Alarma malparia
if (pesoactual > alarma) {
  mb.Coil(nojoda, true); // Enciende la luz de alarma
} else {
  mb.Coil(nojoda, false); // Apaga la luz de alarma
}

if (pesoactual < (setpoint - margen)) {
    
    digitalWrite(RelayPin, HIGH);
    ActivoMenudeo = true;
} 
else if (pesoactual >= setpoint) {

    digitalWrite(RelayPin, LOW);
    ActivoMenudeo = false;
}
// Obtencion del valor de Alarma
alarma = mb.Hreg(Warning);

// Actualización de datos
static unsigned long lastPrint = 0;
if (millis() - lastPrint > 1000) {
  lastPrint = millis();
  lcdGrande.clear();

  // Obtencion de datos
  int minutos = cronometro.GetMinutos();
  int segundos = cronometro.GetSegundos();
  int tare = balanza.ObtenerTareVisual();
  int voltage = balanza.GetVoltaje();

  // Enviar datos por Modbus
  mb.Hreg(REG_VOLTAGE, voltage);
  mb.Hreg(REG_MIN, minutos);
  mb.Hreg(REG_SEG, segundos);
  mb.Hreg(REG_GR, peso);
  mb.Hreg(ValorTare, tare);
  mb.Hreg(ConsignaCogia, setpoint);
  mb.Hreg(Warning,alarma);
  mb.Coil(Menudeo, ActivoMenudeo);

//Mensaje en el puerto serial
Serial.printf("⏱️ T: %02d:%02d | ⚖️ Peso: %d g | ⚡ Volt: %d mV | 🎯 Set: %d g\n",  minutos, segundos, peso, voltage, setpoint);

  // Actualizar LCD
  lcdGrande.setCursor(0, 0);
  lcdGrande.printf("T:%02d:%02d %s", minutos, segundos, cronometro.isRunning() ? "ON " : "OFF");
  lcdGrande.setCursor(0, 1);
  lcdGrande.printf("P:%4dg  Z:%4dg", peso, setpoint);


}

  // Luces
pixels.show();
if (ActivoMenudeo == true) {
  if (millis() - lastDiscoUpdate > 30 ){ 
      lastDiscoUpdate = millis();
      currentHue += 1666; 
      uint32_t color = pixels.ColorHSV(currentHue, 255, 255);
      pixels.setPixelColor(0, color);
      pixels.show();
    }
} else {  if (peso> 100){
    pixels.setPixelColor(0, pixels.Color(255, 69, 0)); // Se está pesando (Naranja)
  } else {
    pixels.setPixelColor(0, pixels.Color(0, 100, 255)); // No se está pesando (Celeste)
}
  }
}
