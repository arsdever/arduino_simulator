#include "avr_base.h"

#include <ram>

CAVRBase::CAVRBase(quint32 flash_size, quint32 memory_size, quint32 eeprom_size, quint32 ext_ioreg_count)
	: CCPU()
	, __state(memory_size + ext_ioreg_count + 96)
	, __ram(new CRAM(__state.GetGeneralRegisterSet() + ext_ioreg_count + 96, memory_size))
	, __flash(new CRAM(flash_size))
{}

CAVRBase::~CAVRBase()
{}

void CAVRBase::Fetch()
{
	if (__flash == nullptr)
		throw ram_not_found_exception();
	
	__state.IR[0] = __flash->operator[]<quint16>(__state.PC);
	__state.IR[1] = __flash->operator[]<quint16>(__state.PC + 2);
}

void CAVRBase::Step()
{
	CallFunction<vm::ILogger>(vm::ILogger::InfoFunctor(QString("Running 0x%1").arg(__state.PC, 8, 16, QChar('0'))));

	Fetch();
	__state.PC += Decode();
	SetInstrucitonDuration(Execute());
}

quint8 CAVRBase::Decode(quint16 instruction)
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
			switch (__state.IR[0] & 0x000F)
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
			switch (__state.IR[0] & 0x000F)
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

	__executer = CAVRBase::s_mapInstructions[code];
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

quint8 CAVRBase::Decode()
{
	return Decode(__state.IR[0]);
}

quint8 CAVRBase::Execute()
{
	return std::invoke(__executer, this);
}

void CAVRBase::Init()
{
	__state.SP = __ram->GetSize();
	__state.PC = 0;
	__state.RUN = true;
}

CRAM * CAVRBase::GetRam() const
{
	return __flash;
}

void CAVRBase::SetRam(CRAM *ram)
{
	__flash = ram;
}

CCPU::SState* CAVRBase::GetState() const
{
	return (SState*)&__state;
}

quint8 * CAVRBase::GetIOPorts() const
{
	return __state.IOR;
}

CAVRBase::InstructionMap CAVRBase::s_mapInstructions = CAVRBase::InitMap();

