#ifndef BALANZA_H
#define BALANZA_H
#include <Arduino.h>

// Definición de la clase Balanza
class Balanza {
    public:   
        Balanza(int pin, float factor); // Constructor de la cosita
    //Metodos
    void inicio(); //Calibracion inicial
    void tare(); //Poner en cero
    int ObtenerPeso(); //Obtener peso en gramos
    int ObtenerTareVisual(); //Obtener valor de la tara para mostrar
    int GetVoltaje(); //Obtener el voltaje actual en mV

    private:
    int _pin; //Pin de lectura
    float _factor; //Factor de conversion segun la cuestion
    int _offsetIniciar;  //Calibracion al encender el sistema
    int _voltajeTare; //Los mv del Tare
    int _Voltaje;      //Voltaje crudo en mV
};

#endif



