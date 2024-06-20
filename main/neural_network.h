// neural_network.h
// TOI 1.projekt
// Autor: Jan Tomeček, FIT
// Vyvýjeno ve VSCode pomocí ESP-IDF rozšíření
#ifndef __NN_TOI__
#define __NN_TOI__

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif


EXTERNC void NNInit();
EXTERNC float* getNNInput();
EXTERNC float* getNNPrediction();


#endif