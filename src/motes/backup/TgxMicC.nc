configuration TgxMicC {
	provides {
		interface Read<uint16_t>;
		interface SplitControl;
	}
}
implementation {
	components MicaBusC, Atm128AdcC, new Atm128I2CMasterC() as I2CPot;
	components MicrophoneC, new TimerMilliC() as MicWarnupTimerC;
	components TgxMicP as MicReader;
	components new TimerMilliC() as MicTimerC;

	MicrophoneC.MicWarnupTimer -> MicWarnupTimerC;
	MicrophoneC.MicPower -> MicaBusC.PW3;
	MicrophoneC.MicMuxSel -> MicaBusC.PW6;
	MicrophoneC.I2CResource -> I2CPot;
	MicrophoneC.I2CPacket -> I2CPot;

	MicReader.AdcResource -> Atm128AdcC.Resource[unique(UQ_ATM128ADC_RESOURCE)];
	MicReader.Atm128AdcSingle -> Atm128AdcC;
	MicReader.MicAdcChannel -> MicaBusC.Adc2;
	MicReader.MicTimer -> MicTimerC;

	components MainC;
	MainC.SoftwareInit -> MicReader;

	Read = MicReader.MicRead;
	SplitControl = MicrophoneC;
}
