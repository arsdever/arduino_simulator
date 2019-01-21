//==  Nomenclature   ===================================================================================================
// d	- destination (source)
// r	- source
// K	- constant data
// k	- constant address
// b	- bit in register file
// s	- bit in status register
// X	- indirect address register (R27:R26)
// Y	- indirect address register (R29:R28)
// Z	- indirect address register (R31:R30)
// A	- I/O location address
// q	- displacement for direct addressing
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "cpu_global.h"

#include <exception>
#include <ram>

#include <QObject>
#include <QMap>

class CRAM;

class CPU_EXPORT CCPU : public QObject
{

	Q_OBJECT

public:
	struct SState
	{
		enum EFlag
		{
			CFlag = 1,
			ZFlag = 2,
			NFlag = 4,
			VFlag = 8,
			SFlag = 16,
			HFlag = 32,
			TFlag = 64,
			IFlag = 128
		};

		quint8* DATA_MEMORY;
		quint16 PC;
		quint16 IR[2];
		quint8* GR;
		quint8* IOR;
		quint8* ExIOR;
		quint16 X() { return (quint16&)GR[26]; }
		quint16 Y() { return (quint16&)GR[28]; }
		quint16 Z() { return (quint16&)GR[30]; }
		void setX(quint16 value) { (quint16&)GR[26] = value; }
		void setY(quint16 value) { (quint16&)GR[28] = value; }
		void setZ(quint16 value) { (quint16&)GR[30] = value; }
		bool RUN;
		quint8& FLAGS;
		quint16 SP;

		SState()
			: PC{ 0 }
			, IR{ 0 }
			, DATA_MEMORY(new quint8[0x8FF])
			, GR(DATA_MEMORY)
			, IOR(DATA_MEMORY + 32)
			, ExIOR(DATA_MEMORY + 96)
			, RUN(false)
			, FLAGS(IOR[0x3F])
			, SP{ 0 }
		{
			for (int i = 0; i < 0x8FF; ++i)
				DATA_MEMORY[i] = 0;
		}
		SState(SState const& c)
			: SP{ 0 }
			, DATA_MEMORY(new quint8[0x8FF])
			, GR(DATA_MEMORY)
			, IOR(DATA_MEMORY + 32)
			, ExIOR(DATA_MEMORY + 96)
			, FLAGS(IOR[0x3F])
		{
			for (int i = 0; i < 0x8FF; ++i)
				DATA_MEMORY[i] = c.DATA_MEMORY[i];
			operator=(c);
		}

		SState& operator=(SState const& c)
		{
			PC = c.PC;
			(quint32&)IR = (quint32&)c.IR;

			for (int i = 0; i < 64; ++i)
				GR[i] = c.GR[i];
			for (int i = 0; i < 64; ++i)
				GR[i] = c.GR[i];

			RUN = c.RUN;
			FLAGS = c.FLAGS;
			return *this;
		}

	} m_sState;


public:
	CCPU(CRAM* pFlash);
	CRAM* Ram() const { return m_pFlash; }
	SState GetState() const;
	QString GetUUID() const;

	quint8* GetIOPorts() const;
	void Run();
	void Step();

	void Restart();

private:
	void Fetch();
	quint8 Decode(quint16 instruction);
	quint8 Execute();
	void HandleInterrupt();

