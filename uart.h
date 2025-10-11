#define F_CPU 16000000UL
#define BAUD 9600
#define UART_BAUD_RATE F_CPU / 16 / BAUD - 1

void uart_init(unsigned int baud);
void uart_transmit(unsigned char data);
void uart_print(const char *str);
unsigned char uart_receive(void);
char * uart_read_line(void);