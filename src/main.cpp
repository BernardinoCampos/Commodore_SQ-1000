#include <Arduino.h>

typedef	unsigned char 		BYTE;
typedef unsigned short		WORD;
typedef unsigned int		DWORD;

hw_timer_t *My_timer = NULL;

#define	CTS_PIN		10
#define RTS_PIN		11
#define RX_PIN		12
#define TX_PIN		13

struct CIRCULAR_BUFFER {
	BYTE mem [16384];
	DWORD ptrRead = 0;
	DWORD ptrWrite = 0;

	BYTE getByte() {
		WORD ptr = ptrRead & 0x3FFF;
		if (ptrRead<ptrWrite) {
			ptrRead++;
			return mem[ptr];
		}
		return 0;
	}

	BYTE putByte(BYTE byte) {
		WORD ptr = ptrWrite++ & 0x3FFF;
		mem[ptr] = byte;
	}
};

CIRCULAR_BUFFER	buffer;

void IRAM_ATTR Timer0_ISR()
{
	if (digitalRead(RTS_PIN) == HIGH) // the PC want to send data
	{
		BYTE buf[64];
		digitalWrite(CTS_PIN, HIGH);  // allow sending of data
		while (Serial.available() == 0) ;  // wait for data to come in.
		digitalWrite(CTS_PIN, LOW);  // signal to stop sending data
		int numBytes = Serial.readBytes(buf,64);
		for(int aa=0; aa<numBytes; aa++)
			buffer.putByte(buf[aa]);
	}
}

void setup() {
	Serial.begin(19200,SERIAL_8N1,RX_PIN,TX_PIN);
	My_timer = timerBegin(0, 80, true);
	timerAttachInterrupt(My_timer, &Timer0_ISR, true);
}

void loop() {
}

