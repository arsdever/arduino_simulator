
memory.elf:     file format elf32-avr


Disassembly of section .text:

00000000 <__vectors>:
   0:	0c 94 34 00 	jmp	0x68	; 0x68 <__ctors_end>
   4:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
   8:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
   c:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  10:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  14:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  18:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  1c:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  20:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  24:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  28:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  2c:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  30:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  34:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  38:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  3c:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  40:	0c 94 a4 00 	jmp	0x148	; 0x148 <__vector_16>
  44:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  48:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  4c:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  50:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  54:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  58:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  5c:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  60:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>
  64:	0c 94 46 00 	jmp	0x8c	; 0x8c <__bad_interrupt>

00000068 <__ctors_end>:
  68:	11 24       	eor	r1, r1
  6a:	1f be       	out	0x3f, r1	; 63
  6c:	cf ef       	ldi	r28, 0xFF	; 255
  6e:	d8 e0       	ldi	r29, 0x08	; 8
  70:	de bf       	out	0x3e, r29	; 62
  72:	cd bf       	out	0x3d, r28	; 61

00000074 <__do_clear_bss>:
  74:	21 e0       	ldi	r18, 0x01	; 1
  76:	a0 e0       	ldi	r26, 0x00	; 0
  78:	b1 e0       	ldi	r27, 0x01	; 1
  7a:	01 c0       	rjmp	.+2      	; 0x7e <.do_clear_bss_start>

0000007c <.do_clear_bss_loop>:
  7c:	1d 92       	st	X+, r1

0000007e <.do_clear_bss_start>:
  7e:	a9 30       	cpi	r26, 0x09	; 9
  80:	b2 07       	cpc	r27, r18
  82:	e1 f7       	brne	.-8      	; 0x7c <.do_clear_bss_loop>
  84:	0e 94 ee 00 	call	0x1dc	; 0x1dc <main>
  88:	0c 94 3f 01 	jmp	0x27e	; 0x27e <_exit>

0000008c <__bad_interrupt>:
  8c:	0c 94 00 00 	jmp	0	; 0x0 <__vectors>

00000090 <micros>:
	return m;
}

unsigned long micros() {
	unsigned long m;
	uint8_t oldSREG = SREG, t;
  90:	3f b7       	in	r19, 0x3f	; 63
	
	cli();
  92:	f8 94       	cli
	m = timer0_overflow_count;
  94:	80 91 05 01 	lds	r24, 0x0105
  98:	90 91 06 01 	lds	r25, 0x0106
  9c:	a0 91 07 01 	lds	r26, 0x0107
  a0:	b0 91 08 01 	lds	r27, 0x0108
#if defined(TCNT0)
	t = TCNT0;
  a4:	26 b5       	in	r18, 0x26	; 38
#else
	#error TIMER 0 not defined
#endif

#ifdef TIFR0
	if ((TIFR0 & _BV(TOV0)) && (t < 255))
  a6:	a8 9b       	sbis	0x15, 0	; 21
  a8:	05 c0       	rjmp	.+10     	; 0xb4 <micros+0x24>
  aa:	2f 3f       	cpi	r18, 0xFF	; 255
  ac:	19 f0       	breq	.+6      	; 0xb4 <micros+0x24>
		m++;
  ae:	01 96       	adiw	r24, 0x01	; 1
  b0:	a1 1d       	adc	r26, r1
  b2:	b1 1d       	adc	r27, r1
#else
	if ((TIFR & _BV(TOV0)) && (t < 255))
		m++;
#endif

	SREG = oldSREG;
  b4:	3f bf       	out	0x3f, r19	; 63
	
	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
  b6:	ba 2f       	mov	r27, r26
  b8:	a9 2f       	mov	r26, r25
  ba:	98 2f       	mov	r25, r24
  bc:	88 27       	eor	r24, r24
  be:	82 0f       	add	r24, r18
  c0:	91 1d       	adc	r25, r1
  c2:	a1 1d       	adc	r26, r1
  c4:	b1 1d       	adc	r27, r1
  c6:	bc 01       	movw	r22, r24
  c8:	cd 01       	movw	r24, r26
  ca:	42 e0       	ldi	r20, 0x02	; 2
  cc:	66 0f       	add	r22, r22
  ce:	77 1f       	adc	r23, r23
  d0:	88 1f       	adc	r24, r24
  d2:	99 1f       	adc	r25, r25
  d4:	4a 95       	dec	r20
  d6:	d1 f7       	brne	.-12     	; 0xcc <micros+0x3c>
}
  d8:	08 95       	ret

000000da <delay.constprop.0>:

