#include "gd_epaper.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace gd_epaper {

static const char *const TAG = "gd_epaper";

static const uint8_t LUT_SIZE_GD_VCOM = 56;
static const uint8_t LUT_SIZE_GD_WW = 42;
static const uint8_t LUT_SIZE_GD_BW = 56;
static const uint8_t LUT_SIZE_GD_WB = 56;
static const uint8_t LUT_SIZE_GD_BB = 56;
  
static const uint8_t FULL_UPDATE_LUT_VCOM[LUT_SIZE_GD_VCOM] = {  
  0x01, 0x05, 0x05, 0x05, 0x05, 0x01, 0x01,
  0x01, 0x05, 0x05, 0x05, 0x05, 0x01, 0x01,
  0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const uint8_t PARTIAL_UPDATE_LUT_VCOM[LUT_SIZE_GD_VCOM] = {
  0x01, 0x04, 0x04, 0x03, 0x01, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
static const uint8_t FULL_UPDATE_LUT_WW[LUT_SIZE_GD_WW] = {  
  0x01, 0x45, 0x45, 0x43, 0x44, 0x01, 0x01,
  0x01, 0x87, 0x83, 0x87, 0x06, 0x01, 0x01,
  0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const uint8_t PARTIAL_UPDATE_LUT_WW[LUT_SIZE_GD_WW] = {
  0x01, 0x04, 0x04, 0x03, 0x01, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  

static const uint8_t FULL_UPDATE_LUT_BW[LUT_SIZE_GD_BW] = {  
  0x01, 0x05, 0x05, 0x45, 0x42, 0x01, 0x01,
  0x01, 0x87, 0x85, 0x85, 0x85, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const uint8_t PARTIAL_UPDATE_LUT_BW[LUT_SIZE_GD_BW] = {
  0x01, 0x84, 0x84, 0x83, 0x01, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
static const uint8_t FULL_UPDATE_LUT_WB[LUT_SIZE_GD_WB] = {  
  0x01, 0x08, 0x08, 0x82, 0x42, 0x01, 0x01,
  0x01, 0x45, 0x45, 0x45, 0x45, 0x01, 0x01,
  0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const uint8_t PARTIAL_UPDATE_LUT_WB[LUT_SIZE_GD_WB] = {
  0x01, 0x44, 0x44, 0x43, 0x01, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
static const uint8_t FULL_UPDATE_LUT_BB[LUT_SIZE_GD_BB] = {  
  0x01, 0x85, 0x85, 0x85, 0x83, 0x01, 0x01,
  0x01, 0x45, 0x45, 0x04, 0x48, 0x01, 0x01,
  0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const uint8_t PARTIAL_UPDATE_LUT_BB[LUT_SIZE_GD_BB] = {
  0x01, 0x04, 0x04, 0x03, 0x01, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  

void GDEPaper::setup_pins_() {
  this->init_internal_(this->get_buffer_length_());
  this->dc_pin_->setup();  // OUTPUT
  this->dc_pin_->digital_write(false);
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();  // OUTPUT
    this->reset_pin_->digital_write(true);
  }
  if (this->busy_pin_ != nullptr) {
    this->busy_pin_->setup();  // INPUT
  }
  this->spi_setup();

  this->reset_();
}
float GDEPaper::get_setup_priority() const { return setup_priority::PROCESSOR; }
void GDEPaper::command(uint8_t value) {
  this->start_command_();
  this->write_byte(value);
  this->end_command_();
}
void GDEPaper::data(uint8_t value) {
  this->start_data_();
  this->write_byte(value);
  this->end_data_();
}
bool GDEPaper::wait_until_idle_() {
  if (this->busy_pin_ == nullptr) {
    return true;
  }

  const uint32_t start = millis();
  while (this->busy_pin_->digital_read()) {
    if (millis() - start > this->idle_timeout_()) {
      ESP_LOGE(TAG, "Timeout while displaying image!");
      return false;
    }
    delay(10);
  }
  return true;
}
void GDEPaper::update() {
  this->do_update_();
  this->display();
}
void GDEPaper::fill(Color color) {
  // flip logic
  const uint8_t fill = color.is_on() ? 0x00 : 0xFF;
  for (uint32_t i = 0; i < this->get_buffer_length_(); i++)
    this->buffer_[i] = fill;
}
void HOT GDEPaper::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || y >= this->get_height_internal() || x < 0 || y < 0)
    return;

  const uint32_t pos = (x + y * this->get_width_internal()) / 8u;
  const uint8_t subpos = x & 0x07;
  // flip logic
  if (!color.is_on()) {
    this->buffer_[pos] |= 0x80 >> subpos;
  } else {
    this->buffer_[pos] &= ~(0x80 >> subpos);
  }
}
uint32_t GDEPaper::get_buffer_length_() { return this->get_width_internal() * this->get_height_internal() / 8u; }
void GDEPaper::start_command_() {
  this->dc_pin_->digital_write(false);
  this->enable();
}
void GDEPaper::end_command_() { this->disable(); }
void GDEPaper::start_data_() {
  this->dc_pin_->digital_write(true);
  this->enable();
}
void GDEPaper::end_data_() { this->disable(); }
void GDEPaper::on_safe_shutdown() { this->deep_sleep(); }

// ========================================================
//                          Type A
// ========================================================

void GDEPaperTypeA::initialize() {

    this->command(0X50);
    this->data(0xd7);
    this->command(0X04);
    delay(50);
  
  this->command(0x00); // panel setting
  this->data (0xdf);
  this->data (0x0e);
  //this->command(0x01); // power setting
  //this->data(0x03);
  //this->data(0x06); // 16V
  //this->data(0x2A);//
  //this->data(0x2A);//
  this->command(0x4D); // FITIinternal code
  this->data (0x55);
  this->command(0xaa);
  this->data (0x0f);
  this->command(0xE9);
  this->data (0x02);
  this->command(0xb6);
  this->data (0x11);
  this->command(0xF3);
  this->data (0x0a);
  //this->command(0x06); // boost soft start
  //this->data (0xc7);
  //this->data (0x0c);
  //this->data (0x0c);
  this->command(0x61); // resolution setting
  this->data (0xc8); // 200
  this->data (0x00);
  this->data (0xc8); // 200
  this->command(0x60); // Tcon setting
  this->data (0x00);
  //this->command(0x82); // VCOM DC setting
  //this->data (0x12);
  //this->command(0x30); // PLL control
  //this->data (0x3C);   // default 50Hz
  this->command(0X50); // VCOM and data interval
  this->data(0xD7);//
  this->command(0XE3); // power saving register
  this->data(0x00); // default
  
  this->command(0X04);
  delay(50);
  
  this->command(0x10);
  for (uint32_t i = 0; i < 200*200; i++){
      this->data(0xff);
  }
  delay(2);
  
  this->command(0x13);
  for (uint32_t i = 0; i < 200*200; i++){
      this->data(0x00);
  }
  
  delay(2);
  this->command(0x12);
  delay(10);
  
  this->command(0x10);
  for (uint32_t i = 0; i < 200*200; i++){
      this->data(0x00);
  }
  delay(2);

  this->command(0x13);
  for (uint32_t i = 0; i < 200*200; i++){
      this->data(0xff);
  }
  
  this->command(0x12);
  delay(10);
LOG_DISPLAY("", "Good Display E-Paper Initialized", this);  
}
void GDEPaperTypeA::dump_config() {
  LOG_DISPLAY("", "Good Display E-Paper", this);
  switch (this->model_) {
    case GD_EPAPER_1_54_IN:
      ESP_LOGCONFIG(TAG, "  Model: 1.54in");
      break;
  }
  ESP_LOGCONFIG(TAG, "  Full Update Every: %u", this->full_update_every_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Busy Pin: ", this->busy_pin_);
  LOG_UPDATE_INTERVAL(this);
}
void HOT GDEPaperTypeA::display() {
  bool full_update = this->at_update_ == 0;
  bool prev_full_update = this->at_update_ == 1;

    this->command(0X50);
    this->data(0xd7);
    this->command(0X04);
    delay(50);
  
  this->command(0x00); // panel setting
  this->data (0xdf);
  this->data (0x0e);
  //this->command(0x01); // power setting
  //this->data(0x03);
  //this->data(0x06); // 16V
  //this->data(0x2A);//
  //this->data(0x2A);//
  this->command(0x4D); // FITIinternal code
  this->data (0x55);
  this->command(0xaa);
  this->data (0x0f);
  this->command(0xE9);
  this->data (0x02);
  this->command(0xb6);
  this->data (0x11);
  this->command(0xF3);
  this->data (0x0a);
  //this->command(0x06); // boost soft start
  //this->data (0xc7);
  //this->data (0x0c);
  //this->data (0x0c);
  this->command(0x61); // resolution setting
  this->data (0xc8); // 200
  this->data (0x00);
  this->data (0xc8); // 200
  this->command(0x60); // Tcon setting
  this->data (0x00);
  //this->command(0x82); // VCOM DC setting
  //this->data (0x12);
  //this->command(0x30); // PLL control
  //this->data (0x3C);   // default 50Hz
  this->command(0X50); // VCOM and data interval
  this->data(0xD7);//
  this->command(0XE3); // power saving register
  this->data(0x00); // default
  
  this->command(0X04);
  delay(50);
  
  this->command(0x10);
  for (uint32_t i = 0; i < 200*200; i++){
      this->data(0xff);
  }
  delay(2);
  
  this->command(0x13);
  for (uint32_t i = 0; i < 200*200; i++){
      this->data(0x00);
  }
  
  delay(2);
  this->command(0x12);
  delay(10);
  
  this->command(0x10);
  for (uint32_t i = 0; i < 200*200; i++){
      this->data(0x00);
  }
  delay(2);

  this->command(0x13);
  for (uint32_t i = 0; i < 200*200; i++){
      this->data(0xff);
  }
  
  this->command(0x12);
  delay(10);

LOG_DISPLAY("", "Good Display E-Paper Initialized", this);    
  
  
  if (!this->wait_until_idle_()) {
    this->status_set_warning();
    return;
  }

  if (this->full_update_every_ >= 1) {
    if (full_update != prev_full_update) {
      switch (this->model_) {
        default:
          this->command(0x20);
          this->write_lut_(full_update ? FULL_UPDATE_LUT_VCOM : PARTIAL_UPDATE_LUT_VCOM, LUT_SIZE_GD_VCOM);
          this->command(0x21);
          this->write_lut_(full_update ? FULL_UPDATE_LUT_WW : PARTIAL_UPDATE_LUT_WW, LUT_SIZE_GD_WW);
          this->command(0x22);
          this->write_lut_(full_update ? FULL_UPDATE_LUT_WB : PARTIAL_UPDATE_LUT_WB, LUT_SIZE_GD_WB);
          this->command(0x23);
          this->write_lut_(full_update ? FULL_UPDATE_LUT_BW : PARTIAL_UPDATE_LUT_BW, LUT_SIZE_GD_BW);
          this->command(0x24);
          this->write_lut_(full_update ? FULL_UPDATE_LUT_BB : PARTIAL_UPDATE_LUT_BB, LUT_SIZE_GD_BB);          
      }
    }
    this->at_update_ = (this->at_update_ + 1) % this->full_update_every_;
  }

  // Set x & y regions we want to write to (full)
  switch (this->model_) {
    default:
      // COMMAND SET RAM X ADDRESS START END POSITION
      this->command(0x44);
      this->data(0x00);
      this->data((this->get_width_internal() - 1) >> 3);
      // COMMAND SET RAM Y ADDRESS START END POSITION
      this->command(0x45);
      this->data(0x00);
      this->data(0x00);
      this->data(this->get_height_internal() - 1);
      this->data((this->get_height_internal() - 1) >> 8);

      // COMMAND SET RAM X ADDRESS COUNTER
      this->command(0x4E);
      this->data(0x00);
      // COMMAND SET RAM Y ADDRESS COUNTER
      this->command(0x4F);
      this->data(0x00);
      this->data(0x00);
  }

  if (!this->wait_until_idle_()) {
    this->status_set_warning();
    return;
  }

  // COMMAND WRITE RAM
  this->command(0x24);
  this->start_data_();
  switch (this->model_) {
    default:
      this->write_array(this->buffer_, this->get_buffer_length_());
  }
  this->end_data_();

  // COMMAND DISPLAY UPDATE CONTROL 2
  this->command(0x22);
  switch (this->model_) {
    default:
      this->data(0xC4);
      break;
  }

  // COMMAND MASTER ACTIVATION
  this->command(0x20);
  // COMMAND TERMINATE FRAME READ WRITE
  this->command(0xFF);

  this->status_clear_warning();
  LOG_DISPLAY("", "Good Display E-Paper Displayed", this);
}
int GDEPaperTypeA::get_width_internal() {
  switch (this->model_) {
    case GD_EPAPER_1_54_IN:
      return 200;
  }
  return 0;
}
int GDEPaperTypeA::get_height_internal() {
  switch (this->model_) {
    case GD_EPAPER_1_54_IN:
      return 200;
  }
  return 0;
}
void GDEPaperTypeA::write_lut_(const uint8_t *lut, const uint8_t size) {
  // COMMAND WRITE LUT REGISTER
  for (uint8_t i = 0; i < size; i++)
    this->data(lut[i]);
}
GDEPaperTypeA::GDEPaperTypeA(GDEPaperTypeAModel model) : model_(model) {}
void GDEPaperTypeA::set_full_update_every(uint32_t full_update_every) {
  this->full_update_every_ = full_update_every;
}

uint32_t GDEPaperTypeA::idle_timeout_() {
  switch (this->model_) {
    default:
      return GDEPaper::idle_timeout_();
  }
}



}  // namespace gd_epaper
}  // namespace esphome
