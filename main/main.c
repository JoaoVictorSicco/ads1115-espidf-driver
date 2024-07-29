#include <stdio.h>
#include "ads1115.h"

#define I2C_MASTER_SCL_IO 22    /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 21    /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0 /*!< I2C port number for master dev */ 
#define I2C_MASTER_FREQ_HZ 100000 /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */

/* Escolhe o pino A0, A1, S2 ou A3*/

ads1115_mux_t A0 = ADS1115_MUX_0_GND;
ads1115_mux_t A1 = ADS1115_MUX_1_GND;
ads1115_mux_t A2 = ADS1115_MUX_2_GND;
ads1115_mux_t A3 = ADS1115_MUX_3_GND;


static esp_err_t i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void read_voltage_task(void *pvParameter) {

    ads1115_t my_ads = ads1115_config(I2C_MASTER_NUM, 0x48); // Inicialize o ADS1115
    ads1115_set_mux(&my_ads, A0); // Seleciona A0, a1, A2 ou A3 em relação ao GND


    while (1) {
        double valor = ads1115_get_voltage(&my_ads);
        printf("Valor: %.2lf V\n", valor);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Aguarda 1 segundo
    }
}

void read_analog_pin(void *pvParameter)
{
    ads1115_t my_ads = ads1115_config(I2C_MASTER_NUM, 0x48); // Inicialize o ADS1115
    ads1115_set_mux(&my_ads, A0); // Seleciona A0, a1, A2 ou A3 em relação ao GND

    while(1)
    {
        int16_t analog_value = ads1115_get_raw(&my_ads);
        printf("Valor analógico: %d\n", analog_value);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Aguarda 1 segundo

    }

}

void app_main(void)
{
    esp_err_t err = i2c_master_init();
    if (err != ESP_OK) {
        printf("I2C init failed: %s\n", esp_err_to_name(err));
        return;
    }
    xTaskCreate(read_voltage_task, "read_voltage_task", 4096, NULL, 1, NULL);
    // xTaskCreate(read_analog_pin, "read_analog_pin_task", 4096, NULL, 1, NULL);


}
