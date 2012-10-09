#include "printf.h"
module TestMicrophoneC {
	uses {
		interface Resource as AdcResource;
		interface Leds;
		interface Atm128AdcSingle;
		interface MicaBusAdc as MicAdcChannel;
		interface SplitControl as Microphone;
		interface StdControl as PrintfControl;
		interface Boot;

		interface SplitControl as RadioControl;
	}
}
implementation {
	uint32_t counter = 0;
	uint16_t reading[5];
	void reportMicWorking() {
		call Leds.led1Toggle();
	}

	task void getMicrophoneData() {
		call Atm128AdcSingle.getData(call MicAdcChannel.getChannel(),
				 ATM128_ADC_VREF_OFF, FALSE,
				 ATM128_ADC_PRESCALE_16);
	}

	event void Boot.booted() {
		call RadioControl.start();
		call Microphone.start();
		atomic {
			counter = 0;
		}
	}

	event void RadioControl.startDone(error_t error) {
		call PrintfControl.start();
	}
	
	event void RadioControl.stopDone(error_t error) {

	}

	event void Microphone.startDone(error_t error) {
		call AdcResource.request();
	}

	event void AdcResource.granted() {
		post getMicrophoneData();
	}

	async event void Atm128AdcSingle.dataReady(uint16_t data, bool precise) {
	  int i, sum, actualCount;
	  reportMicWorking();
	  atomic {
		counter++;
		reading[counter] = data;
		if (counter == 5) {
			counter = 0;
			sum = 0;
			actualCount = 0;
			for (i = 0; i < 5; i++) {
				if (reading[i] > 400) {
					actualCount++;
					sum += reading[i];
				}
			}
			if (actualCount != 0)
				printf("%d\n", sum / actualCount);
		}
	  }
	  post getMicrophoneData();
	}

	event void Microphone.stopDone(error_t error) {

	}
}
