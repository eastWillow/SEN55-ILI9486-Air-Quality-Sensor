/*
 * NOTE: This file is adapted from Waveshare drivers.
 * Original License: No explicit open-source license found (Copyright Waveshare).
 * Used under assumption of compatibility or fair use as per user instruction (Option B).
 */
/*****************************************************************************
* | File      	:	Debug.h
* | Author      :   Waveshare team
* | Function    :	debug with prntf
* | Info        :
*   Image scanning
*      Please use progressive scanning to generate images or fonts
*----------------
* |	This version:   V1.0
* | Date        :   2018-01-11
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __DEBUG_H
#define __DEBUG_H

#define DEV_DEBUG 0

#if DEV_DEBUG
    #ifdef ARDUINO
        #include <Arduino.h>
        #define DEBUG(__info) Serial.print(__info)
    #else
        #include <iostream>
        #define DEBUG(__info) std::cout << __info
    #endif
#else
    #define DEBUG(__info)
#endif

#endif
