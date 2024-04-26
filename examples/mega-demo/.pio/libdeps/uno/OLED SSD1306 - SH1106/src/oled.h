#ifndef OLED_H
#define OLED_H

#include <stdint.h>
#include "Print.h"

// You may change the communication speed here
//#define OLED_I2C_DELAY delayMicroseconds(3) //  68kHz@80Mhz or  83kHz@160Mhz
//#define OLED_I2C_DELAY delayMicroseconds(2) //  85kHz@80Mhz or 110kHz@160Mhz
  #define OLED_I2C_DELAY delayMicroseconds(1) // 110kHz@80Mhz or 170kHz@160Mhz
//#define OLED_I2C_DELAY delayMicroseconds(0) // 150kHz@80Mhz or 300kHz@160Mhz
//#define OLED_I2C_DELAY                      // 250kHz@80Mhz or 500kHz@160Mhz

/**
 * Font pixel size
 */
#define OLED_FONT_HEIGHT 8
#define OLED_FONT_WIDTH 6

/**
 * tty mode default
 */
#define OLED_DEFAULT_TTY_MODE false;

/**
 * Use as pin number if the reset line is not connected to an I/O pin. 
 * In this case, the reset signal must be generated by some other hardware.
 */
#define NO_RESET_PIN 255

/**
 * Driver for OLED displays with SSD1306 or SH1106 controller with write(), print(), println(), printf() support.
 * 
 * Supported display sizes: 196x16, 28x32 and 128x64 pixels.
 * This driver supports only displays with internal charge pump and I2C interface.
 * The I2C communication is done by software bit-banging the configurable I/O pins.
 * Communication errors are not handled.
 * 
 * The driver allocates 192, 512 or 1024 bytes memory on the heap for buffering.
 * 
 * The display is technically organized into "pages" which is a set of 8 horizontal lines.
 * For a 128x34 display the memory contains 4 pages each of 128 bytes. 
 * Each byte represents 8 pixels. Bit 0 is the top pixel, bit 7 is the bottom pixel.
 * 
 * Bitmap graphics have the same format with the exception that they can have any width. The
 * height is always a multiple of 8.
 * 
 * Text and bitmaps can be drawn to any position, but vertical positions at the
 * page boundaries (y=0, 8, 16, 24, 32, 40, 48, 56) provide better performance.
 * 
 * For all drawing functions, the coordinate 0,0 is the top left corner of the display.
 * After drawing into the buffer, you have to call the display() method to send the data to 
 * the display controller. Otherwise you won't see anything.
 * 
 * This driver is distributed without license. You may use it for free.
 * Author: Stefan Frings, 2017
 *
 * Update by:
 * Author: Fabio Durigon, 2018
 * email: develop@dury.it
 *
 */
class OLED : public Print
{
public:
    
    /** Possible colors for drawing */
    enum tColor { BLACK, WHITE };
    
    /** Filling mode */
    enum tFillmode { HOLLOW, SOLID };
    
    /** Supported text sizes. Normal=6x8 pixels, Double=12x16 pixels */
    enum tFontScaling { NORMAL_SIZE, DOUBLE_SIZE };
    
    /** Scroll effects supported by the display controller, note that there is no plain vertical scrolling */
    enum tScrollEffect { NO_SCROLLING=0, HORIZONTAL_RIGHT=0x26, HORIZONTAL_LEFT=0x27, DIAGONAL_RIGHT=0x29, DIAGONAL_LEFT=0x2A };

    /** Possible display width values **/
    enum tWidth { W_96=96, W_128=128 };

    /** Possible display height values **/
    enum tHeight { H_16=16, H_32=32, H_64=64 };

    /** Possible display controller **/
    enum tDisplayCtrl { CTRL_SSD1306, CTRL_SH1106 };
    
