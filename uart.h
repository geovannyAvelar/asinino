#define F_CPU 16000000UL
#define BAUD 9600
#define UART_BAUD_RATE F_CPU / 16 / BAUD - 1

void uart_init(unsigned int baud);
void uart_transmit(unsigned char data);
void uart_print(const char *str);
void uart_print_hex(unsigned int num);
void uart_print_int(int num);
unsigned char uart_receive(void);
void uart_read_line(char *buffer, unsigned int size);