
C:\Users\agharagyozya\AppData\Local\Temp\arduino_build_525691\test.ino.elf:     file format elf32-avr


Disassembly of section .text:

00000000 <__vectors>:
   0:	0c 94 52 00 	jmp	0xa4	; 0xa4 <__ctors_end>
   4:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
   8:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
   c:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  10:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  14:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  18:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  1c:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  20:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  24:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  28:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  2c:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  30:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  34:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  38:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  3c:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  40:	0c 94 c3 00 	jmp	0x186	; 0x186 <__vector_16>
  44:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  48:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  4c:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  50:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  54:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  58:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  5c:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  60:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>
  64:	0c 94 64 00 	jmp	0xc8	; 0xc8 <__bad_interrupt>

00000068 <__trampolines_end>:
  68:	00 00       	nop
  6a:	00 00       	nop
  6c:	25 00       	.word	0x0025	; ????
  6e:	28 00       	.word	0x0028	; ????
  70:	2b 00       	.word	0x002b	; ????

00000072 <port_to_mode_PGM>:
  72:	00 00 00 00 24 00 27 00 2a 00                       ....$.'.*.

0000007c <digital_pin_to_port_PGM>:
  7c:	04 04 04 04 04 04 04 04 02 02 02 02 02 02 03 03     ................
  8c:	03 03 03 03                                         ....

00000090 <digital_pin_to_bit_mask_PGM>:
  90:	01 02 04 08 10 20 40 80 01 02 04 08 10 20 01 02     ..... @...... ..
  a0:	04 08 10 20                                         ... 

000000a4 <__ctors_end>:
  a4:	11 24       	eor	r1, r1
  a6:	1f be       	out	0x3f, r1	; 63
  a8:	cf ef       	ldi	r28, 0xFF	; 255
  aa:	d8 e0       	ldi	r29, 0x08	; 8
  ac:	de bf       	out	0x3e, r29	; 62
  ae:	cd bf       	out	0x3d, r28	; 61

000000b0 <__do_clear_bss>:
  b0:	21 e0       	ldi	r18, 0x01	; 1
  b2:	a0 e0       	ldi	r26, 0x00	; 0
  b4:	b1 e0       	ldi	r27, 0x01	; 1
  b6:	01 c0       	rjmp	.+2      	; 0xba <.do_clear_bss_start>

000000b8 <.do_clear_bss_loop>:
  b8:	1d 92       	st	X+, r1

000000ba <.do_clear_bss_start>:
  ba:	a9 30       	cpi	r26, 0x09	; 9
  bc:	b2 07       	cpc	r27, r18
  be:	e1 f7       	brne	.-8      	; 0xb8 <.do_clear_bss_loop>
  c0:	0e 94 0d 01 	call	0x21a	; 0x21a <main>
  c4:	0c 94 79 01 	jmp	0x2f2	; 0x2f2 <_exit>

000000c8 <__bad_interrupt>:
  c8:	0c 94 00 00 	jmp	0	; 0x0 <__vectors>

000000cc <micros>:
	return m;
}

unsigned long micros() {
	unsigned long m;
	uint8_t oldSREG = SREG, t;
  cc:	3f b7       	in	r19, 0x3f	; 63
	
	cli();
  ce:	f8 94       	cli
	m = timer0_overflow_count;
  d0:	80 91 05 01 	lds	r24, 0x0105
  d4:	90 91 06 01 	lds	r25, 0x0106
  d8:	a0 91 07 01 	lds	r26, 0x0107
  dc:	b0 91 08 01 	lds	r27, 0x0108
#if defined(TCNT0)
	t = TCNT0;
  e0:	26 b5       	in	r18, 0x26	; 38
#else
	#error TIMER 0 not defined
#endif

#ifdef TIFR0
	if ((TIFR0 & _BV(TOV0)) && (t < 255))
  e2:	a8 9b       	sbis	0x15, 0	; 21
  e4:	05 c0       	rjmp	.+10     	; 0xf0 <micros+0x24>
  e6:	2f 3f       	cpi	r18, 0xFF	; 255
  e8:	19 f0       	breq	.+6      	; 0xf0 <micros+0x24>
		m++;
  ea:	01 96       	adiw	r24, 0x01	; 1
  ec:	a1 1d       	adc	r26, r1
  ee:	b1 1d       	adc	r27, r1
#else
	if ((TIFR & _BV(TOV0)) && (t < 255))
		m++;
#endif

	SREG = oldSREG;
  f0:	3f bf       	out	0x3f, r19	; 63
	
	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
  f2:	ba 2f       	mov	r27, r26
  f4:	a9 2f       	mov	r26, r25
  f6:	98 2f       	mov	r25, r24
  f8:	88 27       	eor	r24, r24
  fa:	82 0f       	add	r24, r18
  fc:	91 1d       	adc	r25, r1
  fe:	a1 1d       	adc	r26, r1
 100:	b1 1d       	adc	r27, r1
 102:	bc 01       	movw	r22, r24
 104:	cd 01       	movw	r24, r26
 106:	42 e0       	ldi	r20, 0x02	; 2
 108:	66 0f       	add	r22, r22
 10a:	77 1f       	adc	r23, r23
 10c:	88 1f       	adc	r24, r24
 10e:	99 1f       	adc	r25, r25
 110:	4a 95       	dec	r20
 112:	d1 f7       	brne	.-12     	; 0x108 <micros+0x3c>
}
 114:	08 95       	ret

