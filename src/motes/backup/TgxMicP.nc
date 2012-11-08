#include "printf.h"
module TgxMicP {
	provides {
		interface Init as SoftwareInit;
		interface Read<uint16_t> as MicRead;
	}
	uses {
		interface Atm128AdcSingle;
		interface MicaBusAdc as MicAdcChannel;
		interface Resource as AdcResource;
		interface Timer<TMilli> as MicTimer;
	}
}
implementation {
	uint32_t counter = 0;
	uint16_t reading[50];
	uint16_t actualVal;

	// Tasks
	//************************************************************
	task void getMicrophoneData() {
		call Atm128AdcSingle.getData(call MicAdcChannel.getChannel(),
				 ATM128_ADC_VREF_OFF, FALSE,
				 ATM128_ADC_PRESCALE_16);
	}

	task void startNextMicphoneCapture() {
		call MicTimer.startOneShot(0);
	}

	task void notifyMicReadDone() {
		atomic {
			signal MicRead.readDone(SUCCESS, actualVal);
		}
	}
	//************************************************************

	// commands
	//************************************************************
	command error_t SoftwareInit.init() {
		atomic {
			counter = 0;
			actualVal = 0;
		}
		return SUCCESS;
	}
	
	command error_t MicRead.read() {
		atomic {
			counter = 0;
			actualVal = 0;
		}
		return call AdcResource.request();
	}
	//************************************************************



	// Events
	//************************************************************
	event void MicTimer.fired() {
		post getMicrophoneData();
	}

	event void AdcResource.granted() {
		post startNextMicphoneCapture();
	}

	async event void Atm128AdcSingle.dataReady(uint16_t data, bool precise) {
	  int i, sum, actualCount;
	  atomic {
		reading[counter] = data;
		counter++;
		printf("new counter++, counter = %ld, reading = %d, precise = %d\n", counter, data, precise);
		if (counter >= 50) {
			counter = 0;
			sum = 0;
			actualCount = 0;
			for (i = 0; i < 50; i++) {
				if (reading[i] > 300) {
					actualCount++;
					sum += reading[i];
				}
			}
			if (actualCount != 0) {
				actualVal = sum / actualCount;
				call AdcResource.release();
				post notifyMicReadDone();
			} else {
				// 说明这次采集失败, 重新采集
				// 如果采集没有成功则不释放resource， 继续采集
				printf("capture failure:counter: %ld\n", counter);
				post startNextMicphoneCapture();
			}
		} else {
			printf("capture next:counter: %ld\n", counter);
			post startNextMicphoneCapture();
		}
	  }
	}
	//************************************************************
}
