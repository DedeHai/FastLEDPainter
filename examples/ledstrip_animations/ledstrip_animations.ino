/**
  Demo-set of animations using the FastLED painter library
  The demo animations show different ways the library can be used
  to create colorful led-strip animations with just a few lines of code.

  These examples are written to run on an Arduino Duemilenove (Uno). For
  other Arduino platforms, speeds may need to be adjusted to give the same effect
  Also, on platforms with more available RAM (like the ESP8266), more
  canvases can be used to paint even more mixing colors.

  If you come up with other good animations you want to share, please
  post the code on the GitHub repository by opening a new issue:
  https://github.com/DedeHai/FastLEDPainter/issues
  I may then add it as a demo (with proper attribution).
*/

const int duration = 2000; //number of loops to run each animation for

#define NUMBEROFPIXELS 60 //Number of LEDs on the strip
#define PIXELPIN 2 //Pin where WS281X pixels are connected


#include "Arduino.h"
#include <FastLED.h>
#include <FastLEDPainter.h>

CRGB leds[NUMBEROFPIXELS];

//create one canvas and one brush with global scope
FastLEDPainterCanvas pixelcanvas = FastLEDPainterCanvas(NUMBEROFPIXELS); //create canvas, linked to the FastLED library (canvas must be created before the brush)
FastLEDPainterBrush pixelbrush = FastLEDPainterBrush(&pixelcanvas); //crete brush, linked to the canvas to paint to


void setup() {
  randomSeed(analogRead(0)); //new random seed

  //initilize FastLED library
  FastLED.addLeds<WS2812, PIXELPIN, GRB>(leds, NUMBEROFPIXELS);

  Serial.begin(115200);
  Serial.println(" ");
  Serial.println(F("FastLED Painter Demo Animations"));

  //check if ram allocation of brushes and canvases was successful (painting will not work if unsuccessful, program should still run though)
  //this check is optional but helps to check if something does not work, especially on low ram chips like the Arduino Uno
  if (pixelcanvas.isvalid() == false) Serial.println("canvas allocation problem");
  else  Serial.println("canvas allocation ok");


  if (pixelbrush.isvalid() == false) Serial.println("brush allocation problem");
  else  Serial.println("brush allocation ok");

}

unsigned long loopcounter; //count the loops, switch to next animation after a while
bool initialized = false; //initialize the canvas & brushes in each loop when zero