00000116 <delay.constprop.0>:

void delay(unsigned long ms)
 116:	8f 92       	push	r8
 118:	9f 92       	push	r9
 11a:	af 92       	push	r10
 11c:	bf 92       	push	r11
 11e:	cf 92       	push	r12
 120:	df 92       	push	r13
 122:	ef 92       	push	r14
 124:	ff 92       	push	r15
{
	uint32_t start = micros();
 126:	0e 94 66 00 	call	0xcc	; 0xcc <micros>
 12a:	4b 01       	movw	r8, r22
 12c:	5c 01       	movw	r10, r24
 12e:	88 ee       	ldi	r24, 0xE8	; 232
 130:	c8 2e       	mov	r12, r24
 132:	83 e0       	ldi	r24, 0x03	; 3
 134:	d8 2e       	mov	r13, r24
 136:	e1 2c       	mov	r14, r1
 138:	f1 2c       	mov	r15, r1

	while (ms > 0) {
		yield();
		while ( ms > 0 && (micros() - start) >= 1000) {
 13a:	0e 94 66 00 	call	0xcc	; 0xcc <micros>
 13e:	dc 01       	movw	r26, r24
 140:	cb 01       	movw	r24, r22
 142:	88 19       	sub	r24, r8
 144:	99 09       	sbc	r25, r9
 146:	aa 09       	sbc	r26, r10
 148:	bb 09       	sbc	r27, r11
 14a:	88 3e       	cpi	r24, 0xE8	; 232
 14c:	93 40       	sbci	r25, 0x03	; 3
 14e:	a1 05       	cpc	r26, r1
 150:	b1 05       	cpc	r27, r1
 152:	98 f3       	brcs	.-26     	; 0x13a <delay.constprop.0+0x24>
			ms--;
 154:	21 e0       	ldi	r18, 0x01	; 1
 156:	c2 1a       	sub	r12, r18
 158:	d1 08       	sbc	r13, r1
 15a:	e1 08       	sbc	r14, r1
 15c:	f1 08       	sbc	r15, r1
			start += 1000;
 15e:	88 ee       	ldi	r24, 0xE8	; 232
 160:	88 0e       	add	r8, r24
 162:	83 e0       	ldi	r24, 0x03	; 3
 164:	98 1e       	adc	r9, r24
 166:	a1 1c       	adc	r10, r1
 168:	b1 1c       	adc	r11, r1
{
	uint32_t start = micros();

	while (ms > 0) {
		yield();
		while ( ms > 0 && (micros() - start) >= 1000) {
 16a:	c1 14       	cp	r12, r1
 16c:	d1 04       	cpc	r13, r1
 16e:	e1 04       	cpc	r14, r1
 170:	f1 04       	cpc	r15, r1
 172:	19 f7       	brne	.-58     	; 0x13a <delay.constprop.0+0x24>
			ms--;
			start += 1000;
		}
	}
}
 174:	ff 90       	pop	r15
 176:	ef 90       	pop	r14
 178:	df 90       	pop	r13
 17a:	cf 90       	pop	r12
 17c:	bf 90       	pop	r11
 17e:	af 90       	pop	r10
 180:	9f 90       	pop	r9
 182:	8f 90       	pop	r8
 184:	08 95       	ret

00000186 <__vector_16>:
#if defined(TIM0_OVF_vect)
ISR(TIM0_OVF_vect)
#else
ISR(TIMER0_OVF_vect)
#endif
{
 186:	1f 92       	push	r1
 188:	0f 92       	push	r0
 18a:	0f b6       	in	r0, 0x3f	; 63
 18c:	0f 92       	push	r0
 18e:	11 24       	eor	r1, r1
 190:	2f 93       	push	r18
 192:	3f 93       	push	r19
 194:	8f 93       	push	r24
 196:	9f 93       	push	r25
 198:	af 93       	push	r26
 19a:	bf 93       	push	r27
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
 19c:	80 91 01 01 	lds	r24, 0x0101
 1a0:	90 91 02 01 	lds	r25, 0x0102
 1a4:	a0 91 03 01 	lds	r26, 0x0103
 1a8:	b0 91 04 01 	lds	r27, 0x0104
	unsigned char f = timer0_fract;
 1ac:	30 91 00 01 	lds	r19, 0x0100

	m += MILLIS_INC;
	f += FRACT_INC;
 1b0:	23 e0       	ldi	r18, 0x03	; 3
 1b2:	23 0f       	add	r18, r19
	if (f >= FRACT_MAX) {
 1b4:	2d 37       	cpi	r18, 0x7D	; 125
 1b6:	20 f4       	brcc	.+8      	; 0x1c0 <__vector_16+0x3a>
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;

	m += MILLIS_INC;
 1b8:	01 96       	adiw	r24, 0x01	; 1
 1ba:	a1 1d       	adc	r26, r1
 1bc:	b1 1d       	adc	r27, r1
 1be:	05 c0       	rjmp	.+10     	; 0x1ca <__vector_16+0x44>
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
 1c0:	26 e8       	ldi	r18, 0x86	; 134
 1c2:	23 0f       	add	r18, r19
		m += 1;
 1c4:	02 96       	adiw	r24, 0x02	; 2
 1c6:	a1 1d       	adc	r26, r1
 1c8:	b1 1d       	adc	r27, r1
	}

	timer0_fract = f;
 1ca:	20 93 00 01 	sts	0x0100, r18
	timer0_millis = m;
 1ce:	80 93 01 01 	sts	0x0101, r24
 1d2:	90 93 02 01 	sts	0x0102, r25
 1d6:	a0 93 03 01 	sts	0x0103, r26
 1da:	b0 93 04 01 	sts	0x0104, r27
	timer0_overflow_count++;
 1de:	80 91 05 01 	lds	r24, 0x0105
 1e2:	90 91 06 01 	lds	r25, 0x0106
 1e6:	a0 91 07 01 	lds	r26, 0x0107
 1ea:	b0 91 08 01 	lds	r27, 0x0108
 1ee:	01 96       	adiw	r24, 0x01	; 1
 1f0:	a1 1d       	adc	r26, r1
 1f2:	b1 1d       	adc	r27, r1
 1f4:	80 93 05 01 	sts	0x0105, r24
 1f8:	90 93 06 01 	sts	0x0106, r25
 1fc:	a0 93 07 01 	sts	0x0107, r26
 200:	b0 93 08 01 	sts	0x0108, r27
}
 204:	bf 91       	pop	r27
 206:	af 91       	pop	r26
 208:	9f 91       	pop	r25
 20a:	8f 91       	pop	r24
 20c:	3f 91       	pop	r19
 20e:	2f 91       	pop	r18
 210:	0f 90       	pop	r0
 212:	0f be       	out	0x3f, r0	; 63
 214:	0f 90       	pop	r0
 216:	1f 90       	pop	r1
 218:	18 95       	reti

0000021a <main>:

void init()
{
	// this needs to be called before setup() or some functions won't
	// work there
	sei();
 21a:	78 94       	sei
	
	// on the ATmega168, timer 0 is also used for fast hardware pwm
	// (using phase-correct PWM would mean that timer 0 overflowed half as often
	// resulting in different millis() behavior on the ATmega8 and ATmega168)
#if defined(TCCR0A) && defined(WGM01)
	sbi(TCCR0A, WGM01);
 21c:	84 b5       	in	r24, 0x24	; 36
 21e:	82 60       	ori	r24, 0x02	; 2
 220:	84 bd       	out	0x24, r24	; 36
	sbi(TCCR0A, WGM00);
 222:	84 b5       	in	r24, 0x24	; 36
 224:	81 60       	ori	r24, 0x01	; 1
 226:	84 bd       	out	0x24, r24	; 36
	// this combination is for the standard atmega8
	sbi(TCCR0, CS01);
	sbi(TCCR0, CS00);
#elif defined(TCCR0B) && defined(CS01) && defined(CS00)
	// this combination is for the standard 168/328/1280/2560
	sbi(TCCR0B, CS01);
 228:	85 b5       	in	r24, 0x25	; 37
 22a:	82 60       	ori	r24, 0x02	; 2
 22c:	85 bd       	out	0x25, r24	; 37
	sbi(TCCR0B, CS00);
 22e:	85 b5       	in	r24, 0x25	; 37
 230:	81 60       	ori	r24, 0x01	; 1
 232:	85 bd       	out	0x25, r24	; 37

	// enable timer 0 overflow interrupt
#if defined(TIMSK) && defined(TOIE0)
	sbi(TIMSK, TOIE0);
#elif defined(TIMSK0) && defined(TOIE0)
	sbi(TIMSK0, TOIE0);
 234:	80 91 6e 00 	lds	r24, 0x006E
 238:	81 60       	ori	r24, 0x01	; 1
 23a:	80 93 6e 00 	sts	0x006E, r24
	// this is better for motors as it ensures an even waveform
	// note, however, that fast pwm mode can achieve a frequency of up
	// 8 MHz (with a 16 MHz clock) at 50% duty cycle

#if defined(TCCR1B) && defined(CS11) && defined(CS10)
	TCCR1B = 0;
 23e:	10 92 81 00 	sts	0x0081, r1

	// set timer 1 prescale factor to 64
	sbi(TCCR1B, CS11);
 242:	80 91 81 00 	lds	r24, 0x0081
 246:	82 60       	ori	r24, 0x02	; 2
 248:	80 93 81 00 	sts	0x0081, r24
#if F_CPU >= 8000000L
	sbi(TCCR1B, CS10);
 24c:	80 91 81 00 	lds	r24, 0x0081
 250:	81 60       	ori	r24, 0x01	; 1
 252:	80 93 81 00 	sts	0x0081, r24
	sbi(TCCR1, CS10);
#endif
#endif
	// put timer 1 in 8-bit phase correct pwm mode
#if defined(TCCR1A) && defined(WGM10)
	sbi(TCCR1A, WGM10);
 256:	80 91 80 00 	lds	r24, 0x0080
 25a:	81 60       	ori	r24, 0x01	; 1
 25c:	80 93 80 00 	sts	0x0080, r24

	// set timer 2 prescale factor to 64
#if defined(TCCR2) && defined(CS22)
	sbi(TCCR2, CS22);
#elif defined(TCCR2B) && defined(CS22)
	sbi(TCCR2B, CS22);
 260:	80 91 b1 00 	lds	r24, 0x00B1
 264:	84 60       	ori	r24, 0x04	; 4
 266:	80 93 b1 00 	sts	0x00B1, r24

	// configure timer 2 for phase correct pwm (8-bit)
#if defined(TCCR2) && defined(WGM20)
	sbi(TCCR2, WGM20);
#elif defined(TCCR2A) && defined(WGM20)
	sbi(TCCR2A, WGM20);
 26a:	80 91 b0 00 	lds	r24, 0x00B0
 26e:	81 60       	ori	r24, 0x01	; 1
 270:	80 93 b0 00 	sts	0x00B0, r24
#endif

#if defined(ADCSRA)
	// set a2d prescaler so we are inside the desired 50-200 KHz range.
	#if F_CPU >= 16000000 // 16 MHz / 128 = 125 KHz
		sbi(ADCSRA, ADPS2);
 274:	80 91 7a 00 	lds	r24, 0x007A
 278:	84 60       	ori	r24, 0x04	; 4
 27a:	80 93 7a 00 	sts	0x007A, r24
		sbi(ADCSRA, ADPS1);
 27e:	80 91 7a 00 	lds	r24, 0x007A
 282:	82 60       	ori	r24, 0x02	; 2
 284:	80 93 7a 00 	sts	0x007A, r24
		sbi(ADCSRA, ADPS0);
 288:	80 91 7a 00 	lds	r24, 0x007A
 28c:	81 60       	ori	r24, 0x01	; 1
 28e:	80 93 7a 00 	sts	0x007A, r24
		cbi(ADCSRA, ADPS2);
		cbi(ADCSRA, ADPS1);
		sbi(ADCSRA, ADPS0);
	#endif
	// enable a2d conversions
	sbi(ADCSRA, ADEN);
 292:	80 91 7a 00 	lds	r24, 0x007A
 296:	80 68       	ori	r24, 0x80	; 128
 298:	80 93 7a 00 	sts	0x007A, r24
	// here so they can be used as normal digital i/o; they will be
	// reconnected in Serial.begin()
#if defined(UCSRB)
	UCSRB = 0;
#elif defined(UCSR0B)
	UCSR0B = 0;
 29c:	10 92 c1 00 	sts	0x00C1, r1
#include "wiring_private.h"
#include "pins_arduino.h"

void pinMode(uint8_t pin, uint8_t mode)
{
	uint8_t bit = digitalPinToBitMask(pin);
 2a0:	ed e9       	ldi	r30, 0x9D	; 157
 2a2:	f0 e0       	ldi	r31, 0x00	; 0
 2a4:	24 91       	lpm	r18, Z+
	uint8_t port = digitalPinToPort(pin);
 2a6:	e9 e8       	ldi	r30, 0x89	; 137
 2a8:	f0 e0       	ldi	r31, 0x00	; 0
 2aa:	84 91       	lpm	r24, Z+
	volatile uint8_t *reg, *out;

	if (port == NOT_A_PIN) return;
 2ac:	88 23       	and	r24, r24
 2ae:	99 f0       	breq	.+38     	; 0x2d6 <main+0xbc>

	// JWS: can I let the optimizer do this?
	reg = portModeRegister(port);
 2b0:	90 e0       	ldi	r25, 0x00	; 0
 2b2:	88 0f       	add	r24, r24
 2b4:	99 1f       	adc	r25, r25
 2b6:	fc 01       	movw	r30, r24
 2b8:	ee 58       	subi	r30, 0x8E	; 142
 2ba:	ff 4f       	sbci	r31, 0xFF	; 255
 2bc:	a5 91       	lpm	r26, Z+
 2be:	b4 91       	lpm	r27, Z+
	out = portOutputRegister(port);
 2c0:	fc 01       	movw	r30, r24
 2c2:	e8 59       	subi	r30, 0x98	; 152
 2c4:	ff 4f       	sbci	r31, 0xFF	; 255
 2c6:	85 91       	lpm	r24, Z+
 2c8:	94 91       	lpm	r25, Z+
                cli();
		*reg &= ~bit;
		*out |= bit;
		SREG = oldSREG;
	} else {
		uint8_t oldSREG = SREG;
 2ca:	8f b7       	in	r24, 0x3f	; 63
                cli();
 2cc:	f8 94       	cli
		*reg |= bit;
 2ce:	ec 91       	ld	r30, X
 2d0:	e2 2b       	or	r30, r18
 2d2:	ec 93       	st	X, r30
		SREG = oldSREG;
 2d4:	8f bf       	out	0x3f, r24	; 63
	
	setup();
    
	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
 2d6:	c0 e0       	ldi	r28, 0x00	; 0
 2d8:	d0 e0       	ldi	r29, 0x00	; 0
  pinMode(13, OUTPUT);
}

void loop()
{
  PORTB = 0xff;
 2da:	1f ef       	ldi	r17, 0xFF	; 255
 2dc:	15 b9       	out	0x05, r17	; 5
  delay(1000);
 2de:	0e 94 8b 00 	call	0x116	; 0x116 <delay.constprop.0>
  PORTB = 0x00;
 2e2:	15 b8       	out	0x05, r1	; 5
  delay(1000);
 2e4:	0e 94 8b 00 	call	0x116	; 0x116 <delay.constprop.0>
 2e8:	20 97       	sbiw	r28, 0x00	; 0
 2ea:	c1 f3       	breq	.-16     	; 0x2dc <main+0xc2>
 2ec:	0e 94 00 00 	call	0	; 0x0 <__vectors>
 2f0:	f5 cf       	rjmp	.-22     	; 0x2dc <main+0xc2>

000002f2 <_exit>:
 2f2:	f8 94       	cli

000002f4 <__stop_program>:
 2f4:	ff cf       	rjmp	.-2      	; 0x2f4 <__stop_program>
