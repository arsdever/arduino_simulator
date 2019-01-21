//----------------------------------------------------------------------------------------------------------------------
// References
// AVR instruction set		- http://ww1.microchip.com/downloads/en/devicedoc/atmel-0856-avr-instruction-set-manual.pdf
// AVR instruction opcodes	- http://lyons42.com/AVR/Opcodes/AVRAllOpcodes.html
//----------------------------------------------------------------------------------------------------------------------

#include "cpu.h"

#include "io_controller.h"
#include <core>
#include <ram>

#include <QString>
#include <QUuid>
#include <chrono>
#include <thread>

#define TICK_DURATION 600

CCPU::CCPU(CRAM* pRam)
	: m_sState()
	, m_fptrExecuter(nullptr)
	, m_pFlash(pRam)
	, m_pRam(new CRAM(m_sState.DATA_MEMORY + 256, 2048))
	, m_strUUID("CPU-" + QUuid::createUuid().toString())
{
}

QString CCPU::GetUUID() const
{
	return m_strUUID;
}

void CCPU::Restart()
{
	m_sState.SP = m_pRam->GetSize();
	m_sState.PC = 0;
	m_sState.RUN = true;
}

void CCPU::Fetch()
{
	if (m_pFlash == nullptr)
		throw ram_not_found_exception();

	m_sState.IR[0] = m_pFlash->operator[]<quint16>(m_sState.PC);
	m_sState.IR[1] = m_pFlash->operator[]<quint16>(m_sState.PC + 2);
}

