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


#include "FastLEDPainter.h"


FastLEDPainterCanvas::FastLEDPainterCanvas(uint16_t count)
{
	_numpixels = count;
	//allocate memory for a canvas, add it to the canvas list and return the pointer to the new canvas
	_canvas = (hsvcanvas*)calloc(_numpixels,sizeof(hsvcanvas)); //create an array of canvas pixels

}

FastLEDPainterCanvas::~FastLEDPainterCanvas(void)
{
	if(_canvas) free(_canvas); 
}

bool FastLEDPainterCanvas::isvalid(void)
{
	if(_canvas) return true;
	else return false;
}

void  FastLEDPainterCanvas::clear() //clear all hsv fade pixels on the canvas
{
	memset(_canvas, 0, _numpixels*sizeof(hsvcanvas));
}

//update & transfer the hsv canvas to the LEDs
void FastLEDPainterCanvas::transfer(void){

	if(isvalid()==false) return; //canvas painting array was not properly allocated
	int i;
	for (i = 0; i < _numpixels; i++)
	{
		addColorHSV(i, CHSV(_canvas[i].h, _canvas[i].s, _canvas[i].v));

		if (_canvas[i].fadespeed > 0)
		{
			//do the fading
			//the fadespeed is divided into a value that is added and a time interval
			//this allows for very slow as well as very fast fading using only one variable
			int8_t addvalue = 1;
			uint16_t interval = 800 / _canvas[i].fadespeed; //means 160.0/fadespeed with fixed decimal to avoid floats

			//formula determines the number of intervals it takes for the calculated integer interval to be
			//deviating more than 1 if a float interval was used (formula in floats: 1/(1/floatinterval-1/int_interval) )
			//on intervals lower than one, make the interval 1 and work on adder only

			if (interval > 10) //interval > 1.0
			{
				uint8_t subinterval = interval % 10;
				interval = interval / 10;
				if (subinterval > 0)
				{

					if (_speedcounter % ((interval * interval * 10 + interval * subinterval) / subinterval) == 0)  _canvas[i].skipfadeupdate = 1;

				}
			}
			else //on faster values
			{
				interval = 1;
				addvalue = (_canvas[i].fadespeed + (_canvas[i].fadespeed - 80)) / 8;
				uint8_t subvalue = addvalue % 10;
				addvalue = addvalue / 10;
				if (subvalue > 0)
				{
					if (subvalue >= 5)
					{
						addvalue++; //round up the addvalue
						if (_speedcounter %  (10 / (10 - subvalue)) == 0) addvalue = 1;
					}
					else
					{
						if (_speedcounter % (10 / subvalue) == 0) addvalue++;
					}
				}
			}


			if ((_speedcounter % interval) == 0 )
			{
				if (_canvas[i].skipfadeupdate == 0)
				{
					//fade value
					if (_canvas[i].fadevalue_in)
					{
						if (_canvas[i].fadevalue - addvalue <= _canvas[i].v)
						{
							_canvas[i].v = _canvas[i].fadevalue;
							_canvas[i].fadevalue_in = 0; //finished with fade in
						}
						else _canvas[i].v += addvalue;
					}
					else if (_canvas[i].fadevalue_out) //only fade out if fade in is finished
					{
						if (_canvas[i].v <= addvalue)
						{
							_canvas[i].v = 0;
							_canvas[i].fadevalue_out = 0; //prevents this being executed over and over again after fadout
						}
						else     _canvas[i].v -= addvalue;
					}

					//fade hue
					if (_canvas[i].fadehue_up)
					{
						if (_canvas[i].fadevalue == _canvas[i].h)
						{
							_canvas[i].fadehue_up = 0; //finished with fade in
						}
						else _canvas[i].h++;
					}
					else if (_canvas[i].fadehue_down)
					{
						if (_canvas[i].fadevalue == _canvas[i].h)
						{
							_canvas[i].fadehue_down = 0; //prevents this if being executed over and over again after fadout
						}
						else     _canvas[i].h--;
					}

					//fade saturation
					if (_canvas[i].fadesaturation_in)
					{
						if (_canvas[i].fadevalue == _canvas[i].s)
						{
							_canvas[i].fadesaturation_in = 0; //finished with fade in
						}
						else _canvas[i].s++;
					}
					else if (_canvas[i].fadesaturation_out)
					{
						if (_canvas[i].fadevalue == _canvas[i].s)
						{
							_canvas[i].fadesaturation_out = 0; //prevents this if being executed over and over again after fadout
						}
						else     _canvas[i].s--;
					}

				}
				else _canvas[i].skipfadeupdate = 0; //fadeupdate skipped, continue normally
			}
		}
	}
	_speedcounter++;
}


