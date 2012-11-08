#include "printf.h"
#include "tgx.h"

module TgxC {
	uses {
		interface Boot;
		interface Leds;
		interface Random;
		interface BigQueue<sensor_msg_t> as SerialMsgQueue;
		interface Timer<TMilli> as SensorReadTimer;
		interface Timer<TMilli> as BaseStationTimer;
		interface Timer<TMilli> as CheckDogTimer;

		interface AMSend as SerialSend;
		interface Packet as SerialPacket;
		/*interface Receive as SerialReceive;*/
		interface SplitControl as SerialControl;

		interface Send as CollectionSend;
		interface Receive as CollectionReceive;
		interface RootControl;
		interface StdControl as CollectionControl;
		interface CtpInfo as CollectionInfo;
		interface SplitControl as RadioControl;
		
		/* 用于控制节点的分发协议 */
		/*interface StdControl as DisseminationControl;*/
		/*interface DisseminationUpdate;*/
		/*interface DisseminationValue;*/

		interface Read<uint16_t> as Temp;
		interface Read<uint16_t> as Light;
		interface Read<uint16_t> as Microphone;
		interface SplitControl as MicControl;
		interface Read<uint16_t> as AccelX;
		interface Read<uint16_t> as AccelY;
		interface Read<uint16_t> as MagX;
		interface Read<uint16_t> as MagY;
	}
}
implementation {
	/** 检查节点是否为簇首(汇聚节点) */
	bool bRoot = FALSE;
	/** 检查无线信道以及串口信道是否忙 */
	bool radioBusy = FALSE;
	bool serialBusy = FALSE;

	/** 两种消息包
	  * 1) 采集的数据的消息包,
	  * 2) 将要发往串口的包，
	  */
	message_t m_sensor;
	message_t m_serial;

	/** 节点的坐标会在Boot启动后生成，不会再发生变化 */
	uint16_t positionX, positionY;
	/** 用来盛放采集的数据的临时变量 */
	uint16_t Temp_data, Light_data, Microphone_data, AccelX_data,
			 AccelY_data, MagX_data, MagY_data;
		
	/*  辅助函数 */
	/* ***************************************** */
	/** 用来报告节点的各个部分的工作情况 */
	void reportSensorWorking() {
		call Leds.led0Toggle();
	}
	void reportRadioWorking() {
		call Leds.led1Toggle();
	}
	void reportSerialWorking() {
		call Leds.led2Toggle();
	}
	void reportFatalError() {
		call Leds.led0On();
		call Leds.led1On();
		call Leds.led2On();
	}


	/* ****************************************** */
	// Task
	// 一共在三个地方post Task（在基站节点）
	// 1. 节点启动串口post一次
	// 2. CheckDogTimer 看门狗的时钟里面会post保证不中断
	// 3. 在Serial.sendDone中
	// 注意看门狗不是用startPeriod开启的， 而是在task sendSerialMsg
	// 中用startOneShot开启的

	task void sendSerialMsg() {
		int sizeQueue = call SerialMsgQueue.size();
		if (bRoot && !serialBusy && sizeQueue != 0) {
			sensor_msg_t sensorData = call SerialMsgQueue.dequeue();
			
			sensor_msg_t *serialData = (sensor_msg_t *)(
					call SerialSend.getPayload(&m_serial, sizeof(sensor_msg_t)));
			*serialData = sensorData;
			if (call SerialSend.send(AM_BROADCAST_ADDR, &m_serial, sizeof(*serialData)) == SUCCESS) {
				serialBusy = TRUE;
				reportSerialWorking();
			}
		}
		// 如果退出循环那么说明队列很可能空了， 或者还不能发送， 
		// 但是我们又不想让这个中断， 那么我们开启一个时钟避让一下
		call CheckDogTimer.startOneShot(DEFAULT_CHECKDOG_CHECK_PERIOD);
	}

	/* ****************************************** */
	event void MicControl.startDone(error_t error) {
		if (!bRoot) {
				/*call Temp.read();*/
				call SensorReadTimer.startOneShot(DEFAULT_SENSOR_PERIOD);
		}
	}

	event void MicControl.stopDone(error_t error) {

	}

	event void Boot.booted() {
		// 根据SENSOR_RANGE_MIN - SENSOR_RANGE_MAX, 随即生成节点的位置信息,
		// 后期我们会根据GPRS信息， 进行定位, 得到真实的节点位置信息
		positionX = ((call Random.rand16() / (65536 + 0.0)) *
						(SENSOR_RANGE_MAX_X -  SENSOR_RANGE_MIN_X)) + SENSOR_RANGE_MIN_X;			  
		positionY = ((call Random.rand16() / (65536 + 0.0)) *
						(SENSOR_RANGE_MAX_Y -  SENSOR_RANGE_MIN_Y)) + SENSOR_RANGE_MIN_Y;

		if (TOS_NODE_ID == 0) {
			bRoot = TRUE;
			call SerialControl.start();
		} else {
			call MicControl.start();
		}

		call RadioControl.start();
	}	

	event void BaseStationTimer.fired() {
		am_addr_t parentId;
		if (bRoot) {
			sensor_msg_t baseStationData;
			baseStationData.nodeId = TOS_NODE_ID;
			call CollectionInfo.getParent(&parentId);
			baseStationData.parentId = parentId;
			baseStationData.position.x = positionX;
			baseStationData.position.y = positionY;
			baseStationData.sensor.temp  = 0;
			baseStationData.sensor.photo = 0;
			baseStationData.sensor.sound = 0;
			baseStationData.sensor.x_acc = 0;
			baseStationData.sensor.y_acc = 0;
			baseStationData.sensor.x_mag = 0;
			baseStationData.sensor.y_mag = 0;

			call SerialMsgQueue.enqueue(baseStationData);
		}
	}

	event void CheckDogTimer.fired()
	{
		post sendSerialMsg();
	}

	event void RadioControl.startDone(error_t error) {
		if (error == SUCCESS) {
			if (call CollectionControl.start() != SUCCESS) {
				reportFatalError();
			}
			if (bRoot) {
				call RootControl.setRoot();
			}
			/*call DisseminationControl.start();*/
			if (!bRoot) {
				/*call Temp.read();*/
				/*call SensorReadTimer.startOneShot(100);*/
			}
		} else {
			call RadioControl.start();
		}
	}

	// 如果发送出现了问题， 我们可以通过设置一个定时器， 保证链式采集不中断
	event void SensorReadTimer.fired() {
		call Temp.read();
	}

	event void SerialControl.startDone(error_t error) {
		call BaseStationTimer.startPeriodic(DEFAULT_BASESTATION_REPORT_PERIOD);
		post sendSerialMsg();
	}
	event void SerialControl.stopDone(error_t error) { }
	event void RadioControl.stopDone(error_t error) { }

	// 采集按照链式方法， 保证了数据采集完后发送
	event void Temp.readDone(error_t error, uint16_t val) {
		if (error == SUCCESS)
			Temp_data = val;
		else
			Temp_data = 0;
		call Light.read();
	}
	event void Light.readDone(error_t error, uint16_t val) {
		if (error == SUCCESS)
			Light_data = val;
		else
			Light_data = 0;
		call Microphone.read();
	}
	event void Microphone.readDone(error_t error, uint16_t val) {
		if (error == SUCCESS)
			Microphone_data = val;
		else
			Microphone_data = 0;
		call AccelX.read();
	}
	event void AccelX.readDone(error_t error, uint16_t val) {
		if (error == SUCCESS)
			AccelX_data = val;
		else
			AccelX_data = 0;
		call AccelY.read();
	}
	event void AccelY.readDone(error_t error, uint16_t val) {
		if (error == SUCCESS)
			AccelY_data = val;
		else
			AccelY_data = 0;
		call MagX.read();
	}
	event void MagX.readDone(error_t error, uint16_t val) {
		if (error == SUCCESS)
			MagX_data = val;
		else
			MagX_data = 0;
		call MagY.read();
	}

	// 在链式采集的尾部我们发送采集的数据
	event void MagY.readDone(error_t error, uint16_t val) {
		uint16_t parentId = 0;

		// 从message_t 获取payload
		sensor_msg_t *sensorData = (sensor_msg_t *)(
				call CollectionSend.getPayload(&m_sensor, sizeof(sensor_msg_t)));

		/** 将采集到的数据以及一些节点的本身信息填写到sensorData上面 */
		/** ******************************************************* */
		if (error != SUCCESS)
			val = 0;

		sensorData->nodeId = TOS_NODE_ID;
		call CollectionInfo.getParent(&parentId);
		sensorData->parentId = parentId;
		sensorData->position.x = positionX;
		sensorData->position.y = positionY;
		sensorData->sensor.temp = Temp_data;
		sensorData->sensor.photo = Light_data;
		sensorData->sensor.sound = Microphone_data;
		sensorData->sensor.x_acc = AccelX_data;
		sensorData->sensor.y_acc = AccelY_data;
		sensorData->sensor.x_mag = MagX_data;
		sensorData->sensor.y_mag = val;

		// 每采集完一组数据， 填写到payload之后， 闪led， 报告正在工作
		reportSensorWorking();
		/** ******************************************************* */

		/** 不出意外就可以发送了 */
		if (!bRoot && !radioBusy) {
			if (call CollectionSend.send(&m_sensor, sizeof(sensor_msg_t)) == SUCCESS) {
				radioBusy = TRUE;
				reportRadioWorking();
			} else {
				reportFatalError();
			}
		}
	}

	event void CollectionSend.sendDone(message_t *msg, error_t error) {
		if (msg == &m_sensor) {
			radioBusy = FALSE;
			/*call Temp.read(); // 开启下一次采集，注意这里将采集顺序通过Split-Phase*/
							    // 机制串起来了，解决了并发带来的问题
			call SensorReadTimer.startOneShot(DEFAULT_SENSOR_PERIOD);
		}
	}

	event message_t *CollectionReceive.receive(message_t *msg, void *payload, uint8_t len) {
		// 这个是通过Collection-tree协议从其它节点收集来的采集信息
		sensor_msg_t *newCollectionMsg = payload;
		reportRadioWorking();
		/* 检查包是否合法，并且是否为汇聚节点 */
		if (len == sizeof(sensor_msg_t) && bRoot) {
			/* 我们将message换一个马甲,重新组装准备发往串口
			 * 这边我还不太清楚不换马夹行不行， 不过貌似tinyos
			 * 底层协议栈对于同类型的message，只有一块buffer，
			 * 这就是为什么我们在event最后需要return msg的原因
			 * 如果不换马夹， 那么一块buffer既要接收从其它节点
			 * 来的message， 还要向串口发数据， 压力太大 */
//			sensor_msg_t *serialData = (sensor_msg_t *)(
//					call SerialSend.getPayload(&m_serial, sizeof(sensor_msg_t)));

			// 两种写法
			/* serialData = *((sensor_msg_t *)payload);*/
//			*serialData = *newCollectionMsg;
//			if (call SerialSend.send(AM_BROADCAST_ADDR, &m_serial, sizeof(*serialData)) == SUCCESS) {
//				serialBusy = TRUE;
//				reportSerialWorking();

				// printf调试信息
				/*atomic {
					printf("the packetlength: %d, the max packetlength: %d 
					          the length of the my_octopus_msg: %d\n", 
							  call SerialPacket.payloadLength(&serial_msg), 
							  call SerialPacket.maxPayloadLength(), 
					          sizeof(myoctopus_sensor_msg));
					printfflush();
				}*/
			call SerialMsgQueue.enqueue(*newCollectionMsg);
//			printf("tempeature = %d\n", newCollectionMsg->sensor.temp);
//			printfflush();
		} else {
			reportFatalError();
		}
		return msg;
	}
	event void SerialSend.sendDone(message_t *msg, error_t error) {
		if (msg == &m_serial) {
			serialBusy = FALSE;
			post sendSerialMsg();
		}
	}
}