quint8 CCPU::Decode(quint16 instruction)
{
	InstructionCode code = NOP;

	switch (instruction & 0xF000)
	{
	case 0x3000:
		code = CPI; break;
	case 0x4000:
		code = SBCI; break;
	case 0x5000:
		code = SUBI; break;
	case 0x6000:
		code = ORI; break;
	case 0x7000:
		code = ANDI; break;
	case 0xC000:
		code = RJMP; break;
	case 0XD000:
		code = RCALL; break;
	case 0XE000:
		code = LDI; break;
	}

	if (code == NOP)
	{
		switch (instruction & 0xFF00)
		{
		case 0x0100:
			code = MOVW; break;
		case 0x0200:
			code = MULS; break;
		case 0x300:
			if ((instruction & 0x00F0) <= 0x0070 && (instruction & 0x000F) <= 0x0007)
				code = MULSU;
			else if ((instruction & 0x00F0) <= 0x0070 && (instruction & 0x000F) > 0x0007)
				code = FMULS;
			else if ((instruction & 0x00F0) > 0x0070 && (instruction & 0x000F) > 0x0007)
				code = FMULSU;
			else
				code = FMUL;
			break;
		case 0x0400: case 0x0500: case 0x0600: case 0x0700:
			code = CPC; break;
		case 0x0800: case 0x0900: case 0x0A00: case 0x0B00:
			code = SBC; break;
		case 0x0C00: case 0x0D00: case 0x0E00: case 0x0F00:
			code = ADD; break;
		case 0x1000: case 0x1100: case 0x1200: case 0x1300:
			code = CPSE; break;
		case 0x1400: case 0x1500: case 0x1600: case 0x1700:
			code = CP; break;
		case 0x1800: case 0x1900: case 0x1A00: case 0x1B00:
			code = SUB; break;
		case 0x1C00: case 0x1D00: case 0x1E00: case 0x1F00:
			code = ADC; break;
		case 0x2000: case 0x2100: case 0x2200: case 0x2300:
			code = AND; break;
		case 0x2400: case 0x2500: case 0x2600: case 0x2700:
			code = EOR; break;
		case 0x2800: case 0x2900: case 0x2A00: case 0x2B00:
			code = OR; break;
		case 0x2C00: case 0x2D00: case 0x2E00: case 0x2F00:
			code = MOV; break;
		case 0x8000: case 0x8100:
			if (instruction & 0x000F)
				code = LDD;
			else
				code = LD;
			break;
		case 0x8200: case 0x8300:
			if (instruction & 0x000F)
				code = STD;
			else
				code = ST;
			break;
		case 0x8400: case 0x8500: case 0x8800: case 0x8900: case 0x8C00: case 0x8D00:
			code = LDD; break;
		case 0x8600: case 0x8700: case 0x8A00: case 0x8B00: case 0x8E00: case 0x8F00:
			code = ST; break;
		case 0x9000: case 0x9100:
		{
			switch (m_sState.IR[0] & 0x000F)
			{
			case 0x0000:
				code = LDS; break;
			case 0x0001: case 0x0002: case 0x0009: case 0x000A: case 0x000C: case 0x000D: case 0x000E:
				code = LD; break;
			case 0x0004: case 0x0005:
				code = LPM; break;
			case 0x0006: case 0x0007:
				code = ELPM; break;
			case 0x000F:
				code = POP; break;
			}
			break;
		}
		case 0x9200: case 0x9300:
		{
			switch (m_sState.IR[0] & 0x000F)
			{
			case 0x0000:
				code = STS; break;
			case 0x0001: case 0x0002: case 0x0009: case 0x000A: case 0x000C: case 0x000D: case 0x000E:
				code = ST; break;
			case 0x000F:
				code = PUSH; break;
			}
			break;
		}
		case 0x9400:
		case 0x9500:
		{
			switch (instruction & 0x000F)
			{
			case 0x0000: code = COM; break;
			case 0x0001: code = NEG; break;
			case 0x0002: code = SWAP; break;
			case 0x0003: code = INC; break;
			case 0x0005: code = ASR; break;
			case 0x0006: code = LSR; break;
			case 0x0007: code = ROR; break;
			case 0x000A: code = DEC; break;
			case 0x000C:
			case 0x000D: code = JMP; break;
			case 0x000E:
			case 0x000F: code = CALL; break;
			default:
			{
				if (instruction & 0x0F0F == 0x040B)
				{
					code = DES;
					break;
				}
				switch (instruction & 0x0FFF)
				{
				case 0x0408: code = SEC; break;
				case 0x0418: code = SEZ; break;
				case 0x0428: code = SEN; break;
				case 0x0438: code = SEV; break;
				case 0x0448: code = SES; break;
				case 0x0458: code = SEH; break;
				case 0x0468: code = SET; break;
				case 0x0478: code = SEI; break;
				case 0x0488: code = CLC; break;
				case 0x0498: code = CLZ; break;
				case 0x04A8: code = CLN; break;
				case 0x04B8: code = CLV; break;
				case 0x04C8: code = CLS; break;
				case 0x04D8: code = CLH; break;
				case 0x04E8: code = CLT; break;
				case 0x04F8: code = CLI; break;
				case 0x0409: code = IJMP; break;
				case 0x0419: code = EIJMP; break;
				case 0x0508: code = RET; break;
				case 0x0509: code = ICALL; break;
				case 0x0518: code = RETI; break;
				case 0x0519: code = EICALL; break;
				case 0x0588: code = SLEEP; break;
				case 0x0598: code = BREAK; break;
				case 0x05A8: code = WDR; break;
				case 0x05C8: code = LPM; break;
				case 0x05D8: code = ELPM; break;
				case 0x05E8: code = SPM; break;
				case 0x05F8: code = SPM; break;
				}
			}
			}
			break;
		}
		case 0x9600:
			code = ADIW; break;
		case 0x9700:
			code = SBIW; break;
		case 0x9800:
			code = CBI; break;
		case 0x9900:
			code = SBIC; break;
		case 0x9A00:
			code = SBI; break;
		case 0x9B00:
			code = SBIS; break;
		case 0x9C00: case 0x9D00: case 0x9E00: case 0x9F00:
			code = MUL; break;
		case 0xA000: case 0xA100: case 0xA400: case 0xA500: case 0xA800: case 0xA900: case 0xAC00: case 0xAD00:
			code = LDS_16; break;
		case 0xA200: case 0xA300: case 0xA600: case 0xA700: case 0xAA00: case 0xAB00: case 0xAE00: case 0xAF00:
			code = STS_16; break;
		case 0xB000: case 0xB100: case 0xB200: case 0xB300: case 0xB400: case 0xB500: case 0xB600: case 0xB700:
			code = IN; break;
		case 0xB800: case 0xB900: case 0xBA00: case 0xBB00: case 0xBC00: case 0xBD00: case 0xBE00: case 0xBF00:
			code = OUT; break;
		case 0xF000:
		case 0xF100:
		case 0xF200:
		case 0xF300:
			code = BRBS;
		case 0xF400:
		case 0xF500:
		case 0xF600:
		case 0xF700:
			code = BRBC;
		case 0xF800: case 0xF900:
			if (instruction & 0x000F < 0x0008)
				code = BLD;
			break;
		case 0xFA00: case 0xFB00:
			if (instruction & 0x000F < 0x0008)
				code = BST;
			break;
		case 0xFC00: case 0xFD00:
			if (instruction & 0x000F < 0x0008)
				code = SBRC;
			break;
		case 0xFE00: case 0xFF00:
			if (instruction & 0x000F < 0x0008)
				code = SBRS;
			break;
		}

	}

	m_fptrExecuter = CCPU::s_mapInstructions[code];
	switch (code)
	{
	case LDS:
	case STS:
	case JMP:
	case CALL:
		return 4;
	default:
		return 2;
	}
}

quint8 CCPU::Execute()
{
	if (m_fptrExecuter == nullptr)
	{
		m_sState.RUN = false;
		throw invalid_function_call();
	}

	return std::invoke(m_fptrExecuter, this);
}

