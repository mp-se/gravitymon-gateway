/*
 * GravityMon Gateway
 * Copyright (c) 2021-2026 Magnus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef SRC_ESP_PANEL_BOARD_CUSTOM_CONF_HPP_
#define SRC_ESP_PANEL_CUSTOM_CONF_HPP_

#if defined(WAVESHARE_S3_TFT43)
/*
 * ESP32_Display_Panel board configuration for Waveshare ESP32-S3-Touch-LCD-4.3.
 * Only active when WAVESHARE_S3_TFT43 is defined (gateway32s3wave-tft43-sd env).
 * This header is discovered automatically by ESP32_Display_Panel at compile time.
 */

#define ESP_PANEL_BOARD_DEFAULT_USE_CUSTOM  (1)

#define ESP_PANEL_BOARD_NAME    "Waveshare:ESP32-S3-Touch-LCD-4.3"
#define ESP_PANEL_BOARD_WIDTH   (800)
#define ESP_PANEL_BOARD_HEIGHT  (480)

// ── LCD ──────────────────────────────────────────────────────────────────────

#define ESP_PANEL_BOARD_USE_LCD             (1)
#define ESP_PANEL_BOARD_LCD_CONTROLLER      ST7262
#define ESP_PANEL_BOARD_LCD_BUS_TYPE        (ESP_PANEL_BUS_TYPE_RGB)

#define ESP_PANEL_BOARD_LCD_RGB_USE_CONTROL_PANEL   (0)
#define ESP_PANEL_BOARD_LCD_RGB_CLK_HZ      (16 * 1000 * 1000)
#define ESP_PANEL_BOARD_LCD_RGB_HPW         (30)
#define ESP_PANEL_BOARD_LCD_RGB_HBP         (30)
#define ESP_PANEL_BOARD_LCD_RGB_HFP         (210)
#define ESP_PANEL_BOARD_LCD_RGB_VPW         (4)
#define ESP_PANEL_BOARD_LCD_RGB_VBP         (4)
#define ESP_PANEL_BOARD_LCD_RGB_VFP         (4)
#define ESP_PANEL_BOARD_LCD_RGB_PCLK_ACTIVE_NEG    (1)
#define ESP_PANEL_BOARD_LCD_RGB_DATA_WIDTH  (16)
#define ESP_PANEL_BOARD_LCD_RGB_PIXEL_BITS  (ESP_PANEL_LCD_COLOR_BITS_RGB565)
// Bounce buffer: decouples LCD_CAM GDMA from PSRAM, eliminating "screen drift"
// on ESP32-S3.  Size = width × 10 lines.  Set to 0 here; Display::setup()
// calls configRGB_BounceBufferSize() programmatically after configFrameBufferNumber().
#define ESP_PANEL_BOARD_LCD_RGB_BOUNCE_BUF_SIZE     (0)

#define ESP_PANEL_BOARD_LCD_RGB_IO_HSYNC    (46)
#define ESP_PANEL_BOARD_LCD_RGB_IO_VSYNC    (3)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DE       (5)
#define ESP_PANEL_BOARD_LCD_RGB_IO_PCLK     (7)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DISP     (-1)

#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA0    (14)   // B0
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA1    (38)   // B1
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA2    (18)   // B2
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA3    (17)   // B3
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA4    (10)   // B4
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA5    (39)   // G0
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA6    (0)    // G1
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA7    (45)   // G2
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA8    (48)   // G3
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA9    (47)   // G4
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA10   (21)   // G5
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA11   (1)    // R0
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA12   (2)    // R1
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA13   (42)   // R2
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA14   (41)   // R3
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA15   (40)   // R4

#define ESP_PANEL_BOARD_LCD_COLOR_BITS          (ESP_PANEL_LCD_COLOR_BITS_RGB565)
#define ESP_PANEL_BOARD_LCD_COLOR_BGR_ORDER     (0)
#define ESP_PANEL_BOARD_LCD_COLOR_INEVRT_BIT    (0)
#define ESP_PANEL_BOARD_LCD_SWAP_XY             (0)
#define ESP_PANEL_BOARD_LCD_MIRROR_X            (0)
#define ESP_PANEL_BOARD_LCD_MIRROR_Y            (0)
#define ESP_PANEL_BOARD_LCD_GAP_X               (0)
#define ESP_PANEL_BOARD_LCD_GAP_Y               (0)
#define ESP_PANEL_BOARD_LCD_RST_IO              (-1)
#define ESP_PANEL_BOARD_LCD_RST_LEVEL           (0)

// ── Touch ─────────────────────────────────────────────────────────────────────