void FastLEDPainterCanvas::addColorRGB(int index, CRGB RGBcolor)
{
	CRGB *leddata = FastLED.leds();
	leddata[index] += RGBcolor;
}


void FastLEDPainterCanvas::addColorHSV(int index, CHSV HSVcolor)
{
	CRGB rgb;
  	hsv2rgb_rainbow( HSVcolor, rgb); //using rainbow conversion by default, change this line if you need a different conversion	
	addColorRGB(index, rgb);
}


uint16_t FastLEDPainterCanvas::getNumberofPixels(void) //read the protected _numbpixels variable
{
	return _numpixels;
}



//a brush paints a hsvfade-pixel-canvas in a color. also supports fade-in
//it moves according to its current speed (negative moves backwards)
//it only paints a pixel once (without painting any other pixel that is), even when called multiple times on the same pixel 
//to paint the same pixel (and only that one) multiple times, use the moveTo() function before painting



FastLEDPainterBrush::FastLEDPainterBrush(FastLEDPainterCanvas* hsv_canvas)
{
	
	_brush = (brush*)calloc(1,sizeof(brush)); //create an array of canvas pixels
	
	_canvastopaint = hsv_canvas;
	
	if(isvalid())
	{
		moveTo(0); //move painter to first pixel
		_brush->speedlimit = 0x6FFF; //do not limit speed initially
	} 

}

FastLEDPainterBrush::~FastLEDPainterBrush(void)
{
	if(_brush) free(_brush); //todo: is this a good idea???
}

bool FastLEDPainterBrush::isvalid(void)
{
	if(_brush!= NULL && _canvastopaint->isvalid()) return true;
	else return false;
}

//update and paint to the canvas
void FastLEDPainterBrush::paint(void){

	if(_canvastopaint == NULL || _brush == NULL) return; //make sure we got a valid pointer

	if (((_brush->i) >> SPEEDSHIFT) != _brush->position)
	{
		_brush->position = _brush->i >> SPEEDSHIFT;

		//determine which value(s) to fade and which to paint directly
		//note: only one fade-to value can be saved. if multiple are fadings are used,
		//the highest priority value is painted. priority is h,s,v (h being the highest priority)
		//so a fade of value and hue to a value of 100 and a hue of 50 will fade the value also to 50
		//fadeout can always be used becaus it fades to 0
		//to avoid this mess, more values would have to be stored in the _canvastopaint, using up too much ram (on higher ram chips, this may be added)

		_canvastopaint->_canvas[_brush->position].fadespeed = _brush->fadespeed;
		if (_brush->fadevalue_in == 0)  _canvastopaint->_canvas[_brush->position].v = _brush->value; //no fade? -> set the value
		else  _canvastopaint->_canvas[_brush->position].fadevalue = _brush->value; //use fade? set the fadevalue
		
		if ((_brush->fadesaturation_in || _brush->fadesaturation_out) == 0)  _canvastopaint->_canvas[_brush->position].s = _brush->saturation; //no fade? -> set the value
		else  _canvastopaint->_canvas[_brush->position].fadevalue = _brush->saturation; //use fade? set the fadevalue
		
		if ((_brush->fadehue_near || _brush->fadehue_far) == 0)  _canvastopaint->_canvas[_brush->position].h = _brush->hue; //no fade? -> set the value
		else  
		{
			_canvastopaint->_canvas[_brush->position].fadevalue = _brush->hue; //use fade? set the fadevalue
			//determine hue fade direction and set it

			bool count_up = true; //clockwise (count up) as default
			bool nochange = false; //do not change fading if painting the same hue again
			//find the nearest direction towards the new hue:
			if(_brush->hue > _canvastopaint->_canvas[_brush->position].h)
			{
				if(_brush->hue - _canvastopaint->_canvas[_brush->position].h > 128) 
				{
					count_up = false; //nearest path is counter clockwise

				}
			}
			else if(_brush->hue < _canvastopaint->_canvas[_brush->position].h)
			{
				if(_canvastopaint->_canvas[_brush->position].h - _brush->hue < 128) 
				{
					count_up = false; //nearest path is counter clockwise
				}
			}
			else //same color again, do not set any fading direction
			{
				nochange = true;
			}

			if(_brush->fadehue_far) count_up = !count_up; //invert direction if fading along the far path

			if(nochange == false)
			{
				if(count_up)
				{
					_canvastopaint->_canvas[_brush->position].fadehue_up = 1;
				}      
				else
				{
					_canvastopaint->_canvas[_brush->position].fadehue_down = 1;
				}
			}     

		}
		
		//paint the flags
		_canvastopaint->_canvas[_brush->position].fadevalue_in = _brush->fadevalue_in;
		_canvastopaint->_canvas[_brush->position].fadevalue_out = _brush->fadevalue_out;
		_canvastopaint->_canvas[_brush->position].fadesaturation_in = _brush->fadesaturation_in;
		_canvastopaint->_canvas[_brush->position].fadesaturation_out = _brush->fadesaturation_out;
		
	}


	//check if speed is within the limit
	//  if ( _brush->speed >  _brush->speedlimit)  _brush->speed = _brush->speedlimit;
	//  if ( _brush->speed < -_brush->speedlimit)  _brush->speed = -_brush->speedlimit;

	//apply speed
	_brush->i +=  _brush->speed;

	//jump or bounce at end of strip:
	if (_brush->i < 0)
	{
		if (_brush->bounce)
		{
			_brush->i = 0;
			_brush->speed = -_brush->speed;
		}
		else
		{
			_brush->i = ((int32_t)(_canvastopaint->getNumberofPixels() - 1)) << SPEEDSHIFT;
		}
	}
	else if (((_brush->i) >> SPEEDSHIFT) >= _canvastopaint->getNumberofPixels())
	{
		if (_brush->bounce)
		{
			_brush->i = ((int32_t)(_canvastopaint->getNumberofPixels() - 1)) << SPEEDSHIFT;
			_brush->speed = -_brush->speed;
		}
		else
		{
			_brush->i = 0;
		}
	}
}