    /** 
     * Constructor of the OLED class (!! DEPRECATED !!).
     * @param sda_pin Pin number of the SDA line (can be any Arduino I/O pin)
     * @param sda_pin Pin number of the SCL line (can be any Arduino I/O pin)
     * @param reset_pin Pin number for the /RST line, or use NO_RESET_PIN if the reset signal is generated somewhere else
     * @param i2c_address The I²C address is usually 0x3c or 0x3D
     * @param width With of the display in pixels: 96 or 128
     * @param height Height of the display in pixels: 16, 32 or 64
     * @param isSH1106 Must be true=SH1106 chip, false=SSD1306 chip
     */
    __attribute__((deprecated("Deprecated constructor")))
    OLED(uint8_t sda_pin, uint8_t scl_pin, uint8_t reset_pin, uint8_t i2c_address, uint_fast8_t width, uint_fast8_t height, bool isSH1106=false);

    /**
     * @brief Construct a new OLED object
     * 
     * @param sda_pin Pin number of the SDA line (can be any Arduino I/O pin).
     * @param sda_pin Pin number of the SCL line (can be any Arduino I/O pin).
     * @param reset_pin Pin number for the /RST line, or use NO_RESET_PIN if the reset signal is generated somewhere else.
     * @param width Width of the display in pixels: can be W_96 or W_128 (for 96 or 128 pixel width).
     * @param height Height of the display in pixels: can be H_16, H_32 or H_64 (for 16,32 or 64 pixel height).
     * @param dispayCtrl Display controller chip: can be CTRL_SH1106 or CTRL_SSD1306.
     * @param i2c_address The I²C address is usually 0x3c or 0x3D.
     */
    OLED(uint8_t sda_pin, uint8_t scl_pin, uint8_t reset_pin=NO_RESET_PIN, tWidth width=W_128, tHeight height=H_32, tDisplayCtrl displayCtrl=CTRL_SSD1306, uint8_t i2c_address=0x3C);
    
    /**
     * Destructor. Frees the buffer memory and leaves the display in the previous state.
     */
    virtual ~OLED();    
    
    /**
     * Initialize the display controller, clean memory and switch output on.
     */
    void begin();

    /**
     * Will use offset for wired cases when controller uses SSH1106 132x64 but display is 128x64
     */
    void useOffset(bool enabled=true);
    
    /**
     * Enable or disable the charge pump and display output. May be used to save power.
     * This command is executed by the display controller itself, hence it does not affect the buffer memory.
     * @param enable Whether to enable the display output.
     */
    void set_power(bool enable);
       
    /**
     * Enable display inverting. If enabled, then WHITE and BLACK are swapped.
     * This command is executed by the display controller itself, so it works very fast and does not affect
     * the buffer memory.
     * @param enable Whether to enable inverse output
     */
    void set_invert(bool enable);
    
    /**
     * Set the contrast (brightness) of the display.
     * The value affects the electrical current throught the OLED segments in linear ratio.
     * But the eye reacts logarithmically. A medium brightnes of 128 looks almost as bright as the
     * maximum setting but increases the lifetime of the display a lot.
     * @param contrast Contrast value, default is 0x80.
     */
    void set_contrast(uint8_t contrast);
    
    /**
     * Enable continuous scrolling. This is performed by the display controller itself without needing continuous 
     * communication and it does not affect the buffer memory.
     * For horizontal scrolling, you can decide whether the whole display or only a part of it shall scroll.
     * In case of diagonal scrolling, the vertical part affects always the whole display.
     * @param scroll_type Select the scroll effect, or NO_SCROLLING to disable it
     * @param first_page Defined which pages are affected by the horizontal scrolling (0-7). Each page contains 8 lines
     * @param last_page Defined which pages are affected by the horizontal scrolling (0-7). Each page contains 8 lines
     */
    void set_scrolling(tScrollEffect scroll_type, uint_fast8_t first_page=0, uint_fast8_t last_page=7);
       