CAVRBase::InstructionMap CAVRBase::InitMap()
{
	InstructionMap map;
	map.insert(CAVRBase::ADD, &CAVRBase::ARITHM_exec);
	map.insert(CAVRBase::ADC, &CAVRBase::ARITHM_exec);
	map.insert(CAVRBase::ADIW, &CAVRBase::ARITHM_IW_exec);
	map.insert(CAVRBase::AND, &CAVRBase::LOGIC_exec);
	map.insert(CAVRBase::ANDI, &CAVRBase::LOGICI_exec);
	map.insert(CAVRBase::ASR, &CAVRBase::ASR_exec);
	map.insert(CAVRBase::BCLR, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::BLD, &CAVRBase::TBIT_exec);
	map.insert(CAVRBase::BRBC, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRBS, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRCC, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRCS, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BREAK, &CAVRBase::BREAK_exec);
	map.insert(CAVRBase::BREQ, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRGE, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRHC, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRHS, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRID, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRIE, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRLO, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRLT, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRMI, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRNE, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRPL, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRSH, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRTC, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRTS, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRVC, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BRVS, &CAVRBase::BRANCH_exec);
	map.insert(CAVRBase::BSET, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::BST, &CAVRBase::TBIT_exec);
	map.insert(CAVRBase::CALL, &CAVRBase::JMP_exec);
	map.insert(CAVRBase::CBI, &CAVRBase::CBI_exec);
	map.insert(CAVRBase::CLC, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::CLH, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::CLI, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::CLN, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::CLR, &CAVRBase::CLR_exec);
	map.insert(CAVRBase::CLS, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::CLT, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::CLV, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::CLZ, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::COM, &CAVRBase::COM_exec);
	map.insert(CAVRBase::CP, &CAVRBase::CP_exec);
	map.insert(CAVRBase::CPC, &CAVRBase::CP_exec);
	map.insert(CAVRBase::CPI, &CAVRBase::CPI_exec);
	map.insert(CAVRBase::CPSE, &CAVRBase::CPSE_exec);
	map.insert(CAVRBase::DEC, &CAVRBase::DEC_exec);
	map.insert(CAVRBase::EICALL, &CAVRBase::JUMPS_exec);
	map.insert(CAVRBase::EIJMP, &CAVRBase::JUMPS_exec);
	map.insert(CAVRBase::EOR, &CAVRBase::LOGIC_exec);
	map.insert(CAVRBase::ICALL, &CAVRBase::JUMPS_exec);
	map.insert(CAVRBase::IJMP, &CAVRBase::JUMPS_exec);
	map.insert(CAVRBase::IN, &CAVRBase::IO_exec);
	map.insert(CAVRBase::INC, &CAVRBase::INC_exec);
	map.insert(CAVRBase::JMP, &CAVRBase::JMP_exec);
	map.insert(CAVRBase::LD, &CAVRBase::LDST_exec);
	map.insert(CAVRBase::LDD, &CAVRBase::LDSTD_exec);
	map.insert(CAVRBase::LDI, &CAVRBase::LDI_exec);
	map.insert(CAVRBase::LDS, &CAVRBase::LDSTS_exec);
	map.insert(CAVRBase::LPM, &CAVRBase::LPM_exec);
	map.insert(CAVRBase::MOV, &CAVRBase::MOV_exec);
	map.insert(CAVRBase::MOVW, &CAVRBase::MOVW_exec);
	map.insert(CAVRBase::MUL, &CAVRBase::MUL_exec);
	map.insert(CAVRBase::MULS, &CAVRBase::MULS_exec);
	map.insert(CAVRBase::MULSU, &CAVRBase::MULSU_exec);
	map.insert(CAVRBase::NOP, &CAVRBase::NOP_exec);
	map.insert(CAVRBase::OUT, &CAVRBase::IO_exec);
	map.insert(CAVRBase::ORI, &CAVRBase::LOGICI_exec);
	map.insert(CAVRBase::OR, &CAVRBase::LOGIC_exec);
	map.insert(CAVRBase::POP, &CAVRBase::STACK_exec);
	map.insert(CAVRBase::PUSH, &CAVRBase::STACK_exec);
	map.insert(CAVRBase::RCALL, &CAVRBase::RJMP_exec);
	map.insert(CAVRBase::RET, &CAVRBase::RET_exec);
	map.insert(CAVRBase::RETI, &CAVRBase::RET_exec);
	map.insert(CAVRBase::RJMP, &CAVRBase::RJMP_exec);
	map.insert(CAVRBase::SBI, &CAVRBase::SBI_exec);
	map.insert(CAVRBase::SBC, &CAVRBase::ARITHM_exec);
	map.insert(CAVRBase::SBCI, &CAVRBase::SUBI_exec);
	map.insert(CAVRBase::SBIS, &CAVRBase::SBIS_exec);
	map.insert(CAVRBase::SBIW, &CAVRBase::ARITHM_IW_exec);
	map.insert(CAVRBase::SEC, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::SEH, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::SEI, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::SEN, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::SER, &CAVRBase::SER_exec);
	map.insert(CAVRBase::SES, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::SET, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::SEV, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::SEZ, &CAVRBase::SREG_exec);
	map.insert(CAVRBase::ST, &CAVRBase::LDST_exec);
	map.insert(CAVRBase::STD, &CAVRBase::LDSTD_exec);
	map.insert(CAVRBase::STS, &CAVRBase::LDSTS_exec);
	map.insert(CAVRBase::SUB, &CAVRBase::ARITHM_exec);
	map.insert(CAVRBase::SUBI, &CAVRBase::SUBI_exec);
	return map;
}

// instruction implementation

