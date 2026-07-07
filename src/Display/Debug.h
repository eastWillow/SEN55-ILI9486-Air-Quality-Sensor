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
    #include "../App/Log.h"
    #define DEBUG(...) App_Log(__VA_ARGS__)
#else
    #define DEBUG(...)
#endif

#endif