void CCPU::SetFlag(SState::EFlag flag, bool set)
{
	if (set)
		m_sState.FLAGS |= flag;
	else
		m_sState.FLAGS &= ~flag;
}

void CCPU::SetFlags(int flags)
{
	m_sState.FLAGS = flags;
}

bool CCPU::GetFlag(SState::EFlag flag)
{
	return m_sState.FLAGS & flag;
}

int CCPU::GetFlags()
{
	return m_sState.FLAGS;
}

CCPU::SState CCPU::GetState() const
{
	return m_sState;
}

void CCPU::Run()
{
	while (m_sState.RUN)
	{
		Step();
	}
	CallFunction<ILogger>(ILogger::InfoFunctor(QString("Execution finished")));
}

void CCPU::Step()
{
	CallFunction<ILogger>(ILogger::InfoFunctor(QString("Running 0x%1").arg(m_sState.PC, 8, 16, QChar('0'))));
	if (m_sState.RUN && m_sState.FLAGS & SState::IFlag)
		HandleInterrupt();

	Fetch();
	m_sState.PC += Decode(m_sState.IR[0]);
	std::this_thread::sleep_for(std::chrono::nanoseconds(Execute() * TICK_DURATION));
}

quint8* CCPU::GetIOPorts() const
{
	return m_sState.IOR;
}


void CCPU::HandleInterrupt()
{
	//m_sState.STACK.push(m_sState.PC);
	//m_sState.STACK.push(m_sState.FLAGS);
	//m_sState.PC = m_sState.IVT[m_sState.INT];
	//m_sState.SF = m_sState.SP;
	//m_sState.FLAGS &= ~SState::IFlag;
}

CCPU::InstructionMap CCPU::s_mapInstructions = CCPU::InitMap();