    /**
     * Scroll the display up, which leaves a number of black pixel lines at the bottom.
     * An optional delay time can be used to produce a smooth scrolling effect.
     * This is done by software, requiring continuous communication.
     * @param num_lines Number of pixel lines to scroll the display content up.
     * @param delay_ms Delay time between each step, recommended values are either 0 or 20..60.
     */
    void scroll_up(uint_fast8_t num_lines=OLED_FONT_HEIGHT, uint_fast8_t delay_ms=0);
    
    /**
     * Transfer the buffer memory data to the display controller.
     * You have to call this method after any drawing action into the buffer to make the change visible.
     * A call to this method takes several milliseconds.
     */
    void display();    
    
    /**
     * Fill the whole buffer memory with white or black color.
     * @param color The color to be used
     */
    void clear(tColor color=BLACK);
       
    /**
     * Draw a bitmap from RAM. The raw data format is explained in the description of this class.
     * @param x Pixel position of the upper left corner
     * @param y Pixel position of the upper left corner
     * @param width Width of the bitmap in pixels
     * @param height Height of the bitmap in pixels. Must be a multiple of 8
     * @param data Raw data, number of bytes must be width*height/8
     * @param color Color to draw with
     */
    void draw_bitmap(uint_fast8_t x, uint_fast8_t y, uint_fast8_t width, uint_fast8_t height, const uint8_t* data, tColor color=WHITE);
    
    /**
     * Draw a bitmap from program memory (aka FLASH). The raw data format is explained in the description of this class.
     * @param x Pixel position of the upper left corner
     * @param y Pixel position of the upper left corner
     * @param width Width of the bitmap in pixels
     * @param height Height of the bitmap in pixels. Must be a multiple of 8
     * @param data Raw data, number of bytes must be width*height/8
     * @param color Color to draw with
     */
    void draw_bitmap_P(uint_fast8_t x, uint_fast8_t y, uint_fast8_t width, uint_fast8_t height, const uint8_t* data, tColor color=WHITE);
    
    /**
     * Draw a character.
     * @param x Pixel position of the upper left corner
     * @param y Pixel position of the upper left corner
     * @param c The character code. Supports US-ASCII characters and german umlauts. See source code of oled.cpp
     * @param scaling Scaling factor. Can be used to double the size of the output. The normal font size is 6x8
     * @param color Color to draw with
     */
    size_t draw_character(uint_fast8_t x, uint_fast8_t y, char c, tFontScaling scaling=NORMAL_SIZE, tColor color=WHITE);
    
    /**
     * Draw a C string from RAM, which is a NULL terminated array of characters.
     * @param x Pixel position of the upper left corner
     * @param y Pixel position of the upper left corner
     * @param s The string to draw. Supports US-ASCII characters and german umlauts. See source code of oled.cpp
     * @param scaling Scaling factor. Can be used to double the size of the output. The normal font size is 6x8
     * @param color Color to draw with
     */
    void draw_string(uint_fast8_t x, uint_fast8_t y, const char* s, tFontScaling scaling=NORMAL_SIZE, tColor color=WHITE);

    /**
     * Draw a C string from program memory (aka FLASH), which is a NULL terminated array of characters.
     * @param x Pixel position of the upper left corner
     * @param y Pixel position of the upper left corner
     * @param s The string to draw. Supports US-ASCII characters and german umlauts. See source code of oled.cpp
     * @param scaling Scaling factor. Can be used to double the size of the output. The normal font size is 6x8
     * @param color Color to draw with
     */
    void draw_string_P(uint_fast8_t x, uint_fast8_t y, const char* s, tFontScaling scaling=NORMAL_SIZE, tColor color=WHITE);      
   
    /**
     * Draw a single pixel.
     * @param x,y Position of the pixel
     * @param color Color to draw with
     */
    void draw_pixel(uint_fast8_t x, uint_fast8_t y, tColor color=WHITE);    
    
    /**
     * Draw a line.
     * @param x0,y0 Start point of the line
     * @param x1,y1 End point of the line
     * @param color Color to draw with
     */
    void draw_line(uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t x1, uint_fast8_t y1, tColor color=WHITE);
    
