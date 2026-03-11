#include "Cronometro.h"
Cronometro::Cronometro(int StopwatchPin) {
    _pin = StopwatchPin;
    _running = false;
    _minutos = 0;
    _segundos = 0;
    _ultimosegundo = 0;
    _lastbuttonstate = HIGH; //MOSCA el boton debe estar conectado a una resistencia pull-up para que esto camine
    _ultimorebote = 0;
}
void Cronometro::iniciar() {
    pinMode(_pin, INPUT_PULLUP); // Ya aqui esta la pull-up, conecta la verga directo al pin y el otro extremo a tierra
    }
    void Cronometro::actualizar() {
    int buttonstate = digitalRead(_pin);
    if (buttonstate == LOW && _lastbuttonstate == HIGH) {
        if (millis() - _ultimorebote > _delayrebote) {
            _running = !_running;
            _ultimorebote = millis();
        }
    }
    _lastbuttonstate = buttonstate;
    // Contar el tiempo
    if (_running) {
        if (millis() - _ultimosegundo >= 1000) {
            _ultimosegundo = millis();
            _segundos++;
            if (_segundos >= 60) {
                _segundos = 0;
                _minutos++;
                //Reinicio automatico a los 99 minutos. Si lo llegas a usar, estas bien?
                if (_minutos >= 99) {
                    _minutos = 0;
                    _segundos = 0;
                }
            }
        }
    } else {
        _ultimosegundo = millis(); // Pa cuando se pause la cuestion
    }
}
// Controles de la cuestion
void Cronometro::Start() {
    _running = true;
}
void Cronometro::Stop() {
    _running = false;
}
void Cronometro::Reset() {
    _minutos = 0;
    _segundos = 0;
    _running = false;
}
void Cronometro::Cucu() {
    _running = !_running;

}
// Mostrar los datos
int Cronometro::GetMinutos() {
    return _minutos;
}
int Cronometro::GetSegundos() {
    return _segundos;
}
bool Cronometro::isRunning() {
    return _running;
}