CCPU::InstructionMap CCPU::InitMap()
{
	InstructionMap map;
	map.insert(CCPU::ADD, &CCPU::ARITHM_exec);
	map.insert(CCPU::ADC, &CCPU::ARITHM_exec);
	map.insert(CCPU::ADIW, &CCPU::ARITHM_IW_exec);
	map.insert(CCPU::AND, &CCPU::LOGIC_exec);
	map.insert(CCPU::ANDI, &CCPU::LOGICI_exec);
	map.insert(CCPU::ASR, &CCPU::ASR_exec);
	map.insert(CCPU::BCLR, &CCPU::SREG_exec);
	map.insert(CCPU::BLD, &CCPU::TBIT_exec);
	map.insert(CCPU::BRBC, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRBS, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRCC, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRCS, &CCPU::BRANCH_exec);
	map.insert(CCPU::BREAK, &CCPU::BREAK_exec);
	map.insert(CCPU::BREQ, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRGE, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRHC, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRHS, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRID, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRIE, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRLO, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRLT, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRMI, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRNE, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRPL, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRSH, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRTC, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRTS, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRVC, &CCPU::BRANCH_exec);
	map.insert(CCPU::BRVS, &CCPU::BRANCH_exec);
	map.insert(CCPU::BSET, &CCPU::SREG_exec);
	map.insert(CCPU::BST, &CCPU::TBIT_exec);
	map.insert(CCPU::CALL, &CCPU::JMP_exec);
	map.insert(CCPU::CBI, &CCPU::CBI_exec);
	map.insert(CCPU::CLC, &CCPU::SREG_exec);
	map.insert(CCPU::CLH, &CCPU::SREG_exec);
	map.insert(CCPU::CLI, &CCPU::SREG_exec);
	map.insert(CCPU::CLN, &CCPU::SREG_exec);
	map.insert(CCPU::CLR, &CCPU::CLR_exec);
	map.insert(CCPU::CLS, &CCPU::SREG_exec);
	map.insert(CCPU::CLT, &CCPU::SREG_exec);
	map.insert(CCPU::CLV, &CCPU::SREG_exec);
	map.insert(CCPU::CLZ, &CCPU::SREG_exec);
	map.insert(CCPU::COM, &CCPU::COM_exec);
	map.insert(CCPU::CP, &CCPU::CP_exec);
	map.insert(CCPU::CPC, &CCPU::CP_exec);
	map.insert(CCPU::CPI, &CCPU::CPI_exec);
	map.insert(CCPU::CPSE, &CCPU::CPSE_exec);
	map.insert(CCPU::DEC, &CCPU::DEC_exec);
	map.insert(CCPU::EICALL, &CCPU::JUMPS_exec);
	map.insert(CCPU::EIJMP, &CCPU::JUMPS_exec);
	map.insert(CCPU::EOR, &CCPU::LOGIC_exec);
	map.insert(CCPU::ICALL, &CCPU::JUMPS_exec);
	map.insert(CCPU::IJMP, &CCPU::JUMPS_exec);
	map.insert(CCPU::IN, &CCPU::IO_exec);
	map.insert(CCPU::INC, &CCPU::INC_exec);
	map.insert(CCPU::JMP, &CCPU::JMP_exec);
	map.insert(CCPU::LD, &CCPU::LDST_exec);
	map.insert(CCPU::LDD, &CCPU::LDSTD_exec);
	map.insert(CCPU::LDI, &CCPU::LDI_exec);
	map.insert(CCPU::LDS, &CCPU::LDSTS_exec);
	map.insert(CCPU::LPM, &CCPU::LPM_exec);
	map.insert(CCPU::MOV, &CCPU::MOV_exec);
	map.insert(CCPU::MOVW, &CCPU::MOVW_exec);
	map.insert(CCPU::MUL, &CCPU::MUL_exec);
	map.insert(CCPU::MULS, &CCPU::MULS_exec);
	map.insert(CCPU::MULSU, &CCPU::MULSU_exec);
	map.insert(CCPU::NOP, &CCPU::NOP_exec);
	map.insert(CCPU::OUT, &CCPU::IO_exec);
	map.insert(CCPU::ORI, &CCPU::LOGICI_exec);
	map.insert(CCPU::OR, &CCPU::LOGIC_exec);
	map.insert(CCPU::POP, &CCPU::STACK_exec);
	map.insert(CCPU::PUSH, &CCPU::STACK_exec);
	map.insert(CCPU::RCALL, &CCPU::RJMP_exec);
	map.insert(CCPU::RET, &CCPU::RET_exec);
	map.insert(CCPU::RETI, &CCPU::RET_exec);
	map.insert(CCPU::RJMP, &CCPU::RJMP_exec);
	map.insert(CCPU::SBI, &CCPU::SBI_exec);
	map.insert(CCPU::SBC, &CCPU::ARITHM_exec);
	map.insert(CCPU::SBCI, &CCPU::SUBI_exec);
	map.insert(CCPU::SBIS, &CCPU::SBIS_exec);
	map.insert(CCPU::SBIW, &CCPU::ARITHM_IW_exec);
	map.insert(CCPU::SEC, &CCPU::SREG_exec);
	map.insert(CCPU::SEH, &CCPU::SREG_exec);
	map.insert(CCPU::SEI, &CCPU::SREG_exec);
	map.insert(CCPU::SEN, &CCPU::SREG_exec);
	map.insert(CCPU::SER, &CCPU::SER_exec);
	map.insert(CCPU::SES, &CCPU::SREG_exec);
	map.insert(CCPU::SET, &CCPU::SREG_exec);
	map.insert(CCPU::SEV, &CCPU::SREG_exec);
	map.insert(CCPU::SEZ, &CCPU::SREG_exec);
	map.insert(CCPU::ST, &CCPU::LDST_exec);
	map.insert(CCPU::STD, &CCPU::LDSTD_exec);
	map.insert(CCPU::STS, &CCPU::LDSTS_exec);
	map.insert(CCPU::SUB, &CCPU::ARITHM_exec);
	map.insert(CCPU::SUBI, &CCPU::SUBI_exec);
	return map;
}

// instruction implementation

quint8 CCPU::ARITHM_exec()
{
	quint8& dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	quint8& source = m_sState.GR[(m_sState.IR[0] & 0x0200) >> 5 | (m_sState.IR[0] & 0x000F)];
	quint8 result = 0;

	switch (m_sState.IR[0] & 0x1400)
	{
	case 0x1400:
		result = dest + source + GetFlag(SState::CFlag); break;
	case 0x0400:
		result = dest + source; break;
	case 0x0000:
		result = dest - source - GetFlag(SState::CFlag); break;
	case 0x1000:
		result = dest - source; break;
	}

	SetFlag(SState::HFlag, dest & 0x04 && source & 0x04 || source & 0x04 && !(result & 0x04) || dest & 0x04 && !(result & 0x04));
	SetFlag(SState::VFlag, dest & 0x80 && source & 0x80 && !(result & 0x80) || !(dest & 0x80) && !(source & 0x80) && result & 0x80);
	SetFlag(SState::NFlag, result & 0x80);
	SetFlag(SState::SFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::VFlag));
	SetFlag(SState::ZFlag, !result);
	SetFlag(SState::CFlag, dest & 0x80 && source & 0x80 || source & 0x80 && !(result & 0x80) || dest & 0x80 && !(result & 0x80));
	dest = result;
	return 1;
}

