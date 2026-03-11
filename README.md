# ⚖️ Sistema de Pesaje Industrial ESP32 (4-20mA)

Este proyecto implementa una balanza industrial utilizando un **ESP32** y un sensor de peso con salida de **4-20mA**. El sistema incluye monitoreo remoto vía **Modbus TCP/IP**, visualización local en **LCD**, y control de carga mediante un **relé**.

## 🚀 Características
* **Lectura Industrial**: Integración de lazo de corriente 4-20mA mediante resistencia de carga de $150 \Omega$.
* **Calibración Precisa**: Algoritmo de regresión lineal para conversión de mV a Gramos.
* **Conectividad**: Servidor Modbus IP para integración con sistemas SCADA.
* **Interfaz Local**: Pantalla LCD 16x2 y LED RGB (NeoPixel) para estados del sistema.
* **Control de Tiempo**: Cronómetro integrado para procesos de llenado/dosificación.

## 🛠️ Hardware Utilizado
* **Microcontrolador**: ESP32
* **Acondicionamiento**: Resistencia de precisión $150 \Omega$
* **Salida de Potencia**: Transistor 2N2222 + Relé para bombillo/carga 110V
* **Display**: LCD 16x2 (RS, E, D4-D7)

## 📈 Calibración
El factor de conversión utilizado es `0.4287`, obtenido mediante una tabla de 11 puntos (0g a 1000g) procesada en MATLAB.