	void SetFlag(SState::EFlag flag, bool set);
	void SetFlags(int flags);
	bool GetFlag(SState::EFlag flag);
	int GetFlags();

public:
	typedef quint8 (CCPU::*FunctionPointer)();

public:
	enum InstructionCode
	{
		ADC,	// 0001 11rd dddd rrrr
		ADD,	// 0000 11rd dddd rrrr
		ADIW,	// 1001 0110 KKdd KKKK
		AND,	// 0010 00rd dddd rrrr
		ANDI,	// 0111 KKKK dddd KKKK
		ASR,	// 1001 010d dddd 0101
		BCLR,	// 1001 0100 1sss 1000
		BLD,	// 1111 100d dddd 0bbb
		BRBC,	// 1111 01kk kkkk ksss
		BRBS,	// 1111 00kk kkkk ksss
		BRCC,	// 1111 01kk kkkk k000
		BRCS,	// 1111 00kk kkkk k000
		BREAK,	// 1001 0101 1001 1000
		BREQ,	// 1111 00kk kkkk k001
		BRGE,	// 1111 01kk kkkk k100
		BRHC,	// 1111 01kk kkkk k101
		BRHS,	// 1111 00kk kkkk k101
		BRID,	// 1111 01kk kkkk k111
		BRIE,	// 1111 00kk kkkk k111
		BRLO,	// 1111 00kk kkkk k000
		BRLT,	// 1111 00kk kkkk k100
		BRMI,	// 1111 00kk kkkk k010
		BRNE,	// 1111 01kk kkkk k001
		BRPL,	// 1111 01kk kkkk k010
		BRSH,	// 1111 01kk kkkk k000
		BRTC,	// 1111 01kk kkkk k110
		BRTS,	// 1111 00kk kkkk k110
		BRVC,	// 1111 01kk kkkk k011
		BRVS,	// 1111 00kk kkkk k011
		BSET,	// 1001 0100 0sss 1000
		BST,	// 1111 101d dddd 0bbb
		CALL,	// 1001 010k kkkk 111k kkkk kkkk kkkk kkkk
		CBI,	// 1001 1000 AAAA Abbb
		CBR,	// 
		CLC,	// 1001 0100 1000 1000
		CLH,
		CLI,
		CLN,
		CLR,	// 0010 01dd dddd dddd
		CLS,
		CLT,
		CLV,
		CLZ,
		COM,	// 1001 010d dddd 0000
		CP,		// 0001 01rd dddd rrrr
		CPC,	// 0000 01rd dddd rrrr
		CPI,	// 0011 KKKK dddd KKKK
		CPSE,	// 0001 00rd dddd rrrr
		DEC,	// 1001 010d dddd 1010
		DES,	// 1001 0100 KKKK 1011
		EICALL,	// 1001 0101 0001 1001
		EIJMP,	// 1001 0100 0001 1001
		ELPM,	//					   skipped
		EOR,	// 0010 01rd dddd rrrr
		FMUL,	// 0000 0011 0ddd 1rrr skipped
		FMULS,	// 0000 0011 1ddd 0rrr skipped
		FMULSU, // 0000 0011 1ddd 1rrr skipped
		ICALL,	// 1001 0101 0000 1001
		IJMP,	// 1001 0100 0000 1001
		IN,		// 1011 0AAd dddd AAAA
		INC,	// 1001 010d dddd 0011
		JMP,	// 1001 010k kkkk 110k kkkk kkkk kkkk kkkk
		LAC,
		LAS,
		LAT,
		LD,		// 1001 000d dddd 1100	X unchanged
				// 1001 000d dddd 1101  X post-increment
				// 1001 000d dddd 1110  X pre-increment
				// 1000 000d dddd 1000	Y unchanged
				// 1001 000d dddd 1001  Y post-increment
				// 1001 000d dddd 1010  Y pre-increment
				// 1000 000d dddd 0000	Y unchanged
				// 1001 000d dddd 0001  Y post-increment
				// 1001 000d dddd 0010  Y pre-increment
		LDD,	// 10q0 qq0d dddd 1qqq	Y changed by q
				// 10q0 qq0d dddd 0qqq	Z changed by q
		LDI,	// 1110 KKKK dddd KKKK
		LDS,	// 1001 000d dddd 0000 kkkk kkkk kkkk kkkk
		LDS_16,
		LPM,	// 1001 0101 1100 1000	Z unchanged, R0 is default
				// 1001 000d dddd 0100	Z unchanged
				// 1001 000d dddd 0101  Z post-increment
		LSL,
		LSR,
		MOV,	// 0010 11rd dddd rrrr
		MOVW,	// 0000 0001 dddd rrrr
		MUL,	// 1001 11rd dddd rrrr
		MULS,
		MULSU,
		NEG,
		NOP,	// 0000 0000 0000 0000
		OR,		// 0010	10rd dddd rrrr
		ORI,	// 0110 KKKK dddd KKKK
		OUT,	// 1011 1AAr rrrr AAAA
		POP,	// 1001 000d dddd 1111
		PUSH,	// 1001 001d dddd 1111
		RCALL,	// 1101 kkkk kkkk kkkk
		RET,
		RETI,	// 1001 0101 0001 1000
		RJMP,	// 1100 kkkk kkkk kkkk
		ROL,
		ROR,
		SBC,	// 0000 10rd dddd rrrr
		SBCI,	// 0100 KKKK dddd KKKK
		SBI,	// 1001 1010 AAAA Abbb
		SBIC,
		SBIS,	// 1001 1011 AAAA Abbb
		SBIW,	// 1001 0111 KKdd KKKK
		SBR,
		SBRC,
		SBRS,
		SEC,	//
		SEH,	//
		SEI,	//
		SEN,	//
		SER,	
		SES,	//
		SET,	//
		SEV,	//
		SEZ,	//
		SLEEP,
		SPM,
		SPM_2,
		ST,		// 1001 001r rrrr 1100	X unchanged
				// 1001 001r rrrr 1101  X post-increment
				// 1001 001r rrrr 1110  X pre-increment
				// 1000 001r rrrr 1000	Y unchanged
				// 1001 001r rrrr 1001  Y post-increment
				// 1001 001r rrrr 1010  Y pre-increment
				// 1000 001r rrrr 0000	Y unchanged
				// 1001 001r rrrr 0001  Y post-increment
				// 1001 001r rrrr 0010  Y pre-increment
		STD,	// 10q0 qq1r rrrr 1qqq	Y changed by q
				// 10q0 qq1r rrrr 0qqq	Z changed by q
		STS,	// 1001 001d dddd 0000 kkkk kkkk kkkk kkkk
		STS_16,
		SUB,	// 0001 10rd dddd rrrr
		SUBI,	// 0101 KKKK dddd KKKK
		SWAP,
		TST,
		WDR,
		XCH,
	};