quint8 CCPU::ARITHM_IW_exec()
{
	quint16* int_reg;
	switch (m_sState.IR[0] & 0x0030)
	{
	case 0x0000: int_reg = &(quint16&)m_sState.GR[24]; break;
	case 0x0010: int_reg = &(quint16&)m_sState.GR[26]; break;
	case 0x0020: int_reg = &(quint16&)m_sState.GR[28]; break;
	case 0x0030: int_reg = &(quint16&)m_sState.GR[30]; break;
	}

	quint16 value = ((m_sState.IR[0] & 0x00C0) >> 2) | (m_sState.IR[0] & 0x000F);
	quint16 result;

	if (m_sState.IR[0] & 0x0100)
		result = *int_reg - value;
	else
		result = *int_reg + value;

	SetFlag(SState::VFlag, !(*int_reg & 0x8000) && result & 0x8000);
	SetFlag(SState::NFlag, result & 0x8000);
	SetFlag(SState::SFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::VFlag));
	SetFlag(SState::ZFlag, !result);
	SetFlag(SState::CFlag, *int_reg & 0x8000 && !(result & 0x8000));
	*int_reg = result;
	return 2;
}

quint8 CCPU::LOGIC_exec()
{
	quint8& dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	quint8& source = m_sState.GR[(m_sState.IR[0] & 0x0200) >> 5 | (m_sState.IR[0] & 0x000F)];
	quint8 result;
	switch (m_sState.IR[0] & 0x0C00)
	{
	case 0x0000: result = dest & source; break;
	case 0x0400: result = dest ^ source; break;
	case 0x0800: result = dest | source; break;
	}

	if (m_sState.IR[0] & 0x1000)
		result += (m_sState.FLAGS & SState::CFlag);

	SetFlag(SState::VFlag, 0);
	SetFlag(SState::NFlag, result & 0x80);
	SetFlag(SState::SFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::VFlag));
	SetFlag(SState::ZFlag, !result);
	dest = result;
	return 1;
}

quint8 CCPU::LOGICI_exec()
{
	quint8& dest = m_sState.GR[((m_sState.IR[0] & 0x00F0) >> 4) + 16];
	quint8 value = ((m_sState.IR[0] & 0x0F00) >> 4) | (m_sState.IR[0] & 0x000F);
	quint8 result;
	switch (m_sState.IR[0] & 0xF000)
	{
	case 0x7000:
		result = dest & value;
		break;
	case 0x6000:
		result = dest | value;
		break;
	}

	if (m_sState.IR[0] & 0x1000)
		result += (m_sState.FLAGS & SState::CFlag);

	SetFlag(SState::VFlag, 0);
	SetFlag(SState::NFlag, result & 0x80);
	SetFlag(SState::SFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::VFlag));
	SetFlag(SState::ZFlag, !result);
	dest = result;
	return 1;
}

quint8 CCPU::ASR_exec()
{
	quint8& dest = m_sState.GR[((m_sState.IR[0] & 0x00F0) >> 4) + 16];
	quint8 result = dest >> 1;

	if (dest & 0x80)
		result |= 0x80;
	else
		result &= ~0x7F;

	SetFlag(SState::NFlag, result & 0x80);
	SetFlag(SState::ZFlag, !result);
	SetFlag(SState::CFlag, dest & 0x01);
	SetFlag(SState::VFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::CFlag));
	SetFlag(SState::SFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::VFlag));
	dest = result;
	return 1;
}

quint8 CCPU::SREG_exec()
{
	SetFlag(SState::EFlag((m_sState.IR[0] & 0x0070) >> 4),!(m_sState.IR[0] & 0x0080));
	return 1;
}

quint8 CCPU::TBIT_exec()
{
	quint8& data = m_sState.GR[m_sState.IR[0] & 0x01F0 >> 4];
	if (m_sState.IR[0] & 0x0200)
	{
		SetFlag(SState::TFlag, data & (1 << (m_sState.IR[0] & 0x0007)));
	}
	else
	{
		if (GetFlag(SState::TFlag))
			data |= 1 << (m_sState.IR[0] & 0x0007);
		else
			data &= ~(1 << (m_sState.IR[0] & 0x0007));
	}
	return 1;
}

quint8 CCPU::BRANCH_exec()
{
	qint8 offset = (m_sState.IR[0] & 0x03F8) >> 3;
	if (offset & 0x0040)
		offset |= 0x0080;
	if ((m_sState.IR[0] & 0x0400) && !GetFlag(SState::EFlag(1 << (m_sState.IR[0] & 0x0007))) ||
		!(m_sState.IR[0] & 0x0400) && GetFlag(SState::EFlag(1 << (m_sState.IR[0] & 0x0007))))
	{
		m_sState.PC += offset * 2;
		return 2;
	}
	return 1;
}

quint8 CCPU::BREAK_exec()
{
	m_sState.RUN = false;
	return 1;
}

