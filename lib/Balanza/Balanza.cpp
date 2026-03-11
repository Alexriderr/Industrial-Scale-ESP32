#include "Balanza.h"

Balanza::Balanza(int pin, float factor) {
    _pin = pin;
    _factor = factor;
    _offsetIniciar = 0;
    _voltajeTare = 0;
    _Voltaje = 0;
}
//Encendido del sistema, calibracion inicial
void Balanza::inicio() {
    long sumacogia =0;
    for(int i=0;i<69;i++) {
        sumacogia += analogReadMilliVolts(_pin);
        delay(20);
    }
    _offsetIniciar = sumacogia / 69;
}
//Tare de la balanza, poner en cero
void Balanza::tare() {
    int lecturaactual = analogReadMilliVolts(_pin);
    _voltajeTare = lecturaactual - _offsetIniciar;
}
// Obtencion del peso en gramos
int Balanza::ObtenerPeso() {
long  Sumaviolada = 0;
    for(int i=0;i<33;i++){
        Sumaviolada += analogReadMilliVolts(_pin);
        delay(2);
    }
    int lecturaactual = Sumaviolada / 33;
    _Voltaje = lecturaactual;
    int voltajecalibrado = lecturaactual - _offsetIniciar - _voltajeTare;
    int pesoengramos = voltajecalibrado * _factor;
    return pesoengramos;
    }
//Registro del peso del tare para mostrarlo en el scada
    int Balanza::ObtenerTareVisual() {
    int pesoTare = _voltajeTare * _factor;
    return pesoTare;
}
//Obtencion del voltaje actual en mV
int Balanza::GetVoltaje() {
    return _Voltaje;
}