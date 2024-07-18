#include "st6201.h"
#include "esphome/core/log.h"

namespace esphome {
namespace st6201 {

static const char *const TAG = "st6201";
static const size_t TEMP_BUFFER_SIZE = 128;

void st6201::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SPI st6201...");
#ifdef USE_POWER_SUPPLY
  this->power_.request();
  // the PowerSupply component takes care of post turn-on delay
#endif
  this->spi_setup();
  this->dc_pin_->setup();  // OUTPUT

  this->init_reset_();

  this->write_command_(0xff);
	this->write_data_(0xa5);
	this->write_command_(0xE7); //TE_output_en
	this->write_data_(0x10);
	this->write_command_(0x35); //TE_ interface_en
	this->write_data_(0x00);//01
	this->write_command_(0x36);
	this->write_data_(0xC0);//C8
	this->write_command_(0x3A);
	this->write_data_(0x01);//01---565/00---666
	this->write_command_(0x40);
	this->write_data_(0x01);//01:IPS/00:TN
	this->write_command_(0x41);
	this->write_data_(0x03);//01--8bit//03--16bit
	this->write_command_(0x44); //VBP
	this->write_data_(0x15); 
	this->write_command_(0x45); //VFP
	this->write_data_(0x15);
	this->write_command_(0x7d);//vdds_trim[2:0]
	this->write_data_(0x03);

