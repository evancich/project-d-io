/**************************************************************************//**
 * @file
 * @brief Project D
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2018 Zeteo </b>
 ******************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_adc.h"
#include "em_emu.h"
#include "em_idac.h"
#include "em_timer.h"
#include "em_ldma.h"

#include "40-smith-wes-gun-shot.h"

#define adcFreq   16000000

#define AUDIO_DATA_WINDOW_SIZE 32
#define AUDIO_DATA_WINDOW_LEN 16 * 1024

volatile uint32_t sample;
uint16_t audioData_counter;
unsigned char audioData[AUDIO_DATA_WINDOW_LEN];

// Use a 32-point sine table
#define SINE_TABLE_SIZE 32

// Note: change this to choose the current range of the output
#define IDAC_RANGE IDAC_CURPROG_RANGESEL_RANGE3

// Note: change this to determine the frequency of the output
#define WAVEFORM_FREQ 10000

// The timer needs to run at FIX ME SIZE
#define TIMER0_FREQ (WAVEFORM_FREQ * 16 * 1024)



/**************************************************************************//**
 * @brief
 *    Timer initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Initialize TIMER0
  TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;
  init.enable = false;
  TIMER_Init(TIMER0, &init);

  // Set top (reload) value for the timer
  // Note: the timer runs off of the HFPER clock
  uint32_t topValue = CMU_ClockFreqGet(cmuClock_HFPER) / TIMER0_FREQ;
  TIMER_TopBufSet(TIMER0, topValue);

  // Automatically clear the LDMA request
  TIMER0->CTRL |= TIMER_CTRL_DMACLRACT;

  // Enable TIMER0
  TIMER_Enable(TIMER0, true);
}

/**************************************************************************//**
 * @brief
 *    IDAC initialization
 *****************************************************************************/
void initIdac(void)
{
  // Enable IDAC clock
  CMU_ClockEnable(cmuClock_IDAC0, true);

  // Initialize IDAC
  IDAC_Init_TypeDef init = IDAC_INIT_DEFAULT;
  init.outMode = idacOutputAPORT1XCH10; // Choose output to be on APORT1X Channel 10
  IDAC_Init(IDAC0, &init);

  // Enable IDAC output mode and also enable the IDAC module itself
  IDAC_OutEnable(IDAC0, true);
  IDAC_Enable(IDAC0, true);
}




/**************************************************************************//**
 * @brief  Initialize ADC function
 *****************************************************************************/
void initADC (void)
{
  // Enable ADC0 clock
  CMU_ClockEnable(cmuClock_ADC0, true);

  // Declare init structs
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;

  // Modify init structs and initialize
  init.prescale = ADC_PrescaleCalc(adcFreq, 0); // Init to max ADC clock for Series 1

  initSingle.diff       = false;        // single ended
  initSingle.reference  = adcRef2V5;    // internal 2.5V reference
  initSingle.resolution = adcRes12Bit;  // 12-bit resolution
  initSingle.acqTime    = adcAcqTime4;  // set acquisition time to meet minimum requirements

  // Select ADC input. See README for corresponding EXP header pin.
  initSingle.posSel = adcPosSelAPORT2XCH9;

  ADC_Init(ADC0, &init);
  ADC_InitSingle(ADC0, &initSingle);

  // Enable ADC Single Conversion Complete interrupt
  ADC_IntEnable(ADC0, ADC_IEN_SINGLE);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(ADC0_IRQn);
}

/**************************************************************************//**
 * @brief  ADC Handler
 *****************************************************************************/
void ADC0_IRQHandler(void)
{
  // Get ADC result
  sample = ADC_DataSingleGet(ADC0);

  audioData[audioData_counter++] = sample;

  // Start next ADC conversion
  ADC_Start(ADC0, adcStartSingle);
}


/**
 * @brief Turn on or off the blanking feature of the CODEC
 */