#define ESP_PANEL_BOARD_USE_TOUCH               (1)
#define ESP_PANEL_BOARD_TOUCH_CONTROLLER        GT911
#define ESP_PANEL_BOARD_TOUCH_BUS_TYPE          (ESP_PANEL_BUS_TYPE_I2C)
#define ESP_PANEL_BOARD_TOUCH_BUS_SKIP_INIT_HOST (0)
#define ESP_PANEL_BOARD_TOUCH_I2C_HOST_ID       (1)
#define ESP_PANEL_BOARD_TOUCH_I2C_CLK_HZ        (400 * 1000)
#define ESP_PANEL_BOARD_TOUCH_I2C_SCL_PULLUP    (1)
#define ESP_PANEL_BOARD_TOUCH_I2C_SDA_PULLUP    (1)
#define ESP_PANEL_BOARD_TOUCH_I2C_IO_SCL        (9)
#define ESP_PANEL_BOARD_TOUCH_I2C_IO_SDA        (8)
#define ESP_PANEL_BOARD_TOUCH_I2C_ADDRESS       (0)
#define ESP_PANEL_BOARD_TOUCH_SWAP_XY           (0)
#define ESP_PANEL_BOARD_TOUCH_MIRROR_X          (0)
#define ESP_PANEL_BOARD_TOUCH_MIRROR_Y          (0)
#define ESP_PANEL_BOARD_TOUCH_RST_IO            (-1)
#define ESP_PANEL_BOARD_TOUCH_RST_LEVEL         (0)
#define ESP_PANEL_BOARD_TOUCH_INT_IO            (4)
#define ESP_PANEL_BOARD_TOUCH_INT_LEVEL         (0)

// ── Backlight ─────────────────────────────────────────────────────────────────

#define ESP_PANEL_BOARD_USE_BACKLIGHT           (1)
#define ESP_PANEL_BOARD_BACKLIGHT_TYPE          (ESP_PANEL_BACKLIGHT_TYPE_SWITCH_EXPANDER)
#define ESP_PANEL_BOARD_BACKLIGHT_IO            (2)   // CH422G EXIO2
#define ESP_PANEL_BOARD_BACKLIGHT_ON_LEVEL      (1)
#define ESP_PANEL_BOARD_BACKLIGHT_IDLE_OFF      (0)

// ── IO Expander (CH422G) ───────────────────────────────────────────────────────

#define ESP_PANEL_BOARD_USE_EXPANDER            (1)
#define ESP_PANEL_BOARD_EXPANDER_CHIP           CH422G
#define ESP_PANEL_BOARD_EXPANDER_SKIP_INIT_HOST (0)
#define ESP_PANEL_BOARD_EXPANDER_I2C_HOST_ID    (1)
#define ESP_PANEL_BOARD_EXPANDER_I2C_CLK_HZ     (400 * 1000)
#define ESP_PANEL_BOARD_EXPANDER_I2C_SCL_PULLUP (1)
#define ESP_PANEL_BOARD_EXPANDER_I2C_SDA_PULLUP (1)
#define ESP_PANEL_BOARD_EXPANDER_I2C_IO_SCL     (9)
#define ESP_PANEL_BOARD_EXPANDER_I2C_IO_SDA     (8)
#define ESP_PANEL_BOARD_EXPANDER_I2C_ADDRESS    (0x20)

// Enable all CH422G pins as output before LCD/touch init
#define ESP_PANEL_BOARD_EXPANDER_POST_BEGIN_FUNCTION(p) \
    { \
        auto board = static_cast<Board *>(p); \
        auto expander = static_cast<esp_expander::CH422G*>(board->getIO_Expander()->getBase()); \
        expander->enableAllIO_Output(); \
        return true; \
    }

// Assert LCD reset via CH422G EXIO3 before panel init
#define ESP_PANEL_BOARD_LCD_PRE_BEGIN_FUNCTION(p) \
    { \
        constexpr int LCD_RST = 3; \
        auto board = static_cast<Board *>(p); \
        auto expander = board->getIO_Expander()->getBase(); \
        expander->digitalWrite(LCD_RST, 0); \
        vTaskDelay(pdMS_TO_TICKS(10)); \
        expander->digitalWrite(LCD_RST, 1); \
        vTaskDelay(pdMS_TO_TICKS(100)); \
        return true; \
    }

// Toggle GT911 RST (CH422G EXIO1) and INT (GPIO4) for address selection
#define ESP_PANEL_BOARD_TOUCH_PRE_BEGIN_FUNCTION(p) \
    { \
        constexpr gpio_num_t TP_INT = static_cast<gpio_num_t>(ESP_PANEL_BOARD_TOUCH_INT_IO); \
        constexpr int TP_RST = 1; \
        auto board = static_cast<Board *>(p); \
        auto expander = board->getIO_Expander()->getBase(); \
        gpio_set_direction(TP_INT, GPIO_MODE_OUTPUT); \
        gpio_set_level(TP_INT, 0); \
        vTaskDelay(pdMS_TO_TICKS(10)); \
        expander->digitalWrite(TP_RST, 0); \
        vTaskDelay(pdMS_TO_TICKS(100)); \
        expander->digitalWrite(TP_RST, 1); \
        vTaskDelay(pdMS_TO_TICKS(200)); \
        gpio_reset_pin(TP_INT); \
        return true; \
    }

#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_MAJOR 1
#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_MINOR 0
#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_PATCH 0

#endif  // WAVESHARE_S3_TFT43

#endif  // SRC_ESP_PANEL_BOARD_CUSTOM_CONF_HPP_
