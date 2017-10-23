/**
* Copyright (c) D. Schneider, 2016 <daedae@gmx.ch>
*
* This program is free software: you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation, either version 3 of the License, or (at your option) any later
* version.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
* details.
* 
* You should have received a copy of the GNU General Public License along with
* this program. If not, see <http://www.gnu.org/licenses/>.
*
* https://github.com/DedeHai/FastLEDPainter
*/


#include "Arduino.h"
#include <FastLED.h> 

#ifndef FastLEDPainter_h
#define FastLEDPainter_h

#define SPEEDSHIFT 12 //bit shift for sub-pixel speed of brush movement (1<<SPEEDSHIFT = one pixel)




typedef struct {
	uint8_t h; //hue
	uint8_t s; //saturation
	uint8_t v; //value
	uint8_t fadespeed; //fadespeed (lower is slower)
	uint8_t fadevalue; //selected type (h, s, v) is faded to this value
	//flags:
	uint8_t skipfadeupdate : 1; //skp the next fade update (used for smooth & accurate fading)
	uint8_t fadevalue_out : 1; //boolean, fade out (once the full brightness is reached when using fadein)
	uint8_t fadevalue_in : 1; //fade from zero to fadevalue using fadespeed
	uint8_t fadehue_up : 1; //fade hue, counting up from initial hue
	uint8_t fadehue_down : 1; //fade hue, counting down from initial hue
	uint8_t fadesaturation_in : 1; //fade from zero to fadevalue
	uint8_t fadesaturation_out : 1; //fade from initialvalue to 0
} hsvcanvas;

typedef struct {
	int32_t i; //index, fixed point float shifted by 10 bits (subpixel position sampling to easily allow slow speeds without a lot of if's)
	int16_t speed; //moving speed in subpixel space (speed = 255 means one pixel per frame)
	int16_t speedlimit;
	uint16_t position; //last pixel index that was painted
	uint8_t hue; //hue to paint
	uint8_t saturation; //saturation to paint
	uint8_t value; //value to paint
	uint8_t fadespeed;
	//flags:
	uint8_t fadevalue_out : 1; //boolean, fade out (once the full brightness is reached when using fadein)
	uint8_t fadevalue_in : 1; //fade from zero to fadevalue using fadespeed
	uint8_t fadehue_near : 1; //fade hue, nearest angle on colorwheel
	uint8_t fadehue_far : 1; //fade hue, furthest angle on colorwheel
	uint8_t fadesaturation_in : 1; //fade from zero to fadevalue
	uint8_t fadesaturation_out : 1; //fade from initialvalue to 0
	uint8_t bounce : 1; //speed is reversed when hitting the end of the strip (as opposed to continue at the other side)
} brush;


class FastLEDPainterCanvas
{
public:
	FastLEDPainterCanvas(uint16_t count);
	~FastLEDPainterCanvas(void);
	bool isvalid(void);
	void clear(void);
	void transfer(void);
	uint16_t getNumberofPixels(void); //read the protected _numbpixels variable
	hsvcanvas* _canvas;
	
private:
	uint16_t _numpixels; //number of pixels, needed at initialization so it cannot be read from FastLED.size() 
	uint16_t _speedcounter; //counter for smooth fading without using floats
	void addColorRGB(int index, CRGB RGBcolor);  
	void addColorHSV(int index, CHSV HSVcolor);
};


class FastLEDPainterBrush
{
public:
	FastLEDPainterBrush(FastLEDPainterCanvas* hsv_canvas);
	~FastLEDPainterBrush(void);
	bool isvalid(void);
	void paint(void);
	void moveTo(uint16_t position);
	int16_t getPosition(void);
	void setSpeed(int16_t speed);
	uint16_t getSpeed(void);
	void setSpeedlimit(int16_t limit);
	void setColor(CHSV HSVcolor);
	CHSV getColor(void);
	void setFadeSpeed(uint8_t fadeSpeed);
	uint8_t getFadeSpeed(void);
	void setFadeout(bool value);
	void setFadein(bool value);
	void setFadeHueNear(bool value);
	void setFadeHueFar(bool value);
	void setFadeSaturation_in(bool value);
	void setFadeSaturation_out(bool value);
	void setBounce(bool value);

private:
	brush* _brush; 
	FastLEDPainterCanvas* _canvastopaint;
};


#endif



