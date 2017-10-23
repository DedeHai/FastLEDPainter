# FastLED Painter Library
https://github.com/DedeHai/FastLEDPainter
A library to ease the creation of LED-strip animations. See examples for some inspiration.
[Demo Video](https://youtu.be/PYAIwoRsGbo)

#How to use
The basic principle is to use a brush that can be moved to any pixel. The brush holds a color using the HSV color space
rather than RGB. This allows for making colorful animations with color fading as well as brightness fading. 
To make the brush move on its own, give it a speed and it will move a little on each function call. 
It only paints each pixel once, even if it stays at the same location for a longer time.

The brush paints on a canvas, which holds one HSV color for each pixel as well as a value to fade to, the fade speed and some fading properties (flags).
You should only use one fading type (hue, saturation or value) at a time. Although multiple fadings are allowed, each pixel only 
holds one 'fade-to' value to not use excessive amounts of RAM.
Also, all fading is implemented without the use of floats. This makes it fast and uses a lot less of the precious RAM. 
The downside is, that color fadings are slightly less accurate but almost indistinguishable to the naked eye because I
implemented the fadings using 16bit variables and bit-shifts (aka fixed point arithmetic), making it almost as accurate as when using floats.

##Basic procedure
- create a CRGB led array in the size of the number of LEDs
- create a canvas object (mycanvas), passing the number of LEDs 
- create a brush object (mybrush), passing it the canvas object
- set the brush properties (see examples for some inspiration)
- apply the paint to the canvas by calling mybrush.paint()
- clear the LED color buffer by calling FastLED.clear()
- transfer the paint from the canvas to the color buffer by calling mycanvas.transfer()
- send out the data to the LEDs by calling FastLED.show()
- loop the last four steps (paint the canvas, clear the buffer, transfer the paint, sendout data)



#Reference
##FastLEDPainterBrush-Class functions
- **FastLEDPainterBrush(FastLEDPainterCanvas* hsv_canvas)** 
  - class constructor, taking a pointer to a canvas (use &canvasname to pass the pointer)
- **~FastLEDPainterBrush(void)** 
  - class deconstructor, is called automatically when the created class leaves the local scope (i.e. a loop)
- **bool isvalid(void)** 
  - returns true if memory allocation for the class was successful. Use it if you are not sure if enough memory was available
- **void paint(void)**
  - applies all the painting properties to the current pixel of assigned canvas and updates the brush (moves the brush if it has any speed). Call this before calling canvas.transfer()
- **void moveTo(uint16_t position)**
  - move the brush to a certain pixel in the strip
- **int16_t getPosition(void)** 
  - read current pixel position the brush is at
- **void setSpeed(int16_t speed)** 
  - set the speed of the brush. since sub-pixel speed is used, one pixel per iteration is (1<<12) or 4095
- **uint16_t getSpeed(void)** 
  - get the current speed 
- **void setSpeedlimit(int16_t limit)** 
  - set a limit the brush cannot exceed. useful when using sensors for speed mapping
- **void setColor(HSV color)** 
  - set the color of the brush in HSV, see examples for detail
- **HSV getColor(void)** 
  - read the current color of the brush
- **void setFadeSpeed(uint8_t fadeSpeed)**
  - set the fade speed of all fading effects (cannot have different speeds since the canvas can only hold one speed per pixel)
- **uint8_t getFadeSpeed(void)**
  - read the current fade speed of the brush
- **void setFadeout(bool value)**
  - activate brightness fade-out. fade-out starts as soon as the maximum brightness is reached (if fade-in is also used)
- **void setFadein(bool value)**
  - activate fade-in: pixel will start out dark and fade to the set color with the set fading speed
- **void setFadeHueNear(bool value)**
  - fade hue from current pixel color of the canvas to the painted color chosing the nearest path around the colorcircle
- **void setFadeHueFar(bool value)**
  - like setFadeHueNear but going the long way around the colorcircle
- **void setFadeSaturation_in(bool value)**
  - the canvas will fade the color saturation from current value to the painted value (if painted value is smaller than current value, use FadeSaturation_out)
- **void setFadeSaturation_out(bool value)**
  - the canvas will fade the color saturation from the current value to the set value (or to zero if saturation fade-in is also active)
- **void setBounce(bool value)**
  - bounce the brush once it reaches either end of the strip instead of jumping to the other end

##FastLEDPainterCanvas-Class functions
- **FastLEDPainterCanvas(uint16_t numberofleds)**
  - class constructor, pass the number of LEDs used
- **~FastLEDPainterCanvas(void)**
  - class deconstructor, is called automatically when the created class leaves the local scope (i.e. a loop)
- **bool isvalid(void)**
  - returns true if memory allocation for the class was successful. Use it if you are not sure if enough memory was available. The canvas uses a lot of memory: 6 bytes per pixel.
- **void clear(void)**
  - clear the canvas from all paint
- **void transfer(void)**
  - transfer the paint from the canvas to the LEDs. Always call this function before calling the FastLED.show() function.