    /**
     * Draw a circle.
     * @param x,y Center position of the circle
     * @param radius Radius of the circle
     * @param fillMode Whether the circle is filled with the color
     * @param color Color to draw with     
     */
    void draw_circle(uint_fast8_t x, uint_fast8_t y, uint_fast8_t radius, tFillmode fillMode=HOLLOW, tColor color=WHITE);
    
    /**
     * Draw a rectangle.
     * @param x0,y0 Upper left corner
     * @param x1,y1 Lower right corner
     * @param fillMode Whether the rectangle is filled with the color
     * @param color Color to draw with
     */
    void draw_rectangle(uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t x1, uint_fast8_t y1, tFillmode fillMode=HOLLOW, tColor color=WHITE);

    /**
     * Write a character at current X,Y coordinates.
     * X coordinate will be moved by OLED_FONT_WIDTH value
     * * If the string contains a \r or \n and ttyMode is true, Y coordinate will be incremented
	 * by OLED_FONT_HEIGHT value
     * This function overrides write(...) in Print.cpp of Arduino core
     * @param c character to write
     * @return 1 on succeed, otherwise 0.
     */
    size_t write(uint8_t c) override;

    /**
     * These functions are copied from HardwareSeral.h of Arduino core.
     * I implemented them to be fully compatible witgh Serial calls
     */
    inline size_t write(unsigned long n)
	{
		return write((uint8_t) n);
	}
	inline size_t write(long n)
	{
		return write((uint8_t) n);
	}
	inline size_t write(unsigned int n)
	{
		return write((uint8_t) n);
	}
	inline size_t write(int n)
	{
		return write((uint8_t) n);
	}

	/**
	 * Write a sting at current X,Y coordinates.
	 * X coordinate will be moved by (size*OLED_FONT_WIDTH) pixels.
	 * If ttyMode is true display() will be called to redraw screen.
     * This function overrides write(...) in Print.cpp of Arduino core
     * @param buffer The string to be printed
     * @param size The length of the string
     * @return Nr of characters written.
	 */
	size_t write(const uint8_t *buffer, size_t len) override;

	/** Use also Print::write(const uint8_t *buffer, size_t size)*/
    using Print::write;

    /**
     * Set cursor position
     */
    void setCursor(uint_fast8_t x, uint_fast8_t y);

    /**
     * @brief Std printf implementation.
     * 
     * @param format Format string same in printf
     * @param ... Args for formatted string
     * @return Len of written string
     */
    size_t printf(const char *format, ...);

    /**
     * @brief !! DEPRECATED!! Same as printf but prints at x,y coordinates.
     * 
     * @param x Pixel position of the upper left corner.
     * @param y Pixel position of the upper left corner.
     * @param format Format string same in printf.
     * @param ... Args for formatted string.
     * @return Len of written string.
     */
    __attribute__((deprecated("Use printf_XY() instead")))
    size_t printf(uint_fast8_t x, uint_fast8_t y, const char *format, ...);

    /**
     * @brief Same as printf but prints at x,y coordinates.
     * 
     * @param x Pixel position of the upper left corner.
     * @param y Pixel position of the upper left corner.
     * @param format Format string same in printf.
     * @param ... Args for formatted string.
     * @return Len of written string.
     */
    size_t printf_XY(uint_fast8_t x, uint_fast8_t y, const char *format, ...);
    
    /**
     * @brief Same as printf but prints at column,row position.
     * 
     * @param col column position where start to write (zero based).
     * @param row row position where start to write (zero based).
     * @param format Format string same in printf.
     * @param ... Args for formatted string.
     * @return Len of written string.
     */
    size_t printf_T(uint_fast8_t col, uint_fast8_t row, const char *format, ...);

    /**
     * Set terminal mode on/off
     * @param Enabled If true, driver automatically scroll-up to one text line (height of font) when
     * a printing function (that doesn't use coordinate) reach the bottom of the screen.
     * Es.
     * When the cursor Y position is at the last text line and you use println() or writeln(),
     * the text on the the screen scrolls up and first line of text disappears
     */
    void setTTYMode(bool enabled);

