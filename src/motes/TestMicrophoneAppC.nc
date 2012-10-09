#include "printf.h"
configuration TestMicrophoneAppC {
}
implementation {

	// Base module
	components MainC, LedsC;

	// platform module
	components MicaBusC, Atm128AdcC, new Atm128I2CMasterC() as I2CPot;

	// MicrophoneC module
	components MicrophoneC, new TimerMilliC() as MicWarnupTimerC;
	MicrophoneC.MicWarnupTimer -> MicWarnupTimerC;
	MicrophoneC.MicPower -> MicaBusC.PW3;
	MicrophoneC.MicMuxSel -> MicaBusC.PW6;
	MicrophoneC.I2CResource -> I2CPot;
	MicrophoneC.I2CPacket -> I2CPot;

	// Main module
	components TestMicrophoneC as App;
	App.Boot -> MainC;
	App.Leds -> LedsC;
	App.Microphone -> MicrophoneC;
	App.AdcResource -> Atm128AdcC.Resource[unique(UQ_ATM128ADC_RESOURCE)];
	App.Atm128AdcSingle -> Atm128AdcC;
	App.MicAdcChannel -> MicaBusC.Adc2;

	// DEBUG by radio
	components PrintfC;
	components ActiveMessageC as Radio;

	App.PrintfControl -> PrintfC;
	App.RadioControl -> Radio;
}