quint8 CAVRBase::ARITHM_exec()
{
	quint8& dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	quint8& source = __state.GR[(__state.IR[0] & 0x0200) >> 5 | (__state.IR[0] & 0x000F)];
	quint8 result = 0;

	switch (__state.IR[0] & 0x1400)
	{
	case 0x1400:
		result = dest + source + __state.GetFlag(SAVRState::CFlag); break;
	case 0x0400:
		result = dest + source; break;
	case 0x0000:
		result = dest - source - __state.GetFlag(SAVRState::CFlag); break;
	case 0x1000:
		result = dest - source; break;
	}

	__state.SetFlag(SAVRState::HFlag, dest & 0x04 && source & 0x04 || source & 0x04 && !(result & 0x04) || dest & 0x04 && !(result & 0x04));
	__state.SetFlag(SAVRState::VFlag, dest & 0x80 && source & 0x80 && !(result & 0x80) || !(dest & 0x80) && !(source & 0x80) && result & 0x80);
	__state.SetFlag(SAVRState::NFlag, result & 0x80);
	__state.SetFlag(SAVRState::SFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::VFlag));
	__state.SetFlag(SAVRState::ZFlag, !result);
	__state.SetFlag(SAVRState::CFlag, dest & 0x80 && source & 0x80 || source & 0x80 && !(result & 0x80) || dest & 0x80 && !(result & 0x80));
	dest = result;
	return 1;
}

quint8 CAVRBase::ARITHM_IW_exec()
{
	quint16* int_reg;
	switch (__state.IR[0] & 0x0030)
	{
	case 0x0000: int_reg = &(quint16&)__state.GR[24]; break;
	case 0x0010: int_reg = &(quint16&)__state.GR[26]; break;
	case 0x0020: int_reg = &(quint16&)__state.GR[28]; break;
	case 0x0030: int_reg = &(quint16&)__state.GR[30]; break;
	}

	quint16 value = ((__state.IR[0] & 0x00C0) >> 2) | (__state.IR[0] & 0x000F);
	quint16 result;

	if (__state.IR[0] & 0x0100)
		result = *int_reg - value;
	else
		result = *int_reg + value;

	__state.SetFlag(SAVRState::VFlag, !(*int_reg & 0x8000) && result & 0x8000);
	__state.SetFlag(SAVRState::NFlag, result & 0x8000);
	__state.SetFlag(SAVRState::SFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::VFlag));
	__state.SetFlag(SAVRState::ZFlag, !result);
	__state.SetFlag(SAVRState::CFlag, *int_reg & 0x8000 && !(result & 0x8000));
	*int_reg = result;
	return 2;
}

quint8 CAVRBase::LOGIC_exec()
{
	quint8& dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	quint8& source = __state.GR[(__state.IR[0] & 0x0200) >> 5 | (__state.IR[0] & 0x000F)];
	quint8 result;
	switch (__state.IR[0] & 0x0C00)
	{
	case 0x0000: result = dest & source; break;
	case 0x0400: result = dest ^ source; break;
	case 0x0800: result = dest | source; break;
	}

	if (__state.IR[0] & 0x1000)
		result += (__state.FLAGS & SAVRState::CFlag);

	__state.SetFlag(SAVRState::VFlag, 0);
	__state.SetFlag(SAVRState::NFlag, result & 0x80);
	__state.SetFlag(SAVRState::SFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::VFlag));
	__state.SetFlag(SAVRState::ZFlag, !result);
	dest = result;
	return 1;
}

quint8 CAVRBase::LOGICI_exec()
{
	quint8& dest = __state.GR[((__state.IR[0] & 0x00F0) >> 4) + 16];
	quint8 value = ((__state.IR[0] & 0x0F00) >> 4) | (__state.IR[0] & 0x000F);
	quint8 result;
	switch (__state.IR[0] & 0xF000)
	{
	case 0x7000:
		result = dest & value;
		break;
	case 0x6000:
		result = dest | value;
		break;
	}

	if (__state.IR[0] & 0x1000)
		result += (__state.FLAGS & SAVRState::CFlag);

	__state.SetFlag(SAVRState::VFlag, 0);
	__state.SetFlag(SAVRState::NFlag, result & 0x80);
	__state.SetFlag(SAVRState::SFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::VFlag));
	__state.SetFlag(SAVRState::ZFlag, !result);
	dest = result;
	return 1;
}

quint8 CAVRBase::ASR_exec()
{
	quint8& dest = __state.GR[((__state.IR[0] & 0x00F0) >> 4) + 16];
	quint8 result = dest >> 1;

	if (dest & 0x80)
		result |= 0x80;
	else
		result &= ~0x7F;

	__state.SetFlag(SAVRState::NFlag, result & 0x80);
	__state.SetFlag(SAVRState::ZFlag, !result);
	__state.SetFlag(SAVRState::CFlag, dest & 0x01);
	__state.SetFlag(SAVRState::VFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::CFlag));
	__state.SetFlag(SAVRState::SFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::VFlag));
	dest = result;
	return 1;
}

