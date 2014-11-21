#if !defined _MODBUS_INCLUDED
#define _MODBUS_INCLUDED

#ifdef __cplusplus__
extern "C" {
#endif /* __cplusplus__ */

#define MODBUS_MAX_MSG_LENGTH	(59) // MAX(17, 11 + 4 * N), N - max words to read
#define MODBUS_READ_REQ_LENGTH	(17)
#define MODBUS_MAX_WORDS_READ	(12)

typedef enum
{
	MODBUS_READ = 3,
	MODBUS_WRITE = 6
} modbus_cmd_code_e;

typedef enum
{
	MODBUS_REQ = 0,
	MODBUS_ACK
} modbus_cmd_type_e;

typedef enum
{
	RESULT_OK = 0,
	RESULT_BAD_ACK,
	RESULT_BAD_MSG,
	RESULT_BAD_CRC,
	RESULT_BUFFER_OVERFLOW,
	RESULT_MSG_TOO_BIG,
	RESULT_ALARM,
	RESULT_TIMEOUT,
	RESULT_UNKNOWN_PORT,
	RESULT_IGNORE_CMD
} result_e;

typedef struct
{
/* 	device_id_e			device_id; */
	char				device_id;
	modbus_cmd_code_e	cmd_code;
	short int			addr;
	short int			value[MODBUS_MAX_WORDS_READ];
	modbus_cmd_type_e	cmd_type;
} modbus_cmd_s;

result_e modbus_byte2char(char /* byte */, char * /*buf */);
result_e modbus_char2byte(const char * /* buf */, char * /* byte */);
result_e modbus_word2char(short int /* word */, char * /* buf */);
result_e modbus_char2word(const char * /* buf */, short int * /* word */);
result_e modbus_crc(const char * /* msg */, short int /* length */, char * /* crc */);
result_e modbus_cmd2msg(const modbus_cmd_s * /* cmd */, 
	char * /* msg */, short int /* length */);
result_e modbus_msg2cmd(const char * /* msg */, modbus_cmd_s * /* cmd */);

#ifdef __cplusplus__
}
#endif /* __cplusplus__ */

#endif /* _MODBUS_INCLUDED */
