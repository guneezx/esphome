#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace gd_epaper {

class GDEPaper : public PollingComponent,
                        public display::DisplayBuffer,
                        public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                              spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_2MHZ> {
 public:
  void set_dc_pin(GPIOPin *dc_pin) { dc_pin_ = dc_pin; }
  float get_setup_priority() const override;
  void set_reset_pin(GPIOPin *reset) { this->reset_pin_ = reset; }
  void set_busy_pin(GPIOPin *busy) { this->busy_pin_ = busy; }
  void set_reset_duration(uint32_t reset_duration) { this->reset_duration_ = reset_duration; }

  void command(uint8_t value);
  void data(uint8_t value);

  virtual void display() = 0;
  virtual void initialize() = 0;
  virtual void deep_sleep() = 0;

  void update() override;

  void fill(Color color) override;

  void setup() override {
    this->setup_pins_();
    this->initialize();
  }

  void on_safe_shutdown() override;

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;

  bool wait_until_idle_();

  void setup_pins_();

  void reset_() {
    if (this->reset_pin_ != nullptr) {
      this->reset_pin_->digital_write(false);
      delay(reset_duration_);  // NOLINT
      this->reset_pin_->digital_write(true);
      delay(200);  // NOLINT
    }
  }

  uint32_t get_buffer_length_();
  uint32_t reset_duration_{200};

  void start_command_();
  void end_command_();
  void start_data_();
  void end_data_();

  GPIOPin *reset_pin_{nullptr};
  GPIOPin *dc_pin_;
  GPIOPin *busy_pin_{nullptr};
  virtual uint32_t idle_timeout_() { return 1000u; }  // NOLINT(readability-identifier-naming)
};

enum GDEPaperTypeAModel {
  GD_EPAPER_1_54_IN = 0,
};

class GDEPaperTypeA : public GDEPaper {
 public:
  GDEPaperTypeA(GDEPaperTypeAModel model);

  void initialize() override;

  void dump_config() override;

  void display() override;

  void deep_sleep() override {
    if (this->model_ == GD_EPAPER_1_54_IN) {
      // COMMAND DEEP SLEEP MODE
      this->command(0x50);
      this->data(0xf7);
      this->command(0x02); //power off
      this->wait_until_idle_();      
      this->command(0x07);
      this->data(0xA5);
    } 
  }

  void set_full_update_every(uint32_t full_update_every);

 protected:
  void write_lut_(const uint8_t *lut, uint8_t size);

  int get_width_internal() override;

  int get_height_internal() override;

  uint32_t full_update_every_{30};
  uint32_t at_update_{0};
  GDEPaperTypeAModel model_;
  uint32_t idle_timeout_() override;
};



}  // namespace gd_epaper
}  // namespace esphome