	this->write_command_(0xc1);//avdd_clp_en avdd_clp[1:0] avcl_clp_en avcl_clp[1:0]
	this->write_data_(0xbb);//0xbb 88 a2
	this->write_command_(0xc2);//vgl_clp_en vgl_clp[2:0]
	this->write_data_(0x05);
	this->write_command_(0xc3);//vgl_clp_en vgl_clp[2:0]
	this->write_data_(0x10);
	this->write_command_(0xc6);//avdd_ratio_sel avcl_ratio_sel vgh_ratio_sel[1:0] vgl_ratio_sel[1:0]
	this->write_data_(0x3e);
	this->write_command_(0xc7);//mv_clk_sel[1:0] avdd_clk_sel[1:0] avcl_clk_sel[1:0]
	this->write_data_(0x25);
	this->write_command_(0xc8);// VGL_CLK_sel
	this->write_data_(0x21);
	this->write_command_(0x7a);// user_vgsp
	this->write_data_(0x51); //58
	this->write_command_(0x6f);// user_gvdd
	this->write_data_(0x49); //4F
	this->write_command_(0x78);// user_gvcl
	this->write_data_(0x57); //70
	this->write_command_(0xc9);
	this->write_data_(0x00);
	this->write_command_(0x67);
	this->write_data_(0x11);
	//gate_ed
	this->write_command_(0x51);//gate_st_o[7:0]
	this->write_data_(0x0a);
	this->write_command_(0x52);//gate_ed_o[7:0]
	this->write_data_(0x7D); //7A
	this->write_command_(0x53);//gate_st_e[7:0]
	this->write_data_(0x0a);
	this->write_command_(0x54);//gate_ed_e[7:0]
	this->write_data_(0x7D); //7A
	//sorce
	this->write_command_(0x46);//fsm_hbp_o[5:0]
	this->write_data_(0x0a); 
	this->write_command_(0x47);//fsm_hfp_o[5:0]
	this->write_data_(0x2a);
	this->write_command_(0x48);//fsm_hbp_e[5:0]
	this->write_data_(0x0a);
	this->write_command_(0x49);//fsm_hfp_e[5:0]
	this->write_data_(0x1a);
	this->write_command_(0x44); //VBP
	this->write_data_(0x15);
	this->write_command_(0x45); //VFP
	this->write_data_(0x15);
	this->write_command_(0x73);
	this->write_data_(0x08);
	this->write_command_(0x74);
	this->write_data_(0x10); //0A
	///test mode
	//this->write_command_(0xf8);
	//this->write_data_(0x16);
	//this->write_command_(0xf9);
	//this->write_data_(0x20);
	this->write_command_(0x56);//src_ld_wd[1:0] src_ld_st[5:0]
	this->write_data_(0x43);
	this->write_command_(0x57);//pn_cs_en src_cs_st[5:0]
	this->write_data_(0x42);
	this->write_command_(0x58);//src_cs_p_wd[6:0]
	this->write_data_(0x3c);
	this->write_command_(0x59);//src_cs_n_wd[6:0]
	this->write_data_(0x64);
	this->write_command_(0x5a);//src_pchg_st_o[6:0]
	this->write_data_(0x41);
	this->write_command_(0x5b);//src_pchg_wd_o[6:0]
	this->write_data_(0x3C);
	this->write_command_(0x5c);//src_pchg_st_e[6:0]
	this->write_data_(0x02);
	this->write_command_(0x5d);//src_pchg_wd_e[6:0]
	this->write_data_(0x3c);
	this->write_command_(0x5e);//src_pol_sw[7:0]
	this->write_data_(0x1f);
	this->write_command_(0x60);//src_op_st_o[7:0]
	this->write_data_(0x80); 
	this->write_command_(0x61);//src_op_st_e[7:0]
	this->write_data_(0x3f);
	this->write_command_(0x62);//src_op_ed_o[9:8] src_op_ed_e[9:8]
	this->write_data_(0x21);
	this->write_command_(0x63);//src_op_ed_o[7:0]
	this->write_data_(0x07);
	this->write_command_(0x64);//src_op_ed_e[7:0]
	this->write_data_(0xe0);
	this->write_command_(0x65);//chopper
	this->write_data_(0x02);
	this->write_command_(0xca); //avdd_mux_st_o[7:0]
	this->write_data_(0x20);
	this->write_command_(0xcb); //avdd_mux_ed_o[7:0]
	this->write_data_(0x52);
	this->write_command_(0xcc); //avdd_mux_st_e[7:0]
	this->write_data_(0x10);
	this->write_command_(0xcD); //avdd_mux_ed_e[7:0]
	this->write_data_(0x42);
	this->write_command_(0xD0); //avcl_mux_st_o[7:0]
	this->write_data_(0x20);
	this->write_command_(0xD1); //avcl_mux_ed_o[7:0]
	this->write_data_(0x52);
	this->write_command_(0xD2); //avcl_mux_st_e[7:0]
	this->write_data_(0x10);
	this->write_command_(0xD3); //avcl_mux_ed_e[7:0]
	this->write_data_(0x42);
	this->write_command_(0xD4); //vgh_mux_st[7:0]
	this->write_data_(0x0a);
	this->write_command_(0xD5); //vgh_mux_ed[7:0]
	this->write_data_(0x32);
	//gammma boe4.3
	this->write_command_(0x80); //gam_vrp0
	this->write_data_(0x00);
	this->write_command_(0xA0); //gam_VRN0
	this->write_data_(0x00);
	this->write_command_(0x81); //gam_vrp1
	this->write_data_(0x06);//07
	this->write_command_(0xA1); //gam_VRN1
	this->write_data_(0x08);//06 
	this->write_command_(0x82); //gam_vrp2
	this->write_data_(0x03);//02
	this->write_command_(0xA2); //gam_VRN2
	this->write_data_(0x03);//01
	this->write_command_(0x86); //gam_prp0
	this->write_data_(0x14); //11
	this->write_command_(0xA6); //gam_PRN0
	this->write_data_(0x14); //10
	this->write_command_(0x87); //gam_prp1
	this->write_data_(0x2C); //27
	this->write_command_(0xA7); //gam_PRN1
	this->write_data_(0x26); //27
	this->write_command_(0x83); //gam_vrp3
	this->write_data_(0x37);
	this->write_command_(0xA3); //gam_VRN3
	this->write_data_(0x37);
	this->write_command_(0x84); //gam_vrp4
	this->write_data_(0x35);
	this->write_command_(0xA4); //gam_VRN4
	this->write_data_(0x35);
	this->write_command_(0x85); //gam_vrp5
	this->write_data_(0x3f);
	this->write_command_(0xA5); //gam_VRN5
	this->write_data_(0x3f);
	this->write_command_(0x88); //gam_pkp0
	this->write_data_(0x0A); //0b
	this->write_command_(0xA8); //gam_PKN0
	this->write_data_(0x0A); //0b
	this->write_command_(0x89); //gam_pkp1
	this->write_data_(0x13); //14
	this->write_command_(0xA9); //gam_PKN1
	this->write_data_(0x12); //13
	this->write_command_(0x8a); //gam_pkp2
	this->write_data_(0x18); //1a
	this->write_command_(0xAa); //gam_PKN2
	this->write_data_(0x19); //1a
	this->write_command_(0x8b); //gam_PKP3
	this->write_data_(0x0a);
	this->write_command_(0xAb); //gam_PKN3
	this->write_data_(0x0a);
	this->write_command_(0x8c); //gam_PKP4
	this->write_data_(0x17);//14
	this->write_command_(0xAc); //gam_PKN4
	this->write_data_(0x0B);//08
	this->write_command_(0x8d); //gam_PKP5
	this->write_data_(0x1A);//17
	this->write_command_(0xAd); //gam_PKN5
	this->write_data_(0x09);//07
	this->write_command_(0x8e); //gam_PKP6
	this->write_data_(0x1A); //16 //16
	this->write_command_(0xAe); //gam_PKN6
	this->write_data_(0x08); //06 //13
	this->write_command_(0x8f); //gam_PKP7
	this->write_data_(0x1F); //1B
	this->write_command_(0xAf); //gam_PKN7
	this->write_data_(0x00); //07
	this->write_command_(0x90); //gam_PKP8
	this->write_data_(0x08);//04
	this->write_command_(0xB0); //gam_PKN8
	this->write_data_(0x00);//04
	this->write_command_(0x91); //gam_PKP9
	this->write_data_(0x10);//0A
	this->write_command_(0xB1); //gam_PKN9
	this->write_data_(0x06);//0A

