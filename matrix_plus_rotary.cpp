// Required libraries
// NeoMatrix and its dependencies
// https://github.com/adafruit/Adafruit_NeoMatrix
// https://github.com/adafruit/Adafruit_NeoPixel
// https://github.com/adafruit/Adafruit-GFX-Library
// RotaryEncoder library
// https://github.com/mathertel/RotaryEncoder

#include <RotaryEncoder.h>

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

RotaryEncoder encoder(2, 3);
 
void it_handler() {
	  encoder.tick();
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

#define DEFAULT_BRIGHTNESS 60
#define NB_MODES 4
#define DEFAULT_MODE 0

int scroll_text_x;
uint16_t bottom_line_color;
int last_mode;
long oldPosition;


void setup() {
  Serial.begin(9600);
  Serial.println("LED table test:");
  matrix.begin();

  // matrix init
  matrix.setTextWrap(false);
  matrix.setBrightness(DEFAULT_BRIGHTNESS);
  matrix.setTextColor(rand_color());

  // rotary init
  attachInterrupt(0, it_handler, CHANGE);
  attachInterrupt(1, it_handler, CHANGE);

  scroll_text_x = matrix.width();
  bottom_line_color = rand_color();
  last_mode = DEFAULT_MODE;
  oldPosition = encoder.getPosition();
}

char *index_str[4] = { "0", "1", "2", "3" };

#define INC_MOD(a, inc, mod) ( ( ( a + inc ) == mod ) ?  0 : a + inc )
#define DEC_MOD(a, dec, mod) ( ( ( a - dec ) < 0 ) ?  mod : a - dec )

// Scroll text mode
void scroll_text(const char *text) {
  matrix.clear();
  matrix.setCursor(scroll_text_x, 3);
  matrix.print(text);
  if (--scroll_text_x < -36) {
    scroll_text_x = matrix.width();
    matrix.setTextColor(rand_color());
  }
  matrix.show();
  delay(100);
}

#define NB_COLORS_HYPNOSE (W/2)
uint16_t hypnose_colors[NB_COLORS_HYPNOSE] = {0};
uint16_t hypnose_loop = 0;
uint8_t  hypnose_index = 0;

int hypnose_brightness = 60;
int hypnose_brightness_inc = -5;
void hypnose() {
  hypnose_index = DEC_MOD(hypnose_index, 1, NB_COLORS_HYPNOSE);
  hypnose_colors[hypnose_index] = rand_color();
  for ( int i = 1, index = hypnose_index; i <= NB_COLORS_HYPNOSE; ++i, index = ( index + 1 ) % NB_COLORS_HYPNOSE ) {
    uint16_t color = hypnose_colors[index];
    matrix.drawRect(W / 2 - i, H / 2 - i, i * 2, i * 2, color);
  }
  if ( hypnose_brightness >= 60 )
    hypnose_brightness_inc = -5;
  else if ( hypnose_brightness <= 20 )
    hypnose_brightness_inc = 5;
  hypnose_brightness += hypnose_brightness_inc;
  matrix.setBrightness(hypnose_brightness);
  matrix.show();
  delay(100);
}

class Sprite {
  public:
    int8_t m_x0, m_x1, m_y0, m_y1;
    uint16_t m_color;
    bool m_enable;
  public:
    Sprite() {
      m_enable = false;
    }
    void set(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint16_t color) {
      m_x0 = x0;
      m_x1 = x1;
      m_y0 = y0;
      m_y1 = y1;
      m_color = color;
    }
    void move(int8_t x, int8_t y) {
      m_x0 += x;
      m_x1 += x;
      m_y0 += y;
      m_y1 += y;
    }
    void enable() {
      m_enable = true;
    }
    void disable() {
      m_enable = false;
    }
    bool is_enabled() {
      return m_enable;
    }
    int8_t x0() {
      return m_x0;
    };
    int8_t x1() {
      return m_x1;
    };
    int8_t y0() {
      return m_y0;
    };
    int8_t y1() {
      return m_y1;
    };
    uint16_t color() {
      return m_color;
    };
    virtual void draw() {}
};

class Point : public Sprite {
  public:
    void draw() {
      matrix.drawPixel(m_x0, m_y0, m_color);
    }
};

class Line : public Sprite {
  public:
    void draw() {
      matrix.drawLine(m_x0, m_y0, m_x1, m_y1, m_color);
    }
};

#define NB_DROPS 4
Line drops[NB_DROPS];

void rain() {
  matrix.clear();

  matrix.drawFastHLine(0, H - 1, W, bottom_line_color);

  for ( int i = 0; i < NB_DROPS; ++i ) {
    if ( !drops[i].is_enabled() && random(10) > 5 ) {
      int x = random(W);
      drops[i].set(x, -3, x, -1, rand_color());
      drops[i].enable();
    }
    drops[i].move(0, 1);

    if ( drops[i].y0() == 11 ) {
      drops[i].disable();
      bottom_line_color = drops[i].color();
    }
    if ( drops[i].is_enabled() )
      drops[i].draw();
  }

  matrix.show();
  delay(100);
}

Line racket[2];
Point ball;
int8_t ball_dir_w = 1, ball_dir_h = 1;

void pong() {
  if ( !ball.is_enabled() ) {
    ball.set(0, 6, 0, 0, matrix.Color(255, 255, 255));
    ball.enable();
  }
  if ( !racket[0].is_enabled() ) {
    racket[0].set(0, 5, 0, 7, matrix.Color(255, 0, 0));
    racket[0].enable();
  }
  if ( !racket[1].is_enabled() ) {
    racket[1].set(11, 5, 11, 7, matrix.Color(0, 255, 0));
    racket[1].enable();
  }

  if ( ball.x0() <= 1 )
    ball_dir_w = 1;
  else if ( ball.x0() >= W - 2 )
    ball_dir_w = -1;

  if ( ball.y0() <= 0 )
    ball_dir_h = 1;
  else if ( ball.y0() >= H - 1 )
    ball_dir_h = -1;

  ball.move(ball_dir_w, ball_dir_h);

  Line *racket_move;
  if ( ball_dir_w == 1 )
    racket_move = &racket[1];
  else
    racket_move = &racket[0];

  if ( racket_move->y0() + 1 < ball.y0() )
    racket_move->move(0, 1);
  else if ( racket_move->y0() + 1 > ball.y0() )
    racket_move->move(0, -1);
    
  matrix.clear();
  ball.draw();
  racket[0].draw();
  racket[1].draw();
  matrix.show();
  delay(100);
}

int getMode() {
  bool anim = false;
  int x = 0;
  int splash = 0;

  do {

    long newPosition = encoder.getPosition();

    if (newPosition != oldPosition) {
      if (newPosition > oldPosition) {
        last_mode = (last_mode == 0) ? NB_MODES - 1 : last_mode - 1;
      }
      else {
        last_mode = (last_mode + 1) % NB_MODES;
      }
      x = -5;
      splash = 0;
      anim = true;
      matrix.setBrightness(DEFAULT_BRIGHTNESS);
      oldPosition = newPosition;
      Serial.println(last_mode);
    }
    if ( anim == false ) {
      return last_mode;
    }

    // Splash screen when chaging mode
    matrix.clear();
    matrix.setCursor(x, 3);
    matrix.print(index_str[last_mode]);
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
  return last_mode;
}


void loop() {

  int mode = getMode();
  // Call the function accoring to mode.
  switch (mode) {
    case 0:
      rain();
      break;
    case 1:
      hypnose();
      break;
    case 2:
      pong();
      break;
    case 3:
      scroll_text("POULET!");
      break;
  }
}
