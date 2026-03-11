#ifndef CRONOMETRO_H
#define CRONOMETRO_H
#include <Arduino.h>

class Cronometro {
    public:
    Cronometro(int StopwatchPin); // Constructor del cronometro
    void iniciar(); 
    void actualizar(); //Actualizar el cronometro
    //Controles de la cuestion
    void Start();
    void Stop();
    void Reset();
    void Cucu(); //El On/Off de la vainita
    // Mostrar los datos
int GetMinutos();
int GetSegundos();
bool isRunning();

    private:
    int _pin;
    bool _running;
    int _minutos;
    int _segundos;
    unsigned long _ultimosegundo;
    //el anti rebote del botón
    int _lastbuttonstate;
    unsigned long _ultimorebote;
    const unsigned long _delayrebote = 69; // El número de la suerte

};


#endif