quint8 CCPU::JMP_exec()
{
	if (m_sState.IR[0] & 0x0002)	// CALL
	{
		m_sState.SP -= 2;
		m_pRam->operator[]<quint16>(m_sState.SP) = m_sState.PC;
	}

	m_sState.PC = m_sState.IR[1] * 2;
	return 3;
}

quint8 CCPU::CBI_exec()
{
	quint8& io_reg = m_sState.IOR[(m_sState.IR[0] & 0x00F8) >> 3];
	io_reg &= ~(1 << (m_sState.IR[0] & 0x0007));
	return 2;
}

quint8 CCPU::SER_exec()
{
	quint8& reg = m_sState.GR[((m_sState.IR[0] & 0x00F0) >> 4) + 16];
	reg = 0xFF;
	return 1;
}

quint8 CCPU::COM_exec()
{
	quint8& dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	dest = 0xFF - dest;
	SetFlag(SState::VFlag, 0);
	SetFlag(SState::NFlag, dest & 0x80);
	SetFlag(SState::SFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::VFlag));
	SetFlag(SState::ZFlag, !dest);
	SetFlag(SState::CFlag, 1);
	return 1;
}

quint8 CCPU::CLR_exec()
{
	m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4] = 0;
	SetFlag(SState::SFlag, false);
	SetFlag(SState::VFlag, false);
	SetFlag(SState::NFlag, false);
	SetFlag(SState::ZFlag, true);

	return 1;
}

quint8 CCPU::CP_exec()
{
	quint8& dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	quint8& source = m_sState.GR[(m_sState.IR[0] & 0x0200) >> 5 | (m_sState.IR[0] & 0x000F)];
	quint8 result = dest - source;

	if (!(m_sState.IR[0] & 0x1000) && GetFlag(SState::CFlag))
		result -= 1;

	SetFlag(SState::VFlag, dest & 0x80 && !(source & 0x80) && !(result & 0x80) || !(dest & 0x80) && source & 0x80 && result & 0x80);
	SetFlag(SState::NFlag, dest & 0x80);
	SetFlag(SState::SFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::VFlag));

	if (!(m_sState.IR[0] & 0x1000) && GetFlag(SState::CFlag))
		SetFlag(SState::ZFlag, !result && GetFlag(SState::ZFlag));
	else
		SetFlag(SState::ZFlag, !result);

	SetFlag(SState::HFlag, !(dest & 0x04) && source & 0x04 || source & 0x04 && result & 0x04 || result & 0x04 && !(dest & 0x04));
	SetFlag(SState::CFlag, !(dest & 0x80) && source & 0x80 || source & 0x80 && result & 0x80 || result & 0x80 && !(dest & 0x80));
	return 1;
}

quint8 CCPU::CPI_exec()
{
	quint8& dest = m_sState.GR[((m_sState.IR[0] & 0x00F0) >> 4) + 16];
	quint8 value = ((m_sState.IR[0] & 0x0F00) >> 4) | (m_sState.IR[0] & 0x000F);
	quint8 result = dest - value;

	SetFlag(SState::VFlag, dest & 0x80 && !(value & 0x80) && !(result & 0x80) || !(dest & 0x80) && value & 0x80 && result & 0x80);
	SetFlag(SState::NFlag, dest & 0x80);
	SetFlag(SState::SFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::VFlag));
	SetFlag(SState::ZFlag, !result);
	SetFlag(SState::HFlag, !(dest & 0x04) && value & 0x04 || value & 0x04 && result & 0x04 || result & 0x04 && !(dest & 0x04));
	SetFlag(SState::CFlag, !(dest & 0x80) && value & 0x80 || value & 0x80 && result & 0x80 || result & 0x80 && !(dest & 0x80));
	return 1;
}

quint8 CCPU::CPSE_exec()
{
	quint8 dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	quint8 source = m_sState.GR[((m_sState.IR[0] & 0x0200) >> 5) | (m_sState.IR[0] & 0x000F)];
	quint8 size = Decode(m_sState.IR[1]);

	if (dest == source)
		m_sState.PC += size;
	else
		return 1;

	return size >> 1;
}

quint8 CCPU::DEC_exec()
{
	quint8& dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	--dest;
	SetFlag(SState::ZFlag, !dest);
	SetFlag(SState::NFlag, dest & 0x80);
	SetFlag(SState::VFlag, !(dest & 0x80) && dest & 0x40 && dest & 0x20 && dest & 0x10 && dest & 0x08 && dest & 0x04 && dest & 0x02 && dest & 0x01);
	SetFlag(SState::SFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::VFlag));
	return 1;
}

quint8 CCPU::IO_exec()
{
	quint8 reg_index = ((m_sState.IR[0] & 0x0600) >> 5) | (m_sState.IR[0] & 0x000F);
	quint8& port = m_sState.IOR[reg_index];
	quint8& reg = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	if (m_sState.IR[0] & 0x0800)
	{
		port = reg;
		emit IOChanged();
	}
	else
		reg = port;

	return 2;
}