int BlankCODEC(unsigned char toggle)
{
	GPIO_PinModeSet(gpioPortF, 4, gpioModePushPull, toggle);

	return(toggle);
}

/**
 * @brief Switch filter banks
 */

int SwitchFilterBanks(unsigned char toggle)
{
    GPIO_PinModeSet(gpioPortF, 5, gpioModePushPull, toggle);

	return(toggle);
}

/**
 * @brief send data out of the idac
 */

void SendIDAC(uint32_t *input_data, const uint32_t input_data_size)
{
	  // Transfer configuration and trigger selection
	  // Trigger on TIMER0 overflow and set loop count
	  LDMA_TransferCfg_t transferConfig =
	    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_TIMER0_UFOF);

	  // LDMA initialization
	  LDMA_Init_t init = LDMA_INIT_DEFAULT;
	  LDMA_Init(&init);

}

/**
* @brief Function to find the peak in the current sample window
**/
unsigned char PeakFinder(unsigned char *rawData, unsigned char rawData_window_len, uint16_t *current_peak, const uint16_t threshold, unsigned char *threshold_crossed)
{
	unsigned char i;
	uint16_t audio_sample;
	uint16_t temp_peak;
	unsigned char slope_increasing;

	temp_peak = *current_peak;
	slope_increasing = 0;

	/**
	* @brief Go thru the window
	**/
	for (i = 0; i < rawData_window_len; i += 2)
	{
		/**
		* @brief The wav file data is byte-wise and needs to be unsigned 16-bit ints
		**/
		audio_sample = rawData[i] | (uint16_t)rawData[i + 1] << 8;

		if (audio_sample > temp_peak)
		{
			temp_peak = audio_sample;
			slope_increasing = 1;
		}
		else if (audio_sample < temp_peak)
		{
			slope_increasing = 0;
		}
	}

	if (temp_peak > threshold)
	{
		*threshold_crossed = 1;
	}
	*current_peak = temp_peak;
	return(slope_increasing);
}


/**
 * @brief Function to calculate the current RMS value
 */

uint16_t rms_filter(uint16_t sample, uint16_t current_rms, uint16_t num_samples)
{
    uint16_t rms = current_rms;
    uint32_t sum_squares = 1UL * num_samples * current_rms * current_rms;

    sum_squares -= sum_squares / num_samples;
    sum_squares += (uint32_t) sample * sample;
    if (rms == 0) rms = 1;    /* do not divide by zero */
    rms = (rms + sum_squares / num_samples / rms) / 2;
    return rms;
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{

	unsigned char ret_val;
	unsigned char rawData_window_len = AUDIO_DATA_WINDOW_SIZE;
	uint16_t current_peak = 0;
	const uint16_t threshold = 2048;
	unsigned char threshold_crossed = 0;

	audioData_counter = 0;

	CHIP_Init();

	initADC();

	// Start first conversion
	ADC_Start(ADC0, adcStartSingle);

	// Init DCDC regulator with kit specific parameters
    EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;


    EMU_DCDCInit(&dcdcInit);
    // Initialization
    initIdac();

    initTimer();

    while (1)
    {
    	if(audioData_counter == AUDIO_DATA_WINDOW_LEN)
    	{
			for (audioData_counter = 0; audioData_counter < AUDIO_DATA_WINDOW_LEN; audioData_counter += rawData_window_len)
			{
				ret_val = PeakFinder(&audioData[audioData_counter], rawData_window_len, &current_peak, threshold, &threshold_crossed);

				if(threshold_crossed == 1)
				{
					BlankCODEC(1);
				}
				else if(threshold_crossed == 0)
				{
					BlankCODEC(0);
				}

				if(ret_val > current_peak)
				{
					SwitchFilterBanks(1);
				}
				else if(ret_val <= current_peak)
				{
					SwitchFilterBanks(0);
				}
			}
			audioData_counter = 0;
    	}
      //EMU_EnterEM1(); // Enter EM1 (won't exit)
    }
    return(0);
}