	this->write_command_(0x92); //gam_PKP10
	this->write_data_(0x19);//16
	this->write_command_(0xB2); //gam_PKN10
	this->write_data_(0x15);//15
	this->write_command_(0xff);
	this->write_data_(0x00);
	this->write_command_(0x11);

	//st6201_set_orientation(CONFIG_LV_DISPLAY_ORIENTATION);
    //!use static first need to change  
    this->write_command_(0x36);
    this->write_data_(0x48);// rotate 180

  // Clear display - ensures we do not see garbage at power-on
  this->draw_filled_rect_(0, 0, this->get_width_internal(), this->get_height_internal(), 0x0000);

  delay(120);  // NOLINT

  this->write_command_(st6201_DISPON);  // Display on
  delay(120);                           // NOLINT

  backlight_(true);

  this->init_internal_(this->get_buffer_length_());
  memset(this->buffer_, 0x00, this->get_buffer_length_());
}

void st6201::dump_config() {
  LOG_DISPLAY("", "SPI st6201", this);
  ESP_LOGCONFIG(TAG, "  Model: %s", this->model_str_);
  ESP_LOGCONFIG(TAG, "  Height: %u", this->height_);
  ESP_LOGCONFIG(TAG, "  Width: %u", this->width_);
  ESP_LOGCONFIG(TAG, "  Height Offset: %u", this->offset_height_);
  ESP_LOGCONFIG(TAG, "  Width Offset: %u", this->offset_width_);
  ESP_LOGCONFIG(TAG, "  8-bit color mode: %s", YESNO(this->eightbitcolor_));
  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  B/L Pin: ", this->backlight_pin_);
  LOG_UPDATE_INTERVAL(this);
  ESP_LOGCONFIG(TAG, "  Data rate: %dMHz", (unsigned) (this->data_rate_ / 1000000));
#ifdef USE_POWER_SUPPLY
  ESP_LOGCONFIG(TAG, "  Power Supply Configured: yes");
#endif
}

float st6201::get_setup_priority() const { return setup_priority::PROCESSOR; }

void st6201::update() {
  this->do_update_();
  this->write_display_data();
}

void st6201::set_model_str(const char *model_str) { this->model_str_ = model_str; }

void st6201::write_display_data() {
  uint16_t x1 = this->offset_height_;
  uint16_t x2 = x1 + get_width_internal() - 1;
  uint16_t y1 = this->offset_width_;
  uint16_t y2 = y1 + get_height_internal() - 1;

  this->enable();

  // set column(x) address
  this->dc_pin_->digital_write(false);
  this->write_byte(st6201_CASET);
  this->dc_pin_->digital_write(true);
  this->write_addr_(x1, x2);
  // set page(y) address
  this->dc_pin_->digital_write(false);
  this->write_byte(st6201_RASET);
  this->dc_pin_->digital_write(true);
  this->write_addr_(y1, y2);
  // write display memory
  this->dc_pin_->digital_write(false);
  this->write_byte(st6201_RAMWR);
  this->dc_pin_->digital_write(true);

  if (this->eightbitcolor_) {
    uint8_t temp_buffer[TEMP_BUFFER_SIZE];
    size_t temp_index = 0;
    for (int line = 0; line < this->get_buffer_length_(); line = line + this->get_width_internal()) {
      for (int index = 0; index < this->get_width_internal(); ++index) {
        auto color = display::ColorUtil::color_to_565(
            display::ColorUtil::to_color(this->buffer_[index + line], display::ColorOrder::COLOR_ORDER_RGB,
                                         display::ColorBitness::COLOR_BITNESS_332, true));
        temp_buffer[temp_index++] = (uint8_t) (color >> 8);
        temp_buffer[temp_index++] = (uint8_t) color;
        if (temp_index == TEMP_BUFFER_SIZE) {
          this->write_array(temp_buffer, TEMP_BUFFER_SIZE);
          temp_index = 0;
        }
      }
    }
    if (temp_index != 0)
      this->write_array(temp_buffer, temp_index);
  } else {
    this->write_array(this->buffer_, this->get_buffer_length_());
  }

  this->disable();
}

void st6201::init_reset_() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);
    delay(1);
    // Trigger Reset
    this->reset_pin_->digital_write(false);
    delay(1);
    // Wake up
    this->reset_pin_->digital_write(true);
    delay(5);
  }
}