quint8 CAVRBase::SREG_exec()
{
	__state.SetFlag(SAVRState::EFlag((__state.IR[0] & 0x0070) >> 4), !(__state.IR[0] & 0x0080));
	return 1;
}

quint8 CAVRBase::TBIT_exec()
{
	quint8& data = __state.GR[__state.IR[0] & 0x01F0 >> 4];
	if (__state.IR[0] & 0x0200)
	{
		__state.SetFlag(SAVRState::TFlag, data & (1 << (__state.IR[0] & 0x0007)));
	}
	else
	{
		if (__state.GetFlag(SAVRState::TFlag))
			data |= 1 << (__state.IR[0] & 0x0007);
		else
			data &= ~(1 << (__state.IR[0] & 0x0007));
	}
	return 1;
}

quint8 CAVRBase::BRANCH_exec()
{
	qint8 offset = (__state.IR[0] & 0x03F8) >> 3;
	if (offset & 0x0040)
		offset |= 0x0080;
	if ((__state.IR[0] & 0x0400) && !__state.GetFlag(SAVRState::EFlag(1 << (__state.IR[0] & 0x0007))) ||
		!(__state.IR[0] & 0x0400) && __state.GetFlag(SAVRState::EFlag(1 << (__state.IR[0] & 0x0007))))
	{
		__state.PC += offset * 2;
		return 2;
	}
	return 1;
}

quint8 CAVRBase::BREAK_exec()
{
	__state.RUN = false;
	return 1;
}

quint8 CAVRBase::JMP_exec()
{
	if (__state.IR[0] & 0x0002)	// CALL
	{
		CallFunction<vm::ILogger>(vm::ILogger::InfoFunctor(QString("call 0x%1, SP 0x%2 -> 0x%3")
			.arg(__state.IR[1] * 2, 8, 16, QChar('0'))
			.arg(__state.SP, 8, 16, QChar('0'))
			.arg(__state.SP - 2, 8, 16, QChar('0'))));
		__state.SP -= 2;
		__ram->operator[]<quint16>(__state.SP) = __state.PC;
	}

	__state.PC = __state.IR[1] * 2;
	return 3;
}

quint8 CAVRBase::CBI_exec()
{
	quint8& io_reg = __state.IOR[(__state.IR[0] & 0x00F8) >> 3];
	io_reg &= ~(1 << (__state.IR[0] & 0x0007));
	return 2;
}

quint8 CAVRBase::SER_exec()
{
	quint8& reg = __state.GR[((__state.IR[0] & 0x00F0) >> 4) + 16];
	reg = 0xFF;
	return 1;
}

quint8 CAVRBase::COM_exec()
{
	quint8& dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	dest = 0xFF - dest;
	__state.SetFlag(SAVRState::VFlag, 0);
	__state.SetFlag(SAVRState::NFlag, dest & 0x80);
	__state.SetFlag(SAVRState::SFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::VFlag));
	__state.SetFlag(SAVRState::ZFlag, !dest);
	__state.SetFlag(SAVRState::CFlag, 1);
	return 1;
}

quint8 CAVRBase::CLR_exec()
{
	__state.GR[(__state.IR[0] & 0x01F0) >> 4] = 0;
	__state.SetFlag(SAVRState::SFlag, false);
	__state.SetFlag(SAVRState::VFlag, false);
	__state.SetFlag(SAVRState::NFlag, false);
	__state.SetFlag(SAVRState::ZFlag, true);

	return 1;
}

