# STM32 Bare-Metal USART2 + DMA (Nucleo-F401RE/F411RE)

Register-level (no HAL peripheral drivers) USART2 project using DMA instead of polling to move data — part of an ongoing bare-metal STM32 learning series.

## What it does

- **TX:** On boot, DMA1 Stream6 (normal/one-shot mode) transfers a fixed string out over USART2 with zero CPU involvement — no `while(!TXE)` polling loop needed.
- **RX:** DMA1 Stream5 (circular mode) continuously receives 5-byte chunks into a buffer. A Transfer-Complete interrupt fires every time the buffer fills, toggling the onboard LED (PA5) as proof of reception, before the buffer is safely reused for the next chunk.

## Why DMA instead of polling

In polling mode, the CPU has to sit and check the USART status register for every single byte. With DMA, USART2 raises a request whenever it's ready, and the DMA controller moves the byte automatically in hardware — freeing the CPU to do other work. This project's main goal was learning that hardware handshake: DMA stream/channel selection, direction, memory increment, circular vs. normal mode, and enabling DMA requests on the peripheral itself (`USART_CR3`'s `DMAT`/`DMAR` bits).

## Hardware

- STM32 Nucleo-F401RE or -F411RE
- USART2 (PA2 = TX, PA3 = RX) — routed through the onboard ST-Link Virtual COM Port, no external UART adapter needed
- Onboard LED on PA5

## Key registers touched

| Peripheral | Registers |
|---|---|
| RCC | AHB1ENR, APB1ENR |
| GPIOA | MODER, AFRL, ODR |
| USART2 | SR, DR, BRR, CR1, CR3 |
| DMA1 | Stream5 & Stream6: CR, NDTR, PAR, M0AR; HISR, HIFCR |
| NVIC | ISER0 (DMA1_Stream5 interrupt) |

## Setup

- Baud rate: 9600 (BRR = `0x0683`, derived for 16 MHz HSI on PCLK1)
- Open a serial terminal at 9600 baud on the Nucleo's ST-Link COM port
- On flash: `"Hello From STM32"` prints immediately via DMA
- Type 5 characters into the terminal: the onboard LED toggles once per completed 5-byte reception
- DONE WITH GUIDANCE