void delay(unsigned long ms)
  da:	8f 92       	push	r8
  dc:	9f 92       	push	r9
  de:	af 92       	push	r10
  e0:	bf 92       	push	r11
  e2:	cf 92       	push	r12
  e4:	df 92       	push	r13
  e6:	ef 92       	push	r14
  e8:	ff 92       	push	r15
{
	uint32_t start = micros();
  ea:	0e 94 48 00 	call	0x90	; 0x90 <micros>
  ee:	4b 01       	movw	r8, r22
  f0:	5c 01       	movw	r10, r24
  f2:	8a e0       	ldi	r24, 0x0A	; 10
  f4:	c8 2e       	mov	r12, r24
  f6:	d1 2c       	mov	r13, r1
  f8:	e1 2c       	mov	r14, r1
  fa:	f1 2c       	mov	r15, r1

	while (ms > 0) {
		yield();
		while ( ms > 0 && (micros() - start) >= 1000) {
  fc:	0e 94 48 00 	call	0x90	; 0x90 <micros>
 100:	dc 01       	movw	r26, r24
 102:	cb 01       	movw	r24, r22
 104:	88 19       	sub	r24, r8
 106:	99 09       	sbc	r25, r9
 108:	aa 09       	sbc	r26, r10
 10a:	bb 09       	sbc	r27, r11
 10c:	88 3e       	cpi	r24, 0xE8	; 232
 10e:	93 40       	sbci	r25, 0x03	; 3
 110:	a1 05       	cpc	r26, r1
 112:	b1 05       	cpc	r27, r1
 114:	98 f3       	brcs	.-26     	; 0xfc <delay.constprop.0+0x22>
			ms--;
 116:	21 e0       	ldi	r18, 0x01	; 1
 118:	c2 1a       	sub	r12, r18
 11a:	d1 08       	sbc	r13, r1
 11c:	e1 08       	sbc	r14, r1
 11e:	f1 08       	sbc	r15, r1
			start += 1000;
 120:	88 ee       	ldi	r24, 0xE8	; 232
 122:	88 0e       	add	r8, r24
 124:	83 e0       	ldi	r24, 0x03	; 3
 126:	98 1e       	adc	r9, r24
 128:	a1 1c       	adc	r10, r1
 12a:	b1 1c       	adc	r11, r1
{
	uint32_t start = micros();

	while (ms > 0) {
		yield();
		while ( ms > 0 && (micros() - start) >= 1000) {
 12c:	c1 14       	cp	r12, r1
 12e:	d1 04       	cpc	r13, r1
 130:	e1 04       	cpc	r14, r1
 132:	f1 04       	cpc	r15, r1
 134:	19 f7       	brne	.-58     	; 0xfc <delay.constprop.0+0x22>
			ms--;
			start += 1000;
		}
	}
}
 136:	ff 90       	pop	r15
 138:	ef 90       	pop	r14
 13a:	df 90       	pop	r13
 13c:	cf 90       	pop	r12
 13e:	bf 90       	pop	r11
 140:	af 90       	pop	r10
 142:	9f 90       	pop	r9
 144:	8f 90       	pop	r8
 146:	08 95       	ret

00000148 <__vector_16>:
#if defined(TIM0_OVF_vect)
ISR(TIM0_OVF_vect)
#else
ISR(TIMER0_OVF_vect)
#endif
{
 148:	1f 92       	push	r1
 14a:	0f 92       	push	r0
 14c:	0f b6       	in	r0, 0x3f	; 63
 14e:	0f 92       	push	r0
 150:	11 24       	eor	r1, r1
 152:	2f 93       	push	r18
 154:	3f 93       	push	r19
 156:	8f 93       	push	r24
 158:	9f 93       	push	r25
 15a:	af 93       	push	r26
 15c:	bf 93       	push	r27
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
 15e:	80 91 01 01 	lds	r24, 0x0101
 162:	90 91 02 01 	lds	r25, 0x0102
 166:	a0 91 03 01 	lds	r26, 0x0103
 16a:	b0 91 04 01 	lds	r27, 0x0104
	unsigned char f = timer0_fract;
 16e:	30 91 00 01 	lds	r19, 0x0100

	m += MILLIS_INC;
	f += FRACT_INC;
 172:	23 e0       	ldi	r18, 0x03	; 3
 174:	23 0f       	add	r18, r19
	if (f >= FRACT_MAX) {
 176:	2d 37       	cpi	r18, 0x7D	; 125
 178:	20 f4       	brcc	.+8      	; 0x182 <__vector_16+0x3a>
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;

	m += MILLIS_INC;
 17a:	01 96       	adiw	r24, 0x01	; 1
 17c:	a1 1d       	adc	r26, r1
 17e:	b1 1d       	adc	r27, r1
 180:	05 c0       	rjmp	.+10     	; 0x18c <__vector_16+0x44>
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
 182:	26 e8       	ldi	r18, 0x86	; 134
 184:	23 0f       	add	r18, r19
		m += 1;
 186:	02 96       	adiw	r24, 0x02	; 2
 188:	a1 1d       	adc	r26, r1
 18a:	b1 1d       	adc	r27, r1
	}

	timer0_fract = f;
 18c:	20 93 00 01 	sts	0x0100, r18
	timer0_millis = m;
 190:	80 93 01 01 	sts	0x0101, r24
 194:	90 93 02 01 	sts	0x0102, r25
 198:	a0 93 03 01 	sts	0x0103, r26
 19c:	b0 93 04 01 	sts	0x0104, r27
	timer0_overflow_count++;
 1a0:	80 91 05 01 	lds	r24, 0x0105
 1a4:	90 91 06 01 	lds	r25, 0x0106
 1a8:	a0 91 07 01 	lds	r26, 0x0107
 1ac:	b0 91 08 01 	lds	r27, 0x0108
 1b0:	01 96       	adiw	r24, 0x01	; 1
 1b2:	a1 1d       	adc	r26, r1
 1b4:	b1 1d       	adc	r27, r1
 1b6:	80 93 05 01 	sts	0x0105, r24
 1ba:	90 93 06 01 	sts	0x0106, r25
 1be:	a0 93 07 01 	sts	0x0107, r26
 1c2:	b0 93 08 01 	sts	0x0108, r27
}
 1c6:	bf 91       	pop	r27
 1c8:	af 91       	pop	r26
 1ca:	9f 91       	pop	r25
 1cc:	8f 91       	pop	r24
 1ce:	3f 91       	pop	r19
 1d0:	2f 91       	pop	r18
 1d2:	0f 90       	pop	r0
 1d4:	0f be       	out	0x3f, r0	; 63
 1d6:	0f 90       	pop	r0
 1d8:	1f 90       	pop	r1
 1da:	18 95       	reti

000001dc <main>:

void init()
{
	// this needs to be called before setup() or some functions won't
	// work there
	sei();
 1dc:	78 94       	sei
	
	// on the ATmega168, timer 0 is also used for fast hardware pwm
	// (using phase-correct PWM would mean that timer 0 overflowed half as often
	// resulting in different millis() behavior on the ATmega8 and ATmega168)
#if defined(TCCR0A) && defined(WGM01)
	sbi(TCCR0A, WGM01);
 1de:	84 b5       	in	r24, 0x24	; 36
 1e0:	82 60       	ori	r24, 0x02	; 2
 1e2:	84 bd       	out	0x24, r24	; 36
	sbi(TCCR0A, WGM00);
 1e4:	84 b5       	in	r24, 0x24	; 36
 1e6:	81 60       	ori	r24, 0x01	; 1
 1e8:	84 bd       	out	0x24, r24	; 36
	// this combination is for the standard atmega8
	sbi(TCCR0, CS01);
	sbi(TCCR0, CS00);
#elif defined(TCCR0B) && defined(CS01) && defined(CS00)
	// this combination is for the standard 168/328/1280/2560
	sbi(TCCR0B, CS01);
 1ea:	85 b5       	in	r24, 0x25	; 37
 1ec:	82 60       	ori	r24, 0x02	; 2
 1ee:	85 bd       	out	0x25, r24	; 37
	sbi(TCCR0B, CS00);
 1f0:	85 b5       	in	r24, 0x25	; 37
 1f2:	81 60       	ori	r24, 0x01	; 1
 1f4:	85 bd       	out	0x25, r24	; 37

	// enable timer 0 overflow interrupt
#if defined(TIMSK) && defined(TOIE0)
	sbi(TIMSK, TOIE0);
#elif defined(TIMSK0) && defined(TOIE0)
	sbi(TIMSK0, TOIE0);
 1f6:	80 91 6e 00 	lds	r24, 0x006E
 1fa:	81 60       	ori	r24, 0x01	; 1
 1fc:	80 93 6e 00 	sts	0x006E, r24
	// this is better for motors as it ensures an even waveform
	// note, however, that fast pwm mode can achieve a frequency of up
	// 8 MHz (with a 16 MHz clock) at 50% duty cycle

#if defined(TCCR1B) && defined(CS11) && defined(CS10)
	TCCR1B = 0;
 200:	10 92 81 00 	sts	0x0081, r1

	// set timer 1 prescale factor to 64
	sbi(TCCR1B, CS11);
 204:	80 91 81 00 	lds	r24, 0x0081
 208:	82 60       	ori	r24, 0x02	; 2
 20a:	80 93 81 00 	sts	0x0081, r24
#if F_CPU >= 8000000L
	sbi(TCCR1B, CS10);
 20e:	80 91 81 00 	lds	r24, 0x0081
 212:	81 60       	ori	r24, 0x01	; 1
 214:	80 93 81 00 	sts	0x0081, r24
	sbi(TCCR1, CS10);
#endif
#endif
	// put timer 1 in 8-bit phase correct pwm mode
#if defined(TCCR1A) && defined(WGM10)
	sbi(TCCR1A, WGM10);
 218:	80 91 80 00 	lds	r24, 0x0080
 21c:	81 60       	ori	r24, 0x01	; 1
 21e:	80 93 80 00 	sts	0x0080, r24

	// set timer 2 prescale factor to 64
#if defined(TCCR2) && defined(CS22)
	sbi(TCCR2, CS22);
#elif defined(TCCR2B) && defined(CS22)
	sbi(TCCR2B, CS22);
 222:	80 91 b1 00 	lds	r24, 0x00B1
 226:	84 60       	ori	r24, 0x04	; 4
 228:	80 93 b1 00 	sts	0x00B1, r24

	// configure timer 2 for phase correct pwm (8-bit)
#if defined(TCCR2) && defined(WGM20)
	sbi(TCCR2, WGM20);
#elif defined(TCCR2A) && defined(WGM20)
	sbi(TCCR2A, WGM20);
 22c:	80 91 b0 00 	lds	r24, 0x00B0
 230:	81 60       	ori	r24, 0x01	; 1
 232:	80 93 b0 00 	sts	0x00B0, r24
#endif

#if defined(ADCSRA)
	// set a2d prescaler so we are inside the desired 50-200 KHz range.
	#if F_CPU >= 16000000 // 16 MHz / 128 = 125 KHz
		sbi(ADCSRA, ADPS2);
 236:	80 91 7a 00 	lds	r24, 0x007A
 23a:	84 60       	ori	r24, 0x04	; 4
 23c:	80 93 7a 00 	sts	0x007A, r24
		sbi(ADCSRA, ADPS1);
 240:	80 91 7a 00 	lds	r24, 0x007A
 244:	82 60       	ori	r24, 0x02	; 2
 246:	80 93 7a 00 	sts	0x007A, r24
		sbi(ADCSRA, ADPS0);
 24a:	80 91 7a 00 	lds	r24, 0x007A
 24e:	81 60       	ori	r24, 0x01	; 1
 250:	80 93 7a 00 	sts	0x007A, r24
		cbi(ADCSRA, ADPS2);
		cbi(ADCSRA, ADPS1);
		sbi(ADCSRA, ADPS0);
	#endif
	// enable a2d conversions
	sbi(ADCSRA, ADEN);
 254:	80 91 7a 00 	lds	r24, 0x007A
 258:	80 68       	ori	r24, 0x80	; 128
 25a:	80 93 7a 00 	sts	0x007A, r24
	// here so they can be used as normal digital i/o; they will be
	// reconnected in Serial.begin()
#if defined(UCSRB)
	UCSRB = 0;
#elif defined(UCSR0B)
	UCSR0B = 0;
 25e:	10 92 c1 00 	sts	0x00C1, r1
	
	setup();
    
	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
 262:	c0 e0       	ldi	r28, 0x00	; 0
 264:	d0 e0       	ldi	r29, 0x00	; 0
void setup() {
}

void loop() {
  PORTB = 0xff;
 266:	1f ef       	ldi	r17, 0xFF	; 255
 268:	15 b9       	out	0x05, r17	; 5
  delay(10);
 26a:	0e 94 6d 00 	call	0xda	; 0xda <delay.constprop.0>
  PORTB = 0x00;
 26e:	15 b8       	out	0x05, r1	; 5
  delay(10);
 270:	0e 94 6d 00 	call	0xda	; 0xda <delay.constprop.0>
 274:	20 97       	sbiw	r28, 0x00	; 0
 276:	c1 f3       	breq	.-16     	; 0x268 <main+0x8c>
 278:	0e 94 00 00 	call	0	; 0x0 <__vectors>
 27c:	f5 cf       	rjmp	.-22     	; 0x268 <main+0x8c>

0000027e <_exit>:
 27e:	f8 94       	cli

00000280 <__stop_program>:
 280:	ff cf       	rjmp	.-2      	; 0x280 <__stop_program>