void FastLEDPainterBrush::moveTo(uint16_t position)
{
	if(position >=  _canvastopaint->getNumberofPixels() )
	{
		position = _canvastopaint->getNumberofPixels() - 1;
	}
	_brush->i = ((int32_t)position<<SPEEDSHIFT);
	_brush->position = 0xFFFF; //invalidate the current position marking so the pixel always gets updated
}
int16_t FastLEDPainterBrush::getPosition(void)
{
	return _brush->i>>SPEEDSHIFT;
}

void FastLEDPainterBrush::setSpeed(int16_t speed)
{
	_brush->speed = speed;
}
uint16_t FastLEDPainterBrush::getSpeed(void)
{
	return _brush->speed;
}
void FastLEDPainterBrush::setSpeedlimit(int16_t limit)
{
	_brush->speedlimit = limit;
}

void FastLEDPainterBrush::setColor(CHSV HSVcolor)
{
	_brush->hue = HSVcolor.h;
	_brush->saturation = HSVcolor.s;
	_brush->value = HSVcolor.v;
}

CHSV FastLEDPainterBrush::getColor(void)
{
	CHSV returncolor;
	returncolor.h = _brush->hue;
	returncolor.s =_brush->saturation;
	returncolor.v =_brush->value;
	return returncolor;
}

void  FastLEDPainterBrush::setFadeSpeed(uint8_t fadeSpeed)
{
	_brush->fadespeed = fadeSpeed;
}
uint8_t FastLEDPainterBrush::getFadeSpeed(void)
{
	return _brush->fadespeed;
}
void FastLEDPainterBrush::setFadeout(bool value)
{
	_brush->fadevalue_out = value;
}

void FastLEDPainterBrush::setFadein(bool value)
{
	_brush->fadevalue_in = value;
}


void FastLEDPainterBrush::setFadeHueNear(bool value)
{
	_brush->fadehue_near = value;
}

void FastLEDPainterBrush::setFadeHueFar(bool value)
{
	_brush->fadehue_far = value;
}

void FastLEDPainterBrush::setFadeSaturation_in(bool value)
{
	_brush->fadesaturation_in = value;
}

void FastLEDPainterBrush::setFadeSaturation_out(bool value)
{
	_brush->fadesaturation_out = value;
}

void FastLEDPainterBrush::setBounce(bool value)
{
	_brush->bounce = value;
}




/*
stuff needed:

also: add option (define) to use much faster 8bit hsv to rgb in case anyone wants that.
does it makes transitions noticeably less smooth? 
*/