    // ************************ U8g2 wrappers ************************

     /**
     * @brief Draw a C, NULL terminated string from RAM, using column and row positioning.
     * Wrapper for U8g2 library API compatibility.
     * 
     * @param col column position where start to write (zero based).
     * @param row row position where start to write (zero based).
     * @param s The string to draw. Supports US-ASCII characters and german umlauts. See source code of oled.cpp
     * @param scaling Scaling factor. Can be used to double the size of the output. The normal font size is 6x8
     * @param color Color to draw with
     */
    void drawString(uint_fast8_t col, uint_fast8_t row, const char* s, tFontScaling scaling=NORMAL_SIZE, tColor color=WHITE);

    /**
     * @brief Set inverted font write.
     * Wrapper for U8g2 library API compatibility.
     */
    void inverse(void);

    /**
     * @brief Set normal (not inverted) font wite.
     * Wrapper for U8g2 library API compatibility.
     */
    void noInverse(void);

    /**
     * @brief Convert X coordinate to column index.
     * 
     * @param x Pixel coordinate of horizontal position.
     * @return Column index (zero based).
     */
    uint_fast8_t ToCol(uint_fast8_t x);

    /**
     * @brief Convert Y coordinate to row index.
     * 
     * @param y Pixel coordinate of vertical position.
     * @return Row index (zero based).
     */
    uint_fast8_t ToRow(uint_fast8_t y);

    /**
     * @brief Convert column index to X coordinate.
     * 
     * @param col Index of column (zero based).
     * @return X position coordinate.
     */
    uint_fast8_t ToX(uint_fast8_t col);

    /**
     * @brief Convert row index to Y coordinate.
     * 
     * @param row Index of row (zero based).
     * @return Y position coordinate.
     */
    uint_fast8_t ToY(uint_fast8_t row);
        
private:
    /** Pin for the SDA line */
    const uint8_t sda_pin;
    
    /** Pin for the SCL line */
    const uint8_t scl_pin;
    
    /** Pin for the /RST line (value=NO_RESET_PIN, is disabled) */
    const uint8_t reset_pin;
    
    /** Horziontal display size in pixels */
    const uint_fast8_t width;
    
    /** Vertical display size in pixels */
    const uint_fast8_t height;
    
    /** I2C address of the display controller */
    const uint8_t i2c_address;
    
    /** Display controller type SH1106 or SD1306 */
    tDisplayCtrl displayController;

    /** Offset for SH1106 132x64 controller with 128x64 matrix */
    bool usingOffset;
    
    /** Number of pages in the display and buffer */
    const uint_fast8_t pages;
       
    /** Size of the buffer in bytes */
    const uint_fast16_t bufsize;
    
    /** Pointer to the buffer memory */
    uint8_t *buffer;
    
    /** Current X position */
    uint_fast8_t X;

    /** Current Y position */
	uint_fast8_t Y;

	/** tty mode */
	bool ttyMode;

    /** Font inversion flag */
    bool fontInverted;

    /** Send an I2C start signal */
    void i2c_start();
    
    /** Send an I²C stop signal */
    void i2c_stop();
    
    /** Send a byte via I2C and return the ack */
    bool i2c_send(uint8_t byte);
    
    /** Draw a byte into the buffer */
    void draw_byte(uint_fast8_t x, uint_fast8_t y, uint8_t b, tColor color);
    
    /** Draw multiple bytes into the buffer */
    void draw_bytes(uint_fast8_t x, uint_fast8_t y, const uint8_t* data, uint_fast8_t size, tFontScaling scaling, tColor color, bool useProgmem);

    /**
     * @brief Enable/disable inverted print.
     * 
     * @param enabled If true font are printed in inverted background mode.
     */
    void set_font_inverted(bool enabled);
};

#endif