	quint8 ARITHM_exec();
	quint8 ARITHM_IW_exec();
	quint8 LOGIC_exec();
	quint8 LOGICI_exec();
	quint8 ASR_exec();
	quint8 SREG_exec();
	quint8 TBIT_exec();
	quint8 BRANCH_exec();
	quint8 BREAK_exec();
	quint8 JMP_exec();
	quint8 CBI_exec();
	quint8 CBR_exec();
	quint8 CLR_exec();
	quint8 COM_exec();
	quint8 CP_exec();
	quint8 CPI_exec();
	quint8 CPSE_exec();
	quint8 DEC_exec();
	quint8 DES_exec();
	quint8 JUMPS_exec();
	quint8 ELPM_exec();
	quint8 FMUL_exec();
	quint8 FMULS_exec();
	quint8 FMULSU_exec();
	quint8 IO_exec();
	quint8 INC_exec();
	quint8 LAC_exec();
	quint8 LAS_exec();
	quint8 LAT_exec();
	quint8 LDST_exec();
	quint8 LDSTD_exec();
	quint8 LDI_exec();
	quint8 LDSTS_exec();
	quint8 LDS_16_exec();
	quint8 LPM_exec();
	quint8 LSL_exec();
	quint8 LSR_exec();
	quint8 MOV_exec();
	quint8 MOVW_exec();
	quint8 MUL_exec();
	quint8 MULS_exec();
	quint8 MULSU_exec();
	quint8 NEG_exec();
	quint8 NOP_exec();
	quint8 ORI_exec();
	quint8 STACK_exec();
	quint8 RCALL_exec();
	quint8 RET_exec();
	quint8 RJMP_exec();
	quint8 ROL_exec();
	quint8 ROR_exec();
	quint8 SBI_exec();
	quint8 SBIC_exec();
	quint8 SBIS_exec();
	quint8 SBR_exec();
	quint8 SBRC_exec();
	quint8 SBRS_exec();
	quint8 SEC_exec();
	quint8 SEH_exec();
	quint8 SEI_exec();
	quint8 SEN_exec();
	quint8 SER_exec();
	quint8 SES_exec();
	quint8 SET_exec();
	quint8 SEV_exec();
	quint8 SEZ_exec();
	quint8 SLEEP_exec();
	quint8 SPM_exec();
	quint8 SPM_2_exec();
	quint8 STS_16_exec();
	quint8 SUBI_exec();
	quint8 SWAP_exec();
	quint8 TST_exec();
	quint8 WDR_exec();
	quint8 XCH_exec();
	
	typedef QMap<int, CCPU::FunctionPointer> InstructionMap;

	static InstructionMap InitMap();
	static InstructionMap s_mapInstructions;

private:
	FunctionPointer m_fptrExecuter;
	CRAM* m_pRam;
	CRAM* m_pFlash;
	QString m_strUUID;

signals:
	void IOChanged();

public:
	struct cpu_exception : public std::exception {};
	struct breakpoint_exception : cpu_exception {};
	struct ram_not_found_exception : cpu_exception {};
	struct invalid_register_exception : cpu_exception {};
	struct invalid_function_call : cpu_exception {};
};