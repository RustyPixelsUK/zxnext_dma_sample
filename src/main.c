/*******************************************************************************
 * Original ASM by David Saphier (em00k2020) / Z88DK port by Ben Baker (headkaze)
 * 
 * DMA sample demo for Sinclair ZX Spectrum Next.
 ******************************************************************************/

#include <arch/zxn.h>
#include <input.h>
#include <z80.h>
#include <im2.h>
#include <intrinsic.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "config/zconfig.h"

#include "zxnext/src/dma.h"
#include "zxnext/src/bank.h"
#include "zxnext/src/ctc.h"

#include "samples.h"
#include "globals.h"

#if USE_CTC

// CTC sample playback state
static volatile uint8_t *sample_ptr;
static volatile uint16_t sample_remaining;
static volatile uint8_t *sample_loop_ptr;
static volatile uint16_t sample_loop_length;
static volatile bool sample_playing;
static volatile bool sample_looping;

IM2_DEFINE_ISR(ctc_sample_isr)
{
    if (!sample_playing)
        return;

    z80_outp(SAMPLE_COVOX_PORT, *sample_ptr++);

    if (--sample_remaining == 0)
    {
        if (sample_looping)
        {
            sample_ptr = sample_loop_ptr;
            sample_remaining = sample_loop_length;
        }
        else
        {
            sample_playing = false;
            ctc_stop(CTC_SAMPLE_CHANNEL);
        }
    }
}

static void ctc_play_sample(void *source, uint16_t length, bool loop)
{
    // Stop any currently playing sample
    ctc_stop(CTC_SAMPLE_CHANNEL);
    sample_playing = false;

    // Set up sample state
    sample_ptr = (uint8_t *)source;
    sample_remaining = length;
    sample_loop_ptr = (uint8_t *)source;
    sample_loop_length = length;
    sample_looping = loop;
    sample_playing = true;

    // Start the CTC timer with interrupts
    ctc_init(CTC_SAMPLE_CHANNEL, CTC_SAMPLE_TC, CTC_SAMPLE_PRESCALER_256, true);
}

static void ctc_stop_sample(void)
{
    ctc_stop(CTC_SAMPLE_CHANNEL);
    sample_playing = false;
}

#endif

static uint8_t scalers[] =
{
    // Not used in the multisample demo
    SAMPLE_SCALER,SAMPLE_SCALER*2,SAMPLE_SCALER*3,SAMPLE_SCALER*4,SAMPLE_SCALER*5,
    SAMPLE_SCALER*6,SAMPLE_SCALER*7,SAMPLE_SCALER*8,SAMPLE_SCALER*9,SAMPLE_SCALER*10,SAMPLE_SCALER*12,
    0xff
};

typedef struct
{
    uint8_t page;
    bool loop;
    void *start;
    void *end;
} sample_table_t;

static sample_table_t sample_table[] =
{
    // bank, loop, start, end
    { 15, true,  mothership_start, mothership_end },
    { 15, false, steam_start,      steam_end },
    { 16, false, explode_start,    explode_end },
    { 17, true,  zapzapdii_start,  zapzapdii_end },
    { 17, false, dub1_start,       dub1_end },
    { 17, false, dub2_start,       dub2_end },
    { 17, false, dub3_start,       dub3_end },
    { 17, false, dub4_start,       dub4_end },
    { 17, true,  wawawawa_start,   wawawawa_end },
    { 18, true,  zzzzrrrttt_start, zzzzrrrttt_end },
    { 0,  false, 0xffff,           0xffff }
};

static void hardware_init(void)
{
    // Make sure the Spectrum ROM is paged in initially.
    IO_7FFD = IO_7FFD_ROM0;

    // Put Z80 in 28 MHz turbo mode.
    ZXN_NEXTREG(REG_TURBO_MODE, RTM_28MHZ);

    // Disable RAM memory contention.
    // ZXN_NEXTREGA(REG_PERIPHERAL_3, ZXN_READ_REG(REG_PERIPHERAL_3) | RP3_DISABLE_CONTENTION | RP3_ENABLE_TURBOSOUND);
}

static void isr_init(void)
{
    // Set up IM2 interrupt service routine:
    // Put Z80 in IM2 mode with a 257-byte interrupt vector table located
    // at 0x8000 (before CRT_ORG_CODE) filled with 0x81 bytes. Install an
    // interrupt service routine at the entry at address 0x8181.

    intrinsic_di();
    im2_init((void *) 0x8000);
    memset((void *) 0x8000, 0x81, 257);
#if USE_CTC
    // Install CTC sample ISR via JP instruction
    z80_bpoke(0x8181, 0xC3);
    z80_wpoke(0x8182, (uint16_t)ctc_sample_isr);
#else
    // Install empty ISR (EI; RETI)
    z80_bpoke(0x8181, 0xFB);
    z80_bpoke(0x8182, 0xED);
    z80_bpoke(0x8183, 0x4D);
#endif
    intrinsic_ei();
}

int main(void)
{
    int sample_index = 0;

    hardware_init();
    isr_init();

    printCls();
    printAt(10, 8, "Enjoy the sound!\n");
    printAt(12, 1, "Press any key to cycle samples\n");

    while (true)
    {
        if (in_inkey() != 0)
        {
            in_wait_nokey();
            
            bank_set_8k(SAMPLE_MMU, sample_table[sample_index].page);
            uint8_t *sample_source = zxn_addr_from_mmu(SAMPLE_MMU) + (uint16_t)sample_table[sample_index].start;
            uint16_t sample_length = (uint16_t)sample_table[sample_index].end - (uint16_t)sample_table[sample_index].start;
            
#if USE_CTC
            ctc_play_sample((void *)sample_source, sample_length, sample_table[sample_index].loop);
#else
            dma_transfer_sample((void *)sample_source, sample_length, SAMPLE_SCALER*12, sample_table[sample_index].loop);
#endif

            if (sample_table[++sample_index].start == (void *)0xffff)
                sample_index = 0;
            
            intrinsic_halt();
        }
    }
    
    // Trig a soft reset. The Next hardware registers and I/O ports will be reset by NextZXOS after a soft reset.
    ZXN_NEXTREG(REG_RESET, RR_SOFT_RESET);

    return 0;
}