void loop() {

  //main program loops through different animations, allocating addidional canvas and brushes if needed

  //---------------------
  //RAINBOW PAINT (aka nyan cat)
  //---------------------

  //the brush moves along the strip, leaving a colorful rainbow trail
  for (loopcounter = 0; loopcounter < duration; loopcounter++)
  {
    static unsigned int hue = 0; //color hue to set to brush
    CHSV brushcolor; //HSV color definition


    if (initialized == false) //initialize the brushes
    {
      initialized = true;
      pixelbrush.setSpeed(500); //brush moving speed
      pixelbrush.setFadeSpeed(90);
      pixelbrush.setFadein(false); //brightness will fade-in if set to true
      pixelbrush.setFadeout(true);
      pixelbrush.setBounce(false);
    }

    hue++;
    brushcolor.h = hue / 3; //divide by 3 to slow down color fading
    brushcolor.s = 255; //full saturation
    brushcolor.v = 255; //full brightness

    pixelbrush.setColor(brushcolor); //set new color to the bursh

    FastLED.clear();

    pixelbrush.paint(); //paint the brush to the canvas (and update the brush, i.e. move it a little)
    pixelcanvas.transfer(); //transfer (add) the canvas to the LEDs

    FastLED.show();
  }


  initialized = false; //reset the variable before moving to the next loop


  //SPARKLER: a brush seeding sparkles
  for (loopcounter = 0; loopcounter < duration; loopcounter++)
  {

    CHSV brushcolor;

    if (initialized == false)
    {
      initialized = true;
      pixelbrush.setSpeed(600);
      pixelbrush.setFadeout(true); //sparkles fade in
      pixelbrush.setFadein(true);  //and fade out immediately after reaching the set brightness
    }

    //set a new brush color in each loop
    brushcolor.h = random(255); //random color
    brushcolor.s = random(130); //random but low saturation, giving white-ish sparkles
    brushcolor.v = random(200); //random (peak) brighness

    pixelbrush.setColor(brushcolor);
    pixelbrush.setFadeSpeed(random(100) + 150); //set a new fadespeed with some randomness

    FastLED.clear();

    pixelbrush.paint(); //paint the brush to the canvas (and update the brush, i.e. move it a little)
    pixelcanvas.transfer(); //transfer (add) the canvas to the LEDs

    FastLED.show();
  }



  initialized = false; //reset the variable before moving to the next loop




  //---------------------
  //TWINKLY STARS
  //---------------------
  //brush set to random positions and painting a fading star
  for (loopcounter = 0; loopcounter < duration; loopcounter++)
  {

    CHSV brushcolor;

    if (initialized == false)
    {
      initialized = true;
      pixelbrush.setSpeed(0); //do not move automatically
      pixelbrush.setFadein(true); //fade in
      pixelbrush.setFadeout(true); //and fade out
    }


    if (rand() % 100 == 0) //at a random interval, move the brush to paint a new pixel (brush only paints a new pixel once)
    {
      brushcolor.h = rand();
      brushcolor.s = random(40); //set low saturation, almost white
      brushcolor.v = random(200) + 20; //set random brightness
      pixelbrush.setColor(brushcolor);
      pixelbrush.moveTo(random(NUMBEROFPIXELS)); //move the brush to a new, random pixel
      pixelbrush.setFadeSpeed(random(10) + 5); //set random fade speed, minimum of 5
    }

    //add a background color by setting all pixels to a color (instead of clearing all pixels):
    fill_solid(leds, NUMBEROFPIXELS, CRGB(1, 0, 6)); //color in RGB: dark blue


    pixelbrush.paint(); //paint the brush to the canvas
    pixelcanvas.transfer(); //transfer (add) the canvas to the LEDs

    FastLED.show();
  }


  initialized = false; //reset the variable before moving to the next loop


  //-------------
  //CHASER
  //-------------

  // two brushes chasing each other, one painting the pixel in a color, the other one painting 'black' (acting on the same canvas)

  while (true) //create a loop with an additional brush (is deleted automatically once the loop finishes)
  {

    //create an additional brush, painting on the same canvas as the globally defined brush
    FastLEDPainterBrush pixelbrush2 = FastLEDPainterBrush(&pixelcanvas);

    if (pixelbrush2.isvalid() == false) Serial.println(F("brush2 allocation problem"));
    else  Serial.println(F("brush2 allocation ok"));


    for (loopcounter = 0; loopcounter < duration; loopcounter++)
    {


      if (initialized == false)
      {
        CHSV brushcolor;

        initialized = true;

        brushcolor.h = random(255); //choose random color once
        brushcolor.s = 255; //full staturation
        brushcolor.v = 150;

        //initialize the first brush to move and paint a color, no fading
        pixelbrush.setSpeed(900); //moving speed
        pixelbrush.setColor(brushcolor);
        pixelbrush.setFadeout(false); //deactivate fade-out (was activated in last animation)
        pixelbrush.setFadein(false); //deactivate fade-in
        pixelbrush2.moveTo(0); //move the brush to pixel 0
        //initialize the second brush to move at the same speed but starting at a different position (default position is 0)
        brushcolor.v = 0; //zero intensity = black
        pixelbrush2.setSpeed(900);
        pixelbrush2.setColor(brushcolor);
        pixelbrush2.moveTo(2 * NUMBEROFPIXELS / 3); //move the brush

      }

      FastLED.clear();

      pixelbrush.paint(); //apply the paint of the first brush to the canvas (and update the brush)
      pixelbrush2.paint(); //apply the paint of the second brush to the canvas (and update the brush)
      pixelcanvas.transfer(); //transfer the canvas to the LEDs

      FastLED.show();
    }
    break; //quit the while loop immediately (and delete the created brush)
  }


  initialized = false;



  //------------------------------
  //HUE FADER: demo of hue fading
  //------------------------------

  //hue fading can be done in two ways: change the color moving the shortest distance around the colorwheel (setFadeHueNear)
  //or intentionally moving around the colorwheel choosing the long way (setFadeHueFar)
  //two brushes move along the strip in different speeds, each painting a different color that the canvas will then fade to
  //a new color is set when the first brush passes pixel 0
  //both brushes act on the same canvas

  while (true) //create a loop with an additional brush (is deleted automatically once the loop finishes)
  {

    //create an additional brush, painting on the same canvas as the globally defined brush
    FastLEDPainterBrush pixelbrush2 = FastLEDPainterBrush(&pixelcanvas);
    if (pixelbrush2.isvalid() == false) Serial.println(F("brush2 allocation problem"));
    else  Serial.println(F("brush2 allocation ok"));

    pixelcanvas.clear(); //clear the canvas

    for (loopcounter = 0; loopcounter < duration; loopcounter++)
    {

      static unsigned int lastposition = 0; //to detect zero crossing only once (brush may stay at pixel zero for some time since it uses sub-pixel resolution movement)

      if (pixelbrush.getPosition() == 0 && lastposition > 0) initialized = false; //choose new color & speed if brush reaches pixel 0

      lastposition = pixelbrush.getPosition(); //save current position for next position check

      if (initialized == false)
      {
        initialized = true;

        CHSV brushcolor;


        brushcolor.h = random(255); //random color
        brushcolor.s = 255; //full saturation
        brushcolor.v = 130; //medium brightness

        pixelbrush.setSpeed(random(150) + 150); //random movement speed
        pixelbrush.setFadeSpeed(random(10) + 20); //set random fading speed
        pixelbrush.setColor(brushcolor); //update the color of the brush
        pixelbrush.setFadeHueNear(true); //fade using the near path on the colorcircle

        //second brush paints on the same canvas
        brushcolor.h = random(255);
        pixelbrush2.setSpeed(random(150) + 150);
        pixelbrush2.setFadeSpeed(random(10) + 20);
        pixelbrush2.setColor(brushcolor);
        pixelbrush2.setFadeHueNear(true); //fade using the near path on the colorcircle
        //pixelbrush.setFadeHueFar(true); //fade using the far path on the colorcircle (if both are set, this path is chosen)
        pixelbrush2.setBounce(true); //bounce this brush at the end of the strip
      }

      FastLED.clear();

      pixelbrush.paint(); //apply the paint of the first brush to the canvas (and update the brush)
      pixelbrush2.paint(); //apply the paint of the second brush to the canvas (and update the brush)
      pixelcanvas.transfer(); //transfer the canvas to the LEDs

      FastLED.show();
    }
    break; //quit the while loop immediately (and delete the created brush)
  }


  initialized = false;

  //------------------------------
  //SPEEDTRAILS
  //------------------------------
  //three brushes painting on one canvas, all following each other at the same speed, painting fading pixels

  while (true) //create a loop with two additional brushes (are deleted automatically once the loop finishes)
  {
    int brushspeed = 900;

    //create additional brushes, painting on the same canvas as the globally defined brush
    FastLEDPainterBrush pixelbrush2 = FastLEDPainterBrush(&pixelcanvas);
    FastLEDPainterBrush pixelbrush3 = FastLEDPainterBrush(&pixelcanvas);

    if (pixelbrush2.isvalid() == false) Serial.println(F("brush2 allocation problem"));
    else  Serial.println(F("brush2 allocation ok"));

    if (pixelbrush3.isvalid() == false) Serial.println(F("brush3 allocation problem"));
    else  Serial.println(F("brush3 allocation ok"));

    for (loopcounter = 0; loopcounter < duration; loopcounter++)
    {

      CHSV brushcolor;


      if (initialized == false) //initialize the brushes
      {
        initialized = true;
        brushcolor.h = 0;
        brushcolor.s = 0; //make it white
        brushcolor.v = 150; //medium brightness
        pixelbrush.setColor(brushcolor);
        pixelbrush.setSpeed(brushspeed);
        pixelbrush.setFadeSpeed(250); //fast fading (255 is max.)
        pixelbrush.setFadeHueNear(false); //deactivate hue fading, was activated in last animation
        pixelbrush.setFadeout(true);
        pixelbrush.moveTo(0); //move brush to zero

        //second brush
        brushcolor.h = 0; //red
        brushcolor.s = 250;
        brushcolor.v = 150;
        pixelbrush2.setSpeed(brushspeed);
        pixelbrush2.setFadeSpeed(220);
        pixelbrush2.setFadeout(true);
        pixelbrush2.setColor(brushcolor);
        pixelbrush2.moveTo(NUMBEROFPIXELS / 3); //move it up one third of the strip

        //third brush
        brushcolor.h = 28; //yellow
        brushcolor.s = 255;
        brushcolor.v = 100;
        pixelbrush3.setSpeed(brushspeed);
        pixelbrush3.setFadeSpeed(190);
        pixelbrush3.setFadeout(true);
        pixelbrush3.setColor(brushcolor);
        pixelbrush3.moveTo(2 * NUMBEROFPIXELS / 3); //move it up two thirds of the strip
      }

      FastLED.clear();

      pixelbrush.paint(); //apply the paint of the first brush to the canvas (and update the brush)
      pixelbrush2.paint(); //apply the paint of the second brush to the canvas (and update the brush)
      pixelbrush3.paint(); //apply the paint of the third brush to the canvas (and update the brush)
      pixelcanvas.transfer(); //transfer the canvas to the LEDs (and update i.e. fade pixels)

      FastLED.show();
    }
    break; //quit the while loop immediately (and delete the created brush)
  }


  initialized = false;



  //-------------
  //BOUNCY BALLS
  //-------------
  //three brushes painting on one canvas, attracted to the zero pixel as if by gravity

  while (true) //create a loop with two additional brushes (are deleted automatically once the loop finishes)
  {

    //create additional brushes, painting on the same canvas as the globally defined brush
    FastLEDPainterBrush pixelbrush2 = FastLEDPainterBrush(&pixelcanvas);
    FastLEDPainterBrush pixelbrush3 = FastLEDPainterBrush(&pixelcanvas);

    if (pixelbrush2.isvalid() == false) Serial.println(F("brush2 allocation problem"));
    else  Serial.println(F("brush2 allocation ok"));

    if (pixelbrush3.isvalid() == false) Serial.println(F("brush3 allocation problem"));
    else  Serial.println(F("brush3 allocation ok"));

    byte skipper = 0;

    for (loopcounter = 0; loopcounter < duration; loopcounter++)
    {


      if (initialized == false) //initialize the brushes
      {
        initialized = true;

        CHSV brushcolor;

        brushcolor.h = 20; //orange
        brushcolor.s = 240; //almost full saturation
        brushcolor.v = 150; //medium brightness

        //first brush
        pixelbrush.setSpeed(0); //zero initial speed
        pixelbrush.setFadeSpeed(150);
        pixelbrush.setFadeout(true);
        pixelbrush.setColor(brushcolor);
        pixelbrush.moveTo(NUMBEROFPIXELS - 1); //move to end of the strip
        pixelbrush.setBounce(true); //bounce if either end of the strip is reached

        //second brush
        brushcolor.h = 220; //pink
        pixelbrush2.setSpeed(0); //zero initial speed
        pixelbrush2.setFadeSpeed(190);
        pixelbrush2.setFadeout(true);
        pixelbrush2.setColor(brushcolor);
        pixelbrush2.moveTo(NUMBEROFPIXELS / 3); //move to one third of the strip
        pixelbrush2.setBounce(true);

        brushcolor.h = 70; //green-ish (pure green is 85 or 255/3)
        pixelbrush3.setSpeed(0);
        pixelbrush3.setFadeSpeed(220);
        pixelbrush3.setFadeout(true);
        pixelbrush3.setColor(brushcolor);
        pixelbrush3.moveTo(2 * NUMBEROFPIXELS / 3);
        pixelbrush3.setBounce(true);
      }

      //apply some gravity force that accelerates the painters (i.e. add speed in negative direction = towards zero pixel)
      //  if (skipper % 5 == 0) //only apply gravity at some interval to make it slower on fast processors
      //  {
      //read current speed of each brush and speed it up in negative direction (towards pixel zero)
      pixelbrush.setSpeed(pixelbrush.getSpeed() - 10);
      pixelbrush2.setSpeed(pixelbrush2.getSpeed() - 10);
      pixelbrush3.setSpeed(pixelbrush3.getSpeed() - 10);
      //  }
      //  skipper++;


      FastLED.clear();

      pixelbrush.paint(); //apply the paint of the first brush to the canvas (and update the brush)
      pixelbrush2.paint(); //apply the paint of the second brush to the canvas (and update the brush)
      pixelbrush3.paint(); //apply the paint of the third brush to the canvas (and update the brush)
      pixelcanvas.transfer(); //transfer the canvas to the LEDs (and update i.e. fade pixels)

      FastLED.show();
    }
    break; //quit the while loop immediately (and delete the created brush)
  }


  initialized = false;

  //---------------------
  //TWO-BRUSH-COLORMIXING
  //---------------------

  //two brushes moving around randomly paint on their individual canvas, resulting in colors being mixed
  while (true) //create a loop with two additional brushes (are deleted automatically once the loop finishes)
  {

    //create an additional canvas for the second brush (needs to be created before the brush)
    FastLEDPainterCanvas pixelcanvas2 = FastLEDPainterCanvas(NUMBEROFPIXELS);

    //create additional brush, painting on the second canvas
    FastLEDPainterBrush pixelbrush2 = FastLEDPainterBrush(&pixelcanvas2);


    if (pixelcanvas2.isvalid() == false) Serial.println("canvas2 allocation problem");
    else  Serial.println("canvas2 allocation ok");

    if (pixelbrush2.isvalid() == false) Serial.println(F("brush2 allocation problem"));
    else  Serial.println(F("brush2 allocation ok"));


    for (loopcounter = 0; loopcounter < duration; loopcounter++)
    {


      CHSV brushcolor;
      static bool firstrun = true;

      brushcolor.s = 255; //full color saturation
      brushcolor.v = 100; //medium-low brightness

      if (initialized == false) //initialize the brushes
      {
        initialized = true;

        brushcolor.h = 8;

        //setup the first brush
        pixelbrush.setSpeed(-750);
        pixelbrush.setSpeedlimit(1000);
        pixelbrush.setFadeSpeed(random(80) + 50);
        pixelbrush.setFadeout(true);
        pixelbrush.setFadein(true);
        pixelbrush.setColor(brushcolor);
        pixelbrush.moveTo(random(NUMBEROFPIXELS));
        pixelbrush.setBounce(true);

        //setup the second brush
        brushcolor.h = 160;
        pixelbrush2.setSpeed(600);
        pixelbrush2.setSpeedlimit(1000);
        pixelbrush2.setFadeSpeed(random(80) + 50);
        pixelbrush2.setFadeout(true);
        pixelbrush2.setFadein(true);
        pixelbrush2.setColor(brushcolor);
        pixelbrush2.moveTo(random(NUMBEROFPIXELS));
        pixelbrush2.setBounce(true);
      }


      if (rand() % 10) //slowly (and randomly) change hue of brushes
      {
        brushcolor = pixelbrush.getColor();
        brushcolor.h += random(3) - 1; //randomly change hue a little ( ± random(1))
        pixelbrush.setColor(brushcolor);

        brushcolor = pixelbrush2.getColor();
        brushcolor.h += random(3) - 1; //randomly change hue a little ( ± random(1))
        pixelbrush2.setColor(brushcolor);

      }

      //slowly change speed of both brushes
      pixelbrush.setSpeed(pixelbrush.getSpeed() + random(6) - 3); //means speed = currentspeed ± random(3)
      pixelbrush2.setSpeed(pixelbrush2.getSpeed() + random(6) - 3); //means speed = currentspeed ± random(3)


      FastLED.clear(); //remove any previously applied paint

      pixelbrush.paint(); //apply the paint of the first brush to its assigned canvas (and update the brush)
      pixelbrush2.paint(); //apply the paint of the second brush to  its assigned canvas (and update the brush)

      pixelcanvas.transfer(); //transfer the first canvas to the LEDs
      pixelcanvas2.transfer(); //transfer the sedonc canvas to the LEDs (adding colors, rather than overwriting colors)

      FastLED.show();
    }
    break; //quit the while loop immediately (and delete the created brush)
  }

}//end of loop()
