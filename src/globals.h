// Set to 1 to use CTC timer ISR for sample playback instead of DMA
#define USE_CTC					0

// CTC channel to use for sample playback (0-7)
#define CTC_SAMPLE_CHANNEL		0

// CTC time constant for sample rate
// Rate = clk28 / (prescaler * time_constant)
// At 28 MHz with prescaler 16: 28000000 / (16 * 219) = ~7990 Hz
#define CTC_SAMPLE_TC			219

// Set to 1 to use CTC prescaler 256 instead of 16
#define CTC_SAMPLE_PRESCALER_256	0

#define SAMPLE_BUFFER_SIZE      8192

#define printCls()				printf("%c", 12)
#define printAt(row, col, str)	printf("\x16%c%c%s", (col + 1), (row + 1), (str))