quint8 CCPU::INC_exec()
{
	quint8& dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	++dest;
	SetFlag(SState::ZFlag, !dest);
	SetFlag(SState::NFlag, dest & 0x80);
	SetFlag(SState::VFlag, dest & 0x80 && !(dest & 0x40) && !(dest & 0x20) && !(dest & 0x10) && !(dest & 0x08) && !(dest & 0x04) && !(dest & 0x02) && !(dest & 0x01));
	SetFlag(SState::SFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::VFlag));
	return 1;
}

quint8 CCPU::JUMPS_exec()
{
	if (m_sState.IR[0] & 0x0100)	// check whether JMP or CALL
	{
		m_sState.SP -= 2;
		m_pRam->operator[]<quint16>(m_sState.SP) = m_sState.PC;
	}

	m_sState.PC = m_sState.Z();

	if (m_sState.IR[0] & 0x0100)
		return 4;

	return 2;
	// TODO: reimplement fo 22 bit PC
}

quint8 CCPU::LDI_exec()
{
	quint8& dest = m_sState.GR[((m_sState.IR[0] & 0x00F0) >> 4) + 16];
	quint8 value = ((m_sState.IR[0] & 0x0F00) >> 4) | (m_sState.IR[0] & 0x000F);
	dest = value;
	return 1;
}

quint8 CCPU::MOV_exec()
{
	quint8& dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	quint8& source = m_sState.GR[((m_sState.IR[0] & 0x0200) >> 5) | (m_sState.IR[0] & 0x000F)];
	dest = source;
	return 1;
}

quint8 CCPU::MOVW_exec()
{
	quint16& dest = (quint16&)m_sState.GR[(m_sState.IR[0] & 0x00F0) >> 3];
	quint16& source = (quint16&)m_sState.GR[(m_sState.IR[0] & 0x000F) << 1];
	dest = source;
	return 1;
}

quint8 CCPU::MUL_exec()
{
	quint8 dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	quint8 source = m_sState.GR[((m_sState.IR[0] & 0x0200) >> 5) | (m_sState.IR[0] & 0x000F)];
	quint16& result = (quint16&)m_sState.GR[0];
	result = dest * source;
	SetFlag(SState::CFlag, result & 0x8000);
	SetFlag(SState::ZFlag, !result);
	return 1;
}

quint8 CCPU::MULS_exec()
{
	qint8 dest = m_sState.GR[((m_sState.IR[0] & 0x00F0) >> 4) + 16];
	qint8 source = m_sState.GR[m_sState.IR[0] & 0x000F + 16];
	qint16& result = (qint16&)m_sState.GR[0];
	result = dest * source;
	SetFlag(SState::CFlag, result & 0x8000);
	SetFlag(SState::ZFlag, !result);
	return 1;
}

quint8 CCPU::MULSU_exec()
{
	qint8 dest = m_sState.GR[((m_sState.IR[0] & 0x0070) >> 4) + 16];
	quint8 source = m_sState.GR[m_sState.IR[0] & 0x0007 + 16];
	qint16& result = (qint16&)m_sState.GR[0];
	result = dest * source;
	SetFlag(SState::CFlag, result & 0x8000);
	SetFlag(SState::ZFlag, !result);
	return 2;
}

quint8 CCPU::NOP_exec()
{
	return 1;
}

quint8 CCPU::RET_exec()
{
	m_sState.PC = m_pRam->operator[]<quint16>(m_sState.SP);
	m_sState.SP += 2;

	if (m_sState.IR[0] & 0x0010)
		SetFlag(SState::IFlag, true);

	return 4;
}

quint8 CCPU::RJMP_exec()
{
	if (m_sState.IR[0] & 0x1000)
	{
		m_sState.SP -= 2;
		m_pRam->operator[]<quint16>(m_sState.SP) = m_sState.PC;
	}

	qint16 offset = m_sState.IR[0] & 0x0FFF;

	if (offset & 0x0800)
		offset |= 0xF000;

	m_sState.PC += offset * 2;
	return 2;
}

quint8 CCPU::SBI_exec()
{
	quint8& io_reg = m_sState.IOR[(m_sState.IR[0] & 0x00F8) >> 3];
	io_reg |= 1 << (m_sState.IR[0] & 0x0007);

	return 2;
}

