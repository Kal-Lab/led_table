// Required libraries
// NeoMatrix and its dependencies
// https://github.com/adafruit/Adafruit_NeoMatrix
// https://github.com/adafruit/Adafruit_NeoPixel
// https://github.com/adafruit/Adafruit-GFX-Library
// RotaryEncoder library
// http://www.pjrc.com/teensy/td_libs_Encoder.html

#include <Encoder.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>


#define MATRIX_PIN 6

#define H 12
#define W 12

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(H, W, MATRIX_PIN,
                            NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                            NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB            + NEO_KHZ800);

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(2, 3);

void setup() {
  Serial.begin(9600);
  Serial.println("LED table test:");
  matrix.begin();

  // matrix init
  matrix.setTextWrap(false);
  matrix.setBrightness(60);
  matrix.setTextColor(rand_color());
}

// return a random color
uint16_t rand_color() {
  uint8_t neutral_color[3] = { 255, 255, 255 };
  int red = random(256);
  int green = random(256);
  int blue = random(256);

  // mix the color
  red = (red + neutral_color[0]) / 2;
  green = (green + neutral_color[1]) / 2;
  blue = (blue + neutral_color[2]) / 2;

  return matrix.Color(red, green, blue);
}

char *index_str[4] = { "0", "1", "2", "3" };

// Scroll text mode
void scroll_text(const char *text) {
  static int x    = matrix.width();
  matrix.fillScreen(0);
  matrix.setCursor(x, 3);
  matrix.print(text);
  if (--x < -36) {
    x = matrix.width();
    matrix.setTextColor(rand_color());
  }
  matrix.show();
  delay(100);
}


void rain() {
#define NB_DROPS 4

  typedef struct {
    int8_t x0, x1, y0, y1;
    uint16_t color;
  } rain_drop_t;

  static rain_drop_t drops[NB_DROPS] = { { -1 }, { -1 }, { -1 }, { -1 } };

  static uint16_t bottom_line_color = rand_color();
  matrix.fillScreen(0);
  for ( int i = 0; i < NB_DROPS; ++i ) {
    if ( drops[i].x0 < 0 && random(10) > 5 ) {
      drops[i].y0 = -3;
      drops[i].y1 = -1;
      int x = random(W);
      drops[i].x0 = x;
      drops[i].x1 = x;
      drops[i].color = rand_color();
    }
    drops[i].y0++;
    drops[i].y1++;

    if ( drops[i].y1 == 11 ) {
      drops[i].x0 = -1;
      bottom_line_color = drops[i].color;
    }

    if ( drops[i].x0 >= 0 ) {
      matrix.drawLine(drops[i].x0, drops[i].y0, drops[i].x1, drops[i].y1, drops[i].color);
    }
  }
  matrix.drawFastHLine(0, H - 1, W, bottom_line_color);
  matrix.show();
  delay(100);
}

#define NB_MODES 4
#define DEFAULT_MODE 0

int getMode() {
  static int index = DEFAULT_MODE;
  static long oldPosition = myEnc.read();
  bool anim = false;
  int x = 0;
  int splash = 0;

  do {

    long newPosition = myEnc.read();

    if (newPosition != oldPosition) {
      if (newPosition > oldPosition) {
        index = (index == 0) ? NB_MODES - 1 : index - 1;
      }
      else {
        index = (index + 1) % NB_MODES;
      }
      x = -5;
      splash = 0;
      anim = true;
      oldPosition = newPosition;
      Serial.println(index);
    }
    if ( anim == false ) {
      return index;
    }

    // Splash screen when chaging mode
    matrix.fillScreen(0);
    matrix.setCursor(x, 3);
    matrix.print(index_str[index]);
    matrix.show();
    if ( x < 4 ) {
      x++;
    }
    else {
      if ( ++splash >= 5 ) {
        anim = false;
      }
    }
    delay(100);
  } while ( anim );
  return index;
}


void loop() {

  int mode = getMode();
  // Call the function accoring to mode.
  if ( mode == 0 || mode == 1 ) {
    rain();
  }
  else if ( mode == 2 ) {
    scroll_text("Poulet!");
  }
  else {
    scroll_text("ERROR!");
  }
}