void st6201::backlight_(bool onoff) {
  if (this->backlight_pin_ != nullptr) {
    this->backlight_pin_->setup();
    this->backlight_pin_->digital_write(onoff);
  }
}

void st6201::write_command_(uint8_t value) {
  this->enable();
  this->dc_pin_->digital_write(false);
  this->write_byte(value);
  this->dc_pin_->digital_write(true);
  this->disable();
}

void st6201::write_data_(uint8_t value) {
  this->dc_pin_->digital_write(true);
  this->enable();
  this->write_byte(value);
  this->disable();
}

void st6201::write_addr_(uint16_t addr1, uint16_t addr2) {
  static uint8_t byte[4];
  byte[0] = (addr1 >> 8) & 0xFF;
  byte[1] = addr1 & 0xFF;
  byte[2] = (addr2 >> 8) & 0xFF;
  byte[3] = addr2 & 0xFF;

  this->dc_pin_->digital_write(true);
  this->write_array(byte, 4);
}

void st6201::write_color_(uint16_t color, uint16_t size) {
  static uint8_t byte[1024];
  int index = 0;
  for (int i = 0; i < size; i++) {
    byte[index++] = (color >> 8) & 0xFF;
    byte[index++] = color & 0xFF;
  }

  this->dc_pin_->digital_write(true);
  return write_array(byte, size * 2);
}

size_t st6201::get_buffer_length_() {
  if (this->eightbitcolor_) {
    return size_t(this->get_width_internal()) * size_t(this->get_height_internal());
  }
  return size_t(this->get_width_internal()) * size_t(this->get_height_internal()) * 2;
}

// Draw a filled rectangle
// x1: Start X coordinate
// y1: Start Y coordinate
// x2: End X coordinate
// y2: End Y coordinate
// color: color
void st6201::draw_filled_rect_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
  this->enable();
  this->dc_pin_->digital_write(false);
  this->write_byte(st6201_CASET);  // set column(x) address
  this->dc_pin_->digital_write(true);
  this->write_addr_(x1, x2);

  this->dc_pin_->digital_write(false);
  this->write_byte(st6201_RASET);  // set Page(y) address
  this->dc_pin_->digital_write(true);
  this->write_addr_(y1, y2);
  this->dc_pin_->digital_write(false);
  this->write_byte(st6201_RAMWR);  // begin a write to memory
  this->dc_pin_->digital_write(true);
  for (int i = x1; i <= x2; i++) {
    uint16_t size = y2 - y1 + 1;
    this->write_color_(color, size);
  }
  this->disable();
}

void HOT st6201::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0)
    return;

  if (this->eightbitcolor_) {
    auto color332 = display::ColorUtil::color_to_332(color);
    uint32_t pos = (x + y * this->get_width_internal());
    this->buffer_[pos] = color332;
  } else {
    auto color565 = display::ColorUtil::color_to_565(color);
    uint32_t pos = (x + y * this->get_width_internal()) * 2;
    this->buffer_[pos++] = (color565 >> 8) & 0xff;
    this->buffer_[pos] = color565 & 0xff;
  }
}

}  // namespace st6201
}  // namespace esphome