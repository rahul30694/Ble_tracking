#include "main.h"
#include "ble_advdata.h"
#include "nrf_soc.h"
#include "nordic_common.h"
#include "gatts_handler.h"

#include "hal/delay.h"
#include "hal/time.h"
#include "boards.h"
#include "hal/uart.h"
#include "hal/gpio.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define COMPANY_BLE_ID				     0x0059
#define IS_SRVC_CHANGED_CHARACT_PRESENT  1

#define DEVICE_NAME                     "NEW"

#define MOTOR_GPIO                      30

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)           /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(1000, UNIT_1_25_MS)          /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                          /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)            /**< Connection supervisory timeout (4 seconds). */
         

static uint8_t adv_manuf_data[BLE_GAP_ADV_MAX_SIZE] = "nike";
static uint8_t scan_resp_manuf_data[BLE_GAP_ADV_MAX_SIZE] = "Discount:40";

static ble_advdata_manuf_data_t m_manuf_adv;
static ble_advdata_manuf_data_t m_manuf_scan_resp;


void conn_params_update(ble_gap_conn_params_t *p_conn_params, uint16_t min_conn_int, 
                    uint16_t max_conn_int, uint16_t slave_latency, uint16_t sup_to)
{
    memset(p_conn_params, 0, sizeof(*p_conn_params));

    p_conn_params->min_conn_interval = min_conn_int;
    p_conn_params->max_conn_interval = max_conn_int;
    p_conn_params->slave_latency = slave_latency;
    p_conn_params->conn_sup_timeout = sup_to;
}



int gap_params_init(void)
{
	ble_gap_addr_t gap_address;
    uint8_t add[6] = { 0x9A, 0x01, 0x05, 0x03, 0x04, 0xC0 };
    memcpy(&gap_address.addr, add, 6);
	gap_address.addr_type = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;


	if (sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE, &gap_address) != NRF_SUCCESS)
		return -1;

    // if (sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_TAG) != NRF_SUCCESS)
    //    return -1;
    
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    if (sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME)) != NRF_SUCCESS) {
        return -1;
    }

    ble_gap_conn_params_t conn_params;
    conn_params_update(&conn_params, MIN_CONN_INTERVAL, MAX_CONN_INTERVAL, 
                            SLAVE_LATENCY, CONN_SUP_TIMEOUT);

    if (sd_ble_gap_ppcp_set(&conn_params) != NRF_SUCCESS)
        return -1;
	return 0;
}


int advertising_init(void)
{
    ble_advdata_t advdata;

    m_manuf_adv.company_identifier = COMPANY_BLE_ID; // Nordics company ID
    m_manuf_adv.data.p_data = adv_manuf_data;     
    m_manuf_adv.data.size = strlen((const char *)adv_manuf_data);


    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.p_manuf_specific_data = &m_manuf_adv;
    // int8_t tx_power = -4;
    // advdata.p_tx_power_level = &tx_power;
    advdata.include_appearance = false;

    // Scan Response Data
    ble_advdata_t advdata_response;

     // Prepare the scan response Manufacturer specific data packet
    m_manuf_scan_resp.company_identifier = COMPANY_BLE_ID;               
    m_manuf_scan_resp.data.p_data = scan_resp_manuf_data;        
    m_manuf_scan_resp.data.size = strlen((const char *)scan_resp_manuf_data);

    // Always initialize all fields in structs to zero or you might get unexpected behaviour
    memset(&advdata_response, 0, sizeof(advdata_response));

    // Populate the scan response packet
    advdata_response.name_type = BLE_ADVDATA_NO_NAME; 
    advdata_response.p_manuf_specific_data = &m_manuf_scan_resp;


    if (ble_advdata_set(&advdata, &advdata_response) != NRF_SUCCESS)
    	return -1;
    return 0;
}

