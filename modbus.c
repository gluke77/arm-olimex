#include "common.h"
#include "modbus.h"
#include <string.h>

/*****************************************************************************/

result_e modbus_bits2char(char bits, char * buf)
{
	bits &= 0x0F;
	*buf = (bits < 0x0A)?('0' + bits):('A' - 0x0A + bits);
	return RESULT_OK;
}

/*****************************************************************************/

result_e modbus_byte2char(char byte, char * buf)
{
	modbus_bits2char(byte >> 4, buf);
	modbus_bits2char(byte & 0x0F, buf + 1);
	return RESULT_OK;
}

/*****************************************************************************/

result_e modbus_word2char(short int word, char * buf)
{
	modbus_byte2char(word >> 8, buf);
	modbus_byte2char((char)(word & 0x00FF), buf + 2);
	return RESULT_OK;
}

/*****************************************************************************/

result_e modbus_char2bits(const char ch, char * bits)
{
	if ('A' <= ch && ch <= 'F')
		*bits = ch - 'A' + 0x0A;
	else if ('0' <= ch && ch <= '9' )
		*bits = ch - '0';
	else
	{
		return RESULT_BAD_MSG;
	}

	return RESULT_OK;
}

/*****************************************************************************/

result_e modbus_char2byte(const char * buf, char * byte)
{
	result_e	res;
	char			tmp;
	
	if ((res = modbus_char2bits(*buf, &tmp)) != RESULT_OK)
		return res;
		
	*byte = tmp << 4;
	
	res = modbus_char2bits(*(buf + 1), &tmp);
	
	*byte += tmp;
	
	return res;
}

/*****************************************************************************/

result_e modbus_char2word(const char * buf, short int * word)
{
	result_e	res;
	char		tmp;
	
	if ((res = modbus_char2byte(buf, &tmp)) != RESULT_OK)
		return res;
		
	*word = (short int)tmp << 8;
	
	res = modbus_char2byte(buf + 2, &tmp);
	
	*word += (short int)tmp;
	
	return res;
}

/*****************************************************************************/

result_e modbus_crc(const char * msg, short int length, char * crc)
{
	char		acc;
	char 	byte;
	short int	tmp;
	result_e	res;
	
	for (acc = 0, tmp = 0; tmp < length; tmp += 2, acc += byte)
		if ((res = modbus_char2byte(msg + tmp, &byte)) != RESULT_OK)
			return res;
	
	modbus_byte2char((~acc)+1, crc);
	
	return RESULT_OK;
}

/*****************************************************************************/

result_e modbus_cmd2msg(const modbus_cmd_s * cmd, char * msg, short int length)
{
	short int	cmd_size;
	char		idx;
	result_e	res;
	char 	* pos;
	
	cmd_size = (cmd->cmd_type == MODBUS_ACK && cmd->cmd_code == MODBUS_READ)?MODBUS_MAX_MSG_LENGTH:17;
	
	if (length < cmd_size)
		return RESULT_BUFFER_OVERFLOW;
		
	pos = msg;
	
	*(pos++) = ':';
	
	modbus_byte2char(cmd->device_id, pos);
	pos += 2;
	
	modbus_byte2char((char)cmd->cmd_code, pos);
	pos += 2;
	
	if (cmd->cmd_type == MODBUS_ACK && cmd->cmd_code == MODBUS_READ)
	{
		modbus_byte2char((char)((cmd->addr * 2) & 0x00FF), pos);
		pos += 2;
	}
	else
	{
		modbus_word2char(cmd->addr, pos);
		pos += 4;
	}
	
	modbus_word2char(cmd->value[0], pos);
	pos += 4;
	
	if (cmd->cmd_type == MODBUS_ACK && cmd->cmd_code == MODBUS_READ)
		for (idx = 1; idx < cmd->addr; idx++)
		{
			modbus_word2char(cmd->value[idx], pos);
			pos += 4;
		}
	
	
	if ((res = modbus_crc(msg + 1, (short int)(pos - msg - 1), pos)) != RESULT_OK)
		return res;
	pos += 2;

	*pos = '\r';
	pos++;
	
	*pos = '\n';
	
	return RESULT_OK;
}

/*****************************************************************************/

result_e modbus_msg2cmd(const char * msg, modbus_cmd_s * cmd)
{
	short int		msg_length;
	char			tmp;
	result_e		res;
	char			crc[2];
		
	memset(cmd, 0x00, sizeof(modbus_cmd_s));
	
	for (msg_length = 0; msg_length < MODBUS_MAX_MSG_LENGTH; msg_length++)
		if (msg[msg_length] == '\r' && msg[msg_length + 1] == '\n')
			break;
			
	if (msg_length == MODBUS_MAX_MSG_LENGTH)
		return RESULT_MSG_TOO_BIG;
		
	msg_length += 2;
	
	if ((res = modbus_crc(msg + 1, msg_length - 5, crc)) != RESULT_OK)
		return res;
		
	if (*crc != *(msg + msg_length - 4) || *(crc + 1) != *(msg + msg_length - 3))
		return RESULT_BAD_CRC;
		
	msg++;
	if ((res = modbus_char2byte(msg, &(cmd->device_id))) != RESULT_OK)
		return res;
		
	msg += 2;
	if ((res = modbus_char2byte(msg, (char *)(&(cmd->cmd_code)))) != RESULT_OK)
		return res;
	
	if (cmd->cmd_code == MODBUS_READ && msg_length != MODBUS_READ_REQ_LENGTH)
	{
		cmd->cmd_type = MODBUS_ACK;
		msg += 2;
		if ((res = modbus_char2byte(msg, &tmp)) != RESULT_OK)
			return res;
		tmp /= 2;
		cmd->addr = (tmp < MODBUS_MAX_WORDS_READ)?tmp:MODBUS_MAX_WORDS_READ;
		msg += 2;

		for (tmp = 0; tmp < cmd->addr; tmp++, msg += 4)
			if ((res = modbus_char2word(msg, cmd->value + tmp)) != RESULT_OK)
				return res;
	}
	else
	{
		msg += 2;
		cmd->cmd_type = MODBUS_REQ;
		if ((res = modbus_char2word(msg, &(cmd->addr))) != RESULT_OK)
			return res;
		msg += 4;
		if ((res = modbus_char2word(msg, cmd->value)) != RESULT_OK)
			return res;
	}

	return RESULT_OK;
}