quint8 CCPU::LDST_exec()
{
	quint8 clocks = 2;
	quint16 regold;
	quint16 reg;
	quint8& dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];

	switch (m_sState.IR[0] & 0x000C)
	{
	case 0x000C:
		reg = m_sState.X(); break;
	case 0x0004:
		reg = m_sState.Y(); break;
	case 0x0000:
		reg = m_sState.Z(); break;
	}

	regold = reg;

	if (m_sState.IR[0] & 0x0200)		// ST operations
	{
		switch (m_sState.IR[0] & 0x0003)
		{
		case 0x0000:
			m_pRam->operator[]<quint8>(reg) = dest; break;
		case 0x0001:
			m_pRam->operator[]<quint8>(reg++) = dest; break;
		case 0x0002:
			m_pRam->operator[]<quint8>(--reg) = dest; break;
		}
	}
	else								// LD operations
	{
		switch (m_sState.IR[0] & 0x0003)
		{
		case 0x0000:
			dest = m_pRam->operator[]<quint8>(reg); clocks = 1; break;
		case 0x0001:
			dest = m_pRam->operator[]<quint8>(reg++); break;
		case 0x0002:
			dest = m_pRam->operator[]<quint8>(--reg); clocks = 3; break;
		}
	}

	if (regold != reg)
	{
		switch (m_sState.IR[0] & 0x000C)
		{
		case 0x000C:
			m_sState.setX(reg); break;
		case 0x0004:
			m_sState.setY(reg); break;
		case 0x0000:
			m_sState.setZ(reg); break;
		}
	}

	return clocks;
}

quint8 CCPU::LDSTD_exec()
{
	quint16 reg;
	quint8& dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	quint8 value = ((m_sState.IR[0] & 0x2000) >> 8) | ((m_sState.IR[0] & 0x0C00) >> 7) | (m_sState.IR[0] & 0x0007);

	if (m_sState.IR[0] & 0x0008)
		reg = m_sState.Y();
	else
		reg = m_sState.Z();

	if (m_sState.IR[0] & 0x0002)	// ST operations
		m_pFlash->operator[]<quint8>(reg + value) = dest;
	else							// LD operations
		dest = m_pFlash->operator[]<quint8>(reg + value);

	return 2;
}

quint8 CCPU::LDSTS_exec()
{
	quint8& reg = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
	quint16 address = m_sState.IR[1];

	if (m_sState.IR[0] & 0x0200)	// STS
		m_pRam->operator[]<quint8>(address) = reg;
	else							// LDS
		reg = m_pRam->operator[]<quint8>(address);

	return 2;
}

quint8 CCPU::LPM_exec()
{
	if (m_sState.IR[0] & 0x0400)
		m_sState.GR[0] = m_pFlash->operator[]<quint8>(m_sState.Z());
	else
	{
		quint8& dest = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];
		dest = m_pFlash->operator[]<quint8>(m_sState.Z() & 0x7FFF);
		if (m_sState.IR[0] & 0x0001)
			m_sState.setZ(m_sState.Z() + 1);
	}

	return 3;
}

quint8 CCPU::SUBI_exec()
{
	quint8& dest = m_sState.GR[((m_sState.IR[0] & 0x00F0) >> 4) + 16];
	quint8 value = ((m_sState.IR[0] & 0x0F00) >> 4) | (m_sState.IR[0] & 0x000F);
	quint8 result = dest - value;

	if(!(m_sState.IR[0] & 0x1000))
		result -= GetFlag(SState::CFlag);

	SetFlag(SState::HFlag, !(dest & 0x04) && value & 0x04 || value & 0x04 && result & 0x04 || !(dest & 0x04) && result & 0x04);
	SetFlag(SState::VFlag, dest & 0x80 && !(value & 0x80) && !(result & 0x80) || !(dest & 0x80) && value & 0x80 && result & 0x80);
	SetFlag(SState::NFlag, result & 0x80);
	SetFlag(SState::SFlag, GetFlag(SState::NFlag) ^ GetFlag(SState::VFlag));
	SetFlag(SState::ZFlag, !result);
	SetFlag(SState::CFlag, !(dest & 0x80) && value & 0x80 || value & 0x80 && result & 0x80 || !(dest & 0x80) && result & 0x80);
	dest = result;
	return 1;
}

quint8 CCPU::SBIS_exec()
{
	quint8& io_register = m_sState.IOR[(m_sState.IR[0] & 0x00F8) >> 3];
	quint8 size = Decode(m_sState.IR[1]);

	if (io_register & (1 << (m_sState.IR[0] & 0x0007)))
		m_sState.PC += size;
	else
		return 1;

	return size / 2;
}

quint8 CCPU::STACK_exec()
{
	quint8& reg = m_sState.GR[(m_sState.IR[0] & 0x01F0) >> 4];

	if (m_sState.IR[0] & 0x0200)	// PUSH
	{
		m_sState.SP -= 2;
		m_pRam->operator[]<quint8>(m_sState.SP) = reg;
	}
	else							// POP
	{
		reg = m_pRam->operator[]<quint8>(m_sState.SP);
		m_sState.SP += 2;
	}

	return 2;
}