void rx_handler(uint8_t *p_data, unsigned length)
{
    printf("RX Length - %u\r\n", length);
    if (!strncmp((const char *)p_data, "ON", 2)) {
        gpio_write(MOTOR_GPIO, true);
        if (ble_trs_transmit((uint8_t *)"OK", 2))
            printf("Transmission Couldn't Send\r\n");
    } else {
        gpio_write(MOTOR_GPIO, false);
        if (ble_trs_transmit((uint8_t *)"NOT", 3))
            printf("Transmission Couldn't Send\r\n");
    }
}

void tx_handler(bool successfull)
{
    if (successfull)
        printf("Transmit Rec Ack\r\n");
    else
        printf("Transmit Rec NACK\r\n");

}
int advertising_start(void);

static void ble_evt_dispatch(ble_evt_t *p_ble_evt)
{
    handler(p_ble_evt);
    gatts_handler(p_ble_evt);

    if (p_ble_evt->header.evt_id == BLE_GAP_EVT_DISCONNECTED)
        advertising_start();
}

static void sys_evt_dispatch(uint32_t sys_evt)
{

}

// It is e.g. typically 20%-25% lower current consumption during advertising 
// if 10 payload bytes are sent in each advertising packet than the maximum of 31 bytes.
int advertising_start(void)
{
    ble_gap_adv_params_t adv_params;
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.type = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr = NULL;
    adv_params.fp = BLE_GAP_ADV_FP_ANY;
    adv_params.p_whitelist = NULL;
    adv_params.interval = 0x0020;
    adv_params.timeout =  0x3FFF;

    adv_params.channel_mask.ch_37_off = 1;
    adv_params.channel_mask.ch_39_off = 1;

    if (sd_ble_gap_adv_start(&adv_params) != NRF_SUCCESS)
        return -1;
    return 0;
 
}



static void ble_stack_init(void)
{
    uint32_t err_code;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);

    // Enable BLE stack 
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = sd_ble_enable(&ble_enable_params);
    // APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    // APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    // APP_ERROR_CHECK(err_code);
}

static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1                           /**< UART RX buffer size. */

//void uart_error_handle(app_uart_evt_t * p_event)
//{
//    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
//    {
//        // APP_ERROR_HANDLER(p_event->data.error_communication);
//    }
//    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
//    {
//        // APP_ERROR_HANDLER(p_event->data.error_code);
//    }
//}

int main()
{
    uint32_t err_code;
    (void)err_code;

    // const app_uart_comm_params_t comm_params =
    //   {
    //       RX_PIN_NUMBER,
    //       TX_PIN_NUMBER,
    //       RTS_PIN_NUMBER,
    //       CTS_PIN_NUMBER,
    //       APP_UART_FLOW_CONTROL_DISABLED,
    //       false,
    //       UART_BAUDRATE_BAUDRATE_Baud115200
    //   };

    // APP_UART_FIFO_INIT(&comm_params,
    //                      UART_RX_BUF_SIZE,
    //                      UART_TX_BUF_SIZE,
    //                      uart_error_handle,
    //                      APP_IRQ_PRIORITY_LOW,
    //                      err_code);

	ble_stack_init();

	gpio_output_cfg(LED_1);
	gpio_output_cfg(LED_2);
	gpio_output_cfg(LED_3);
	gpio_output_cfg(LED_4);
    gpio_output_cfg(MOTOR_GPIO);
	
	uart_cfg(115200, false);
	gpio_write(LED_1, true);
	gpio_write(LED_2, false);
	gpio_write(LED_3, true);
    gpio_write(LED_4, true);
	gpio_write(MOTOR_GPIO, false);

    printf("\033[2J");
    printf("Screen Cleared\r\n");
    printf("In Main\r\n");

	if (gap_params_init()) {
        printf("Failed Gap Init\r\n");
    }

    if (trs_init(rx_handler, tx_handler))
        printf("TRS Init Failed\r\n");


    if (advertising_init()) {
        printf("Adv init Failed\r\n");
    }

    if (advertising_start())
        printf("ADv start failed\r\n");
    conn_manager_init();
    printf("Size of ble_evt_t - %u\r\n", sizeof(ble_evt_t));

	while (true) {
		// power_manage();
        char c = '\0';
        scanf("%c", &c);
        if (isspace(c))
            printf("Alive\r\n");
	}

}