quint8 CAVRBase::CP_exec()
{
	quint8& dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	quint8& source = __state.GR[(__state.IR[0] & 0x0200) >> 5 | (__state.IR[0] & 0x000F)];
	quint8 result = dest - source;

	if (!(__state.IR[0] & 0x1000) && __state.GetFlag(SAVRState::CFlag))
		result -= 1;

	__state.SetFlag(SAVRState::VFlag, dest & 0x80 && !(source & 0x80) && !(result & 0x80) || !(dest & 0x80) && source & 0x80 && result & 0x80);
	__state.SetFlag(SAVRState::NFlag, dest & 0x80);
	__state.SetFlag(SAVRState::SFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::VFlag));

	if (!(__state.IR[0] & 0x1000) && __state.GetFlag(SAVRState::CFlag))
		__state.SetFlag(SAVRState::ZFlag, !result && __state.GetFlag(SAVRState::ZFlag));
	else
		__state.SetFlag(SAVRState::ZFlag, !result);

	__state.SetFlag(SAVRState::HFlag, !(dest & 0x04) && source & 0x04 || source & 0x04 && result & 0x04 || result & 0x04 && !(dest & 0x04));
	__state.SetFlag(SAVRState::CFlag, !(dest & 0x80) && source & 0x80 || source & 0x80 && result & 0x80 || result & 0x80 && !(dest & 0x80));
	return 1;
}

quint8 CAVRBase::CPI_exec()
{
	quint8& dest = __state.GR[((__state.IR[0] & 0x00F0) >> 4) + 16];
	quint8 value = ((__state.IR[0] & 0x0F00) >> 4) | (__state.IR[0] & 0x000F);
	quint8 result = dest - value;

	__state.SetFlag(SAVRState::VFlag, dest & 0x80 && !(value & 0x80) && !(result & 0x80) || !(dest & 0x80) && value & 0x80 && result & 0x80);
	__state.SetFlag(SAVRState::NFlag, dest & 0x80);
	__state.SetFlag(SAVRState::SFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::VFlag));
	__state.SetFlag(SAVRState::ZFlag, !result);
	__state.SetFlag(SAVRState::HFlag, !(dest & 0x04) && value & 0x04 || value & 0x04 && result & 0x04 || result & 0x04 && !(dest & 0x04));
	__state.SetFlag(SAVRState::CFlag, !(dest & 0x80) && value & 0x80 || value & 0x80 && result & 0x80 || result & 0x80 && !(dest & 0x80));
	return 1;
}

quint8 CAVRBase::CPSE_exec()
{
	quint8 dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	quint8 source = __state.GR[((__state.IR[0] & 0x0200) >> 5) | (__state.IR[0] & 0x000F)];
	quint8 size = Decode(__state.IR[1]);

	if (dest == source)
		__state.PC += size;
	else
		return 1;

	return size >> 1;
}

quint8 CAVRBase::DEC_exec()
{
	quint8& dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	--dest;
	__state.SetFlag(SAVRState::ZFlag, !dest);
	__state.SetFlag(SAVRState::NFlag, dest & 0x80);
	__state.SetFlag(SAVRState::VFlag, !(dest & 0x80) && dest & 0x40 && dest & 0x20 && dest & 0x10 && dest & 0x08 && dest & 0x04 && dest & 0x02 && dest & 0x01);
	__state.SetFlag(SAVRState::SFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::VFlag));
	return 1;
}

quint8 CAVRBase::IO_exec()
{
	quint8 reg_index = ((__state.IR[0] & 0x0600) >> 5) | (__state.IR[0] & 0x000F);
	quint8& port = __state.IOR[reg_index];
	quint8& reg = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	if (__state.IR[0] & 0x0800)
	{
		port = reg;
		emit IOChanged(port);
	}
	else
		reg = port;

	return 2;
}

quint8 CAVRBase::INC_exec()
{
	quint8& dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	++dest;
	__state.SetFlag(SAVRState::ZFlag, !dest);
	__state.SetFlag(SAVRState::NFlag, dest & 0x80);
	__state.SetFlag(SAVRState::VFlag, dest & 0x80 && !(dest & 0x40) && !(dest & 0x20) && !(dest & 0x10) && !(dest & 0x08) && !(dest & 0x04) && !(dest & 0x02) && !(dest & 0x01));
	__state.SetFlag(SAVRState::SFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::VFlag));
	return 1;
}

quint8 CAVRBase::JUMPS_exec()
{
	if (__state.IR[0] & 0x0100)	// check whether JMP or CALL
	{
		__state.SP -= 2;
		__ram->operator[]<quint16>(__state.SP) = __state.PC;
	}

	__state.PC = __state.Z();

	if (__state.IR[0] & 0x0100)
		return 4;

	return 2;
	// TODO: reimplement fo 22 bit PC
}

quint8 CAVRBase::LDI_exec()
{
	quint8& dest = __state.GR[((__state.IR[0] & 0x00F0) >> 4) + 16];
	quint8 value = ((__state.IR[0] & 0x0F00) >> 4) | (__state.IR[0] & 0x000F);
	dest = value;
	return 1;
}

quint8 CAVRBase::MOV_exec()
{
	quint8& dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	quint8& source = __state.GR[((__state.IR[0] & 0x0200) >> 5) | (__state.IR[0] & 0x000F)];
	dest = source;
	return 1;
}

quint8 CAVRBase::MOVW_exec()
{
	quint16& dest = (quint16&)__state.GR[(__state.IR[0] & 0x00F0) >> 3];
	quint16& source = (quint16&)__state.GR[(__state.IR[0] & 0x000F) << 1];
	dest = source;
	return 1;
}

quint8 CAVRBase::MUL_exec()
{
	quint8 dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	quint8 source = __state.GR[((__state.IR[0] & 0x0200) >> 5) | (__state.IR[0] & 0x000F)];
	quint16& result = (quint16&)__state.GR[0];
	result = dest * source;
	__state.SetFlag(SAVRState::CFlag, result & 0x8000);
	__state.SetFlag(SAVRState::ZFlag, !result);
	return 1;
}

quint8 CAVRBase::MULS_exec()
{
	qint8 dest = __state.GR[((__state.IR[0] & 0x00F0) >> 4) + 16];
	qint8 source = __state.GR[__state.IR[0] & 0x000F + 16];
	qint16& result = (qint16&)__state.GR[0];
	result = dest * source;
	__state.SetFlag(SAVRState::CFlag, result & 0x8000);
	__state.SetFlag(SAVRState::ZFlag, !result);
	return 1;
}

quint8 CAVRBase::MULSU_exec()
{
	qint8 dest = __state.GR[((__state.IR[0] & 0x0070) >> 4) + 16];
	quint8 source = __state.GR[__state.IR[0] & 0x0007 + 16];
	qint16& result = (qint16&)__state.GR[0];
	result = dest * source;
	__state.SetFlag(SAVRState::CFlag, result & 0x8000);
	__state.SetFlag(SAVRState::ZFlag, !result);
	return 2;
}

quint8 CAVRBase::NOP_exec()
{
	return 1;
}

quint8 CAVRBase::RET_exec()
{
	__state.PC = __ram->operator[]<quint16>(__state.SP);
	__state.SP += 2;

	if (__state.IR[0] & 0x0010)
		__state.SetFlag(SAVRState::IFlag, true);

	return 4;
}

quint8 CAVRBase::RJMP_exec()
{
	if (__state.IR[0] & 0x1000)
	{
		__state.SP -= 2;
		__ram->operator[]<quint16>(__state.SP) = __state.PC;
	}

	qint16 offset = __state.IR[0] & 0x0FFF;

	if (offset & 0x0800)
		offset |= 0xF000;

	__state.PC += offset * 2;
	return 2;
}

quint8 CAVRBase::SBI_exec()
{
	quint8& io_reg = __state.IOR[(__state.IR[0] & 0x00F8) >> 3];
	io_reg |= 1 << (__state.IR[0] & 0x0007);

	return 2;
}

quint8 CAVRBase::LDST_exec()
{
	quint8 clocks = 2;
	quint16 regold;
	quint16 reg;
	quint8& dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];

	switch (__state.IR[0] & 0x000C)
	{
	case 0x000C:
		reg = __state.X(); break;
	case 0x0004:
		reg = __state.Y(); break;
	case 0x0000:
		reg = __state.Z(); break;
	}

	regold = reg;

	if (__state.IR[0] & 0x0200)		// ST operations
	{
		switch (__state.IR[0] & 0x0003)
		{
		case 0x0000:
			__ram->operator[]<quint8>(reg) = dest; break;
		case 0x0001:
			__ram->operator[]<quint8>(reg++) = dest; break;
		case 0x0002:
			__ram->operator[]<quint8>(--reg) = dest; break;
		}
	}
	else								// LD operations
	{
		switch (__state.IR[0] & 0x0003)
		{
		case 0x0000:
			dest = __ram->operator[]<quint8>(reg); clocks = 1; break;
		case 0x0001:
			dest = __ram->operator[]<quint8>(reg++); break;
		case 0x0002:
			dest = __ram->operator[]<quint8>(--reg); clocks = 3; break;
		}
	}

	if (regold != reg)
	{
		switch (__state.IR[0] & 0x000C)
		{
		case 0x000C:
			__state.X() = reg; break;
		case 0x0004:
			__state.Y() = reg; break;
		case 0x0000:
			__state.Z() = reg; break;
		}
	}

	return clocks;
}

quint8 CAVRBase::LDSTD_exec()
{
	quint16 reg;
	quint8& dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	quint8 value = ((__state.IR[0] & 0x2000) >> 8) | ((__state.IR[0] & 0x0C00) >> 7) | (__state.IR[0] & 0x0007);

	if (__state.IR[0] & 0x0008)
		reg = __state.Y();
	else
		reg = __state.Z();

	if (__state.IR[0] & 0x0002)	// ST operations
		__flash->operator[]<quint8>(reg + value) = dest;
	else							// LD operations
		dest = __flash->operator[]<quint8>(reg + value);

	return 2;
}

quint8 CAVRBase::LDSTS_exec()
{
	quint8& reg = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
	quint16 address = __state.IR[1];

	if (__state.IR[0] & 0x0200)	// STS
		__ram->operator[]<quint8>(address) = reg;
	else							// LDS
		reg = __ram->operator[]<quint8>(address);

	return 2;
}

quint8 CAVRBase::LPM_exec()
{
	if (__state.IR[0] & 0x0400)
		__state.GR[0] = __flash->operator[]<quint8>(__state.Z());
	else
	{
		quint8& dest = __state.GR[(__state.IR[0] & 0x01F0) >> 4];
		dest = __flash->operator[]<quint8>(__state.Z() & 0x7FFF);
		if (__state.IR[0] & 0x0001)
			++__state.Z();
	}

	return 3;
}

quint8 CAVRBase::SUBI_exec()
{
	quint8& dest = __state.GR[((__state.IR[0] & 0x00F0) >> 4) + 16];
	quint8 value = ((__state.IR[0] & 0x0F00) >> 4) | (__state.IR[0] & 0x000F);
	quint8 result = dest - value;

	if (!(__state.IR[0] & 0x1000))
		result -= __state.GetFlag(SAVRState::CFlag);

	__state.SetFlag(SAVRState::HFlag, !(dest & 0x04) && value & 0x04 || value & 0x04 && result & 0x04 || !(dest & 0x04) && result & 0x04);
	__state.SetFlag(SAVRState::VFlag, dest & 0x80 && !(value & 0x80) && !(result & 0x80) || !(dest & 0x80) && value & 0x80 && result & 0x80);
	__state.SetFlag(SAVRState::NFlag, result & 0x80);
	__state.SetFlag(SAVRState::SFlag, __state.GetFlag(SAVRState::NFlag) ^ __state.GetFlag(SAVRState::VFlag));
	__state.SetFlag(SAVRState::ZFlag, !result);
	__state.SetFlag(SAVRState::CFlag, !(dest & 0x80) && value & 0x80 || value & 0x80 && result & 0x80 || !(dest & 0x80) && result & 0x80);
	dest = result;
	return 1;
}

quint8 CAVRBase::SBIS_exec()
{
	quint8& io_register = __state.IOR[(__state.IR[0] & 0x00F8) >> 3];
	quint8 size = Decode(__state.IR[1]);

	if (io_register & (1 << (__state.IR[0] & 0x0007)))
		__state.PC += size;
	else
		return 1;

	return size / 2;
}

quint8 CAVRBase::STACK_exec()
{
	quint8& reg = __state.GR[(__state.IR[0] & 0x01F0) >> 4];

	if (__state.IR[0] & 0x0200)	// PUSH
	{
		__state.SP -= 2;
		__ram->operator[]<quint8>(__state.SP) = reg;
	}
	else							// POP
	{
		reg = __ram->operator[]<quint8>(__state.SP);
		__state.SP += 2;
	}

	return 2;
}