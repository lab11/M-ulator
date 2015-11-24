
Pstack_ondemand_v1.8/Pstack_ondemand_v1.8.elf:     file format elf32-littlearm


Disassembly of section .text:

00000000 <hang-0x70>:
   0:	00000c00 	.word	0x00000c00
   4:	00000075 	.word	0x00000075
	...
  1c:	00000070 	.word	0x00000070
  20:	00000070 	.word	0x00000070
  24:	00000070 	.word	0x00000070
  28:	00000070 	.word	0x00000070
	...
  34:	00000070 	.word	0x00000070
	...
  40:	000007fd 	.word	0x000007fd
  44:	00000815 	.word	0x00000815
  48:	0000082d 	.word	0x0000082d
  4c:	00000845 	.word	0x00000845
	...

00000070 <hang>:
  70:	e7fe      	b.n	70 <hang>
	...

00000074 <_start>:
  74:	f000 fbf2 	bl	85c <main>
  78:	e7fc      	b.n	74 <_start>

Disassembly of section .text.delay:

0000009a <delay>:
  9a:	b500      	push	{lr}
  9c:	2300      	movs	r3, #0
  9e:	e001      	b.n	a4 <delay+0xa>
  a0:	46c0      	nop			; (mov r8, r8)
  a2:	3301      	adds	r3, #1
  a4:	4283      	cmp	r3, r0
  a6:	d1fb      	bne.n	a0 <delay+0x6>
  a8:	bd00      	pop	{pc}

Disassembly of section .text.config_timer:

000000b0 <config_timer>:
  b0:	b570      	push	{r4, r5, r6, lr}
  b2:	24a5      	movs	r4, #165	; 0xa5
  b4:	0100      	lsls	r0, r0, #4
  b6:	0624      	lsls	r4, r4, #24
  b8:	4304      	orrs	r4, r0
  ba:	2500      	movs	r5, #0
  bc:	6025      	str	r5, [r4, #0]
  be:	4d06      	ldr	r5, [pc, #24]	; (d8 <config_timer+0x28>)
  c0:	9e04      	ldr	r6, [sp, #16]
  c2:	4305      	orrs	r5, r0
  c4:	602e      	str	r6, [r5, #0]
  c6:	4d05      	ldr	r5, [pc, #20]	; (dc <config_timer+0x2c>)
  c8:	4305      	orrs	r5, r0
  ca:	602a      	str	r2, [r5, #0]
  cc:	4a04      	ldr	r2, [pc, #16]	; (e0 <config_timer+0x30>)
  ce:	4310      	orrs	r0, r2
  d0:	6003      	str	r3, [r0, #0]
  d2:	6021      	str	r1, [r4, #0]
  d4:	bd70      	pop	{r4, r5, r6, pc}
  d6:	46c0      	nop			; (mov r8, r8)
  d8:	a5000004 	.word	0xa5000004
  dc:	a5000008 	.word	0xa5000008
  e0:	a500000c 	.word	0xa500000c

Disassembly of section .text.set_wakeup_timer:

000000e4 <set_wakeup_timer>:
  e4:	b500      	push	{lr}
  e6:	2900      	cmp	r1, #0
  e8:	d003      	beq.n	f2 <set_wakeup_timer+0xe>
  ea:	2380      	movs	r3, #128	; 0x80
  ec:	021b      	lsls	r3, r3, #8
  ee:	4318      	orrs	r0, r3
  f0:	e001      	b.n	f6 <set_wakeup_timer+0x12>
  f2:	0440      	lsls	r0, r0, #17
  f4:	0c40      	lsrs	r0, r0, #17
  f6:	4b04      	ldr	r3, [pc, #16]	; (108 <set_wakeup_timer+0x24>)
  f8:	6018      	str	r0, [r3, #0]
  fa:	2a00      	cmp	r2, #0
  fc:	d002      	beq.n	104 <set_wakeup_timer+0x20>
  fe:	4b03      	ldr	r3, [pc, #12]	; (10c <set_wakeup_timer+0x28>)
 100:	2201      	movs	r2, #1
 102:	601a      	str	r2, [r3, #0]
 104:	bd00      	pop	{pc}
 106:	46c0      	nop			; (mov r8, r8)
 108:	a2000010 	.word	0xa2000010
 10c:	a2000014 	.word	0xa2000014

Disassembly of section .text.enumerate:

00000384 <enumerate>:
 384:	2280      	movs	r2, #128	; 0x80
 386:	0600      	lsls	r0, r0, #24
 388:	0592      	lsls	r2, r2, #22
 38a:	23a0      	movs	r3, #160	; 0xa0
 38c:	4302      	orrs	r2, r0
 38e:	061b      	lsls	r3, r3, #24
 390:	2000      	movs	r0, #0
 392:	6018      	str	r0, [r3, #0]
 394:	4b01      	ldr	r3, [pc, #4]	; (39c <enumerate+0x18>)
 396:	601a      	str	r2, [r3, #0]
 398:	4770      	bx	lr
 39a:	46c0      	nop			; (mov r8, r8)
 39c:	a0000004 	.word	0xa0000004

Disassembly of section .text.sleep:

000003a0 <sleep>:
 3a0:	23a0      	movs	r3, #160	; 0xa0
 3a2:	2201      	movs	r2, #1
 3a4:	061b      	lsls	r3, r3, #24
 3a6:	601a      	str	r2, [r3, #0]
 3a8:	4b01      	ldr	r3, [pc, #4]	; (3b0 <sleep+0x10>)
 3aa:	2000      	movs	r0, #0
 3ac:	6018      	str	r0, [r3, #0]
 3ae:	4770      	bx	lr
 3b0:	a0000004 	.word	0xa0000004

Disassembly of section .text.write_mbus_register:

000003b4 <write_mbus_register>:
 3b4:	23a0      	movs	r3, #160	; 0xa0
 3b6:	0100      	lsls	r0, r0, #4
 3b8:	061b      	lsls	r3, r3, #24
 3ba:	0212      	lsls	r2, r2, #8
 3bc:	6018      	str	r0, [r3, #0]
 3be:	0a12      	lsrs	r2, r2, #8
 3c0:	4b02      	ldr	r3, [pc, #8]	; (3cc <write_mbus_register+0x18>)
 3c2:	0609      	lsls	r1, r1, #24
 3c4:	4311      	orrs	r1, r2
 3c6:	2000      	movs	r0, #0
 3c8:	6019      	str	r1, [r3, #0]
 3ca:	4770      	bx	lr
 3cc:	a0000004 	.word	0xa0000004

Disassembly of section .text.read_mbus_register:

000003d0 <read_mbus_register>:
 3d0:	2301      	movs	r3, #1
 3d2:	0100      	lsls	r0, r0, #4
 3d4:	4318      	orrs	r0, r3
 3d6:	23a0      	movs	r3, #160	; 0xa0
 3d8:	061b      	lsls	r3, r3, #24
 3da:	6018      	str	r0, [r3, #0]
 3dc:	0212      	lsls	r2, r2, #8
 3de:	4b03      	ldr	r3, [pc, #12]	; (3ec <read_mbus_register+0x1c>)
 3e0:	0609      	lsls	r1, r1, #24
 3e2:	4311      	orrs	r1, r2
 3e4:	2000      	movs	r0, #0
 3e6:	6019      	str	r1, [r3, #0]
 3e8:	4770      	bx	lr
 3ea:	46c0      	nop			; (mov r8, r8)
 3ec:	a0000004 	.word	0xa0000004

Disassembly of section .text.unlikely.send_radio_data:

00000410 <send_radio_data>:
 410:	b538      	push	{r3, r4, r5, lr}
 412:	1c05      	adds	r5, r0, #0
 414:	2417      	movs	r4, #23
 416:	1c2a      	adds	r2, r5, #0
 418:	2301      	movs	r3, #1
 41a:	40e2      	lsrs	r2, r4
 41c:	401a      	ands	r2, r3
 41e:	2004      	movs	r0, #4
 420:	2127      	movs	r1, #39	; 0x27
 422:	2a00      	cmp	r2, #0
 424:	d000      	beq.n	428 <send_radio_data+0x18>
 426:	1c1a      	adds	r2, r3, #0
 428:	f7ff ffc4 	bl	3b4 <write_mbus_register>
 42c:	2022      	movs	r0, #34	; 0x22
 42e:	f7ff fe34 	bl	9a <delay>
 432:	2004      	movs	r0, #4
 434:	2127      	movs	r1, #39	; 0x27
 436:	2200      	movs	r2, #0
 438:	f7ff ffbc 	bl	3b4 <write_mbus_register>
 43c:	2022      	movs	r0, #34	; 0x22
 43e:	f7ff fe2c 	bl	9a <delay>
 442:	3c01      	subs	r4, #1
 444:	d2e7      	bcs.n	416 <send_radio_data+0x6>
 446:	bd38      	pop	{r3, r4, r5, pc}

Disassembly of section .text.ldo_power_off:

00000448 <ldo_power_off>:
 448:	b508      	push	{r3, lr}
 44a:	4b0e      	ldr	r3, [pc, #56]	; (484 <ldo_power_off+0x3c>)
 44c:	2102      	movs	r1, #2
 44e:	681a      	ldr	r2, [r3, #0]
 450:	2005      	movs	r0, #5
 452:	430a      	orrs	r2, r1
 454:	601a      	str	r2, [r3, #0]
 456:	6819      	ldr	r1, [r3, #0]
 458:	2280      	movs	r2, #128	; 0x80
 45a:	0152      	lsls	r2, r2, #5
 45c:	430a      	orrs	r2, r1
 45e:	601a      	str	r2, [r3, #0]
 460:	681a      	ldr	r2, [r3, #0]
 462:	2101      	movs	r1, #1
 464:	430a      	orrs	r2, r1
 466:	601a      	str	r2, [r3, #0]
 468:	6819      	ldr	r1, [r3, #0]
 46a:	2280      	movs	r2, #128	; 0x80
 46c:	0112      	lsls	r2, r2, #4
 46e:	430a      	orrs	r2, r1
 470:	601a      	str	r2, [r3, #0]
 472:	681a      	ldr	r2, [r3, #0]
 474:	2112      	movs	r1, #18
 476:	f7ff ff9d 	bl	3b4 <write_mbus_register>
 47a:	2064      	movs	r0, #100	; 0x64
 47c:	f7ff fe0d 	bl	9a <delay>
 480:	bd08      	pop	{r3, pc}
 482:	46c0      	nop			; (mov r8, r8)
 484:	00000c58 	.word	0x00000c58

Disassembly of section .text.release_cdc_meas:

00000488 <release_cdc_meas>:
 488:	b508      	push	{r3, lr}
 48a:	4b07      	ldr	r3, [pc, #28]	; (4a8 <release_cdc_meas+0x20>)
 48c:	2180      	movs	r1, #128	; 0x80
 48e:	881a      	ldrh	r2, [r3, #0]
 490:	2005      	movs	r0, #5
 492:	438a      	bics	r2, r1
 494:	801a      	strh	r2, [r3, #0]
 496:	681a      	ldr	r2, [r3, #0]
 498:	2100      	movs	r1, #0
 49a:	f7ff ff8b 	bl	3b4 <write_mbus_register>
 49e:	2064      	movs	r0, #100	; 0x64
 4a0:	f7ff fdfb 	bl	9a <delay>
 4a4:	bd08      	pop	{r3, pc}
 4a6:	46c0      	nop			; (mov r8, r8)
 4a8:	00000c5c 	.word	0x00000c5c

Disassembly of section .text.assert_cdc_reset:

000004ac <assert_cdc_reset>:
 4ac:	b508      	push	{r3, lr}
 4ae:	4b07      	ldr	r3, [pc, #28]	; (4cc <assert_cdc_reset+0x20>)
 4b0:	4a07      	ldr	r2, [pc, #28]	; (4d0 <assert_cdc_reset+0x24>)
 4b2:	8819      	ldrh	r1, [r3, #0]
 4b4:	2005      	movs	r0, #5
 4b6:	400a      	ands	r2, r1
 4b8:	801a      	strh	r2, [r3, #0]
 4ba:	681a      	ldr	r2, [r3, #0]
 4bc:	2100      	movs	r1, #0
 4be:	f7ff ff79 	bl	3b4 <write_mbus_register>
 4c2:	2064      	movs	r0, #100	; 0x64
 4c4:	f7ff fde9 	bl	9a <delay>
 4c8:	bd08      	pop	{r3, pc}
 4ca:	46c0      	nop			; (mov r8, r8)
 4cc:	00000c5c 	.word	0x00000c5c
 4d0:	fffffeff 	.word	0xfffffeff

Disassembly of section .text.cdc_power_off:

000004d4 <cdc_power_off>:
 4d4:	b508      	push	{r3, lr}
 4d6:	4b10      	ldr	r3, [pc, #64]	; (518 <cdc_power_off+0x44>)
 4d8:	2280      	movs	r2, #128	; 0x80
 4da:	8819      	ldrh	r1, [r3, #0]
 4dc:	0092      	lsls	r2, r2, #2
 4de:	430a      	orrs	r2, r1
 4e0:	801a      	strh	r2, [r3, #0]
 4e2:	8819      	ldrh	r1, [r3, #0]
 4e4:	2280      	movs	r2, #128	; 0x80
 4e6:	0112      	lsls	r2, r2, #4
 4e8:	430a      	orrs	r2, r1
 4ea:	801a      	strh	r2, [r3, #0]
 4ec:	8819      	ldrh	r1, [r3, #0]
 4ee:	2280      	movs	r2, #128	; 0x80
 4f0:	00d2      	lsls	r2, r2, #3
 4f2:	430a      	orrs	r2, r1
 4f4:	801a      	strh	r2, [r3, #0]
 4f6:	8819      	ldrh	r1, [r3, #0]
 4f8:	2280      	movs	r2, #128	; 0x80
 4fa:	0152      	lsls	r2, r2, #5
 4fc:	430a      	orrs	r2, r1
 4fe:	801a      	strh	r2, [r3, #0]
 500:	681a      	ldr	r2, [r3, #0]
 502:	2100      	movs	r1, #0
 504:	2005      	movs	r0, #5
 506:	f7ff ff55 	bl	3b4 <write_mbus_register>
 50a:	2064      	movs	r0, #100	; 0x64
 50c:	f7ff fdc5 	bl	9a <delay>
 510:	f7ff ff9a 	bl	448 <ldo_power_off>
 514:	bd08      	pop	{r3, pc}
 516:	46c0      	nop			; (mov r8, r8)
 518:	00000c5c 	.word	0x00000c5c

Disassembly of section .text.unlikely.operation_sleep_noirqreset:

0000051c <operation_sleep_noirqreset>:
 51c:	b508      	push	{r3, lr}
 51e:	4b03      	ldr	r3, [pc, #12]	; (52c <operation_sleep_noirqreset+0x10>)
 520:	2201      	movs	r2, #1
 522:	601a      	str	r2, [r3, #0]
 524:	f7ff ff3c 	bl	3a0 <sleep>
 528:	e7fe      	b.n	528 <operation_sleep_noirqreset+0xc>
 52a:	46c0      	nop			; (mov r8, r8)
 52c:	a2000014 	.word	0xa2000014

Disassembly of section .text.operation_sleep:

00000530 <operation_sleep>:
 530:	b508      	push	{r3, lr}
 532:	4b04      	ldr	r3, [pc, #16]	; (544 <operation_sleep+0x14>)
 534:	2201      	movs	r2, #1
 536:	601a      	str	r2, [r3, #0]
 538:	2200      	movs	r2, #0
 53a:	2368      	movs	r3, #104	; 0x68
 53c:	601a      	str	r2, [r3, #0]
 53e:	f7ff ff2f 	bl	3a0 <sleep>
 542:	e7fe      	b.n	542 <operation_sleep+0x12>
 544:	a2000014 	.word	0xa2000014

Disassembly of section .text.operation_sleep_notimer:

00000548 <operation_sleep_notimer>:
 548:	b508      	push	{r3, lr}
 54a:	f7ff ff7d 	bl	448 <ldo_power_off>
 54e:	2000      	movs	r0, #0
 550:	1c01      	adds	r1, r0, #0
 552:	1c02      	adds	r2, r0, #0
 554:	f7ff fdc6 	bl	e4 <set_wakeup_timer>
 558:	f7ff ffea 	bl	530 <operation_sleep>

Disassembly of section .text.operation_cdc_run:

0000055c <operation_cdc_run>:
 55c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 55e:	4c91      	ldr	r4, [pc, #580]	; (7a4 <operation_cdc_run+0x248>)
 560:	7825      	ldrb	r5, [r4, #0]
 562:	2d00      	cmp	r5, #0
 564:	d110      	bne.n	588 <operation_cdc_run+0x2c>
 566:	2304      	movs	r3, #4
 568:	7023      	strb	r3, [r4, #0]
 56a:	4b8f      	ldr	r3, [pc, #572]	; (7a8 <operation_cdc_run+0x24c>)
 56c:	2401      	movs	r4, #1
 56e:	601d      	str	r5, [r3, #0]
 570:	4b8e      	ldr	r3, [pc, #568]	; (7ac <operation_cdc_run+0x250>)
 572:	2112      	movs	r1, #18
 574:	681a      	ldr	r2, [r3, #0]
 576:	2005      	movs	r0, #5
 578:	43a2      	bics	r2, r4
 57a:	601a      	str	r2, [r3, #0]
 57c:	681a      	ldr	r2, [r3, #0]
 57e:	f7ff ff19 	bl	3b4 <write_mbus_register>
 582:	1c20      	adds	r0, r4, #0
 584:	1c21      	adds	r1, r4, #0
 586:	e092      	b.n	6ae <operation_cdc_run+0x152>
 588:	7823      	ldrb	r3, [r4, #0]
 58a:	2b04      	cmp	r3, #4
 58c:	d10d      	bne.n	5aa <operation_cdc_run+0x4e>
 58e:	4b87      	ldr	r3, [pc, #540]	; (7ac <operation_cdc_run+0x250>)
 590:	2005      	movs	r0, #5
 592:	7020      	strb	r0, [r4, #0]
 594:	681a      	ldr	r2, [r3, #0]
 596:	2102      	movs	r1, #2
 598:	438a      	bics	r2, r1
 59a:	601a      	str	r2, [r3, #0]
 59c:	2112      	movs	r1, #18
 59e:	681a      	ldr	r2, [r3, #0]
 5a0:	f7ff ff08 	bl	3b4 <write_mbus_register>
 5a4:	2001      	movs	r0, #1
 5a6:	1c01      	adds	r1, r0, #0
 5a8:	e0f0      	b.n	78c <operation_cdc_run+0x230>
 5aa:	7825      	ldrb	r5, [r4, #0]
 5ac:	2d05      	cmp	r5, #5
 5ae:	d134      	bne.n	61a <operation_cdc_run+0xbe>
 5b0:	2301      	movs	r3, #1
 5b2:	7023      	strb	r3, [r4, #0]
 5b4:	4c7e      	ldr	r4, [pc, #504]	; (7b0 <operation_cdc_run+0x254>)
 5b6:	4b7f      	ldr	r3, [pc, #508]	; (7b4 <operation_cdc_run+0x258>)
 5b8:	8822      	ldrh	r2, [r4, #0]
 5ba:	2100      	movs	r1, #0
 5bc:	4013      	ands	r3, r2
 5be:	8023      	strh	r3, [r4, #0]
 5c0:	6822      	ldr	r2, [r4, #0]
 5c2:	1c28      	adds	r0, r5, #0
 5c4:	f7ff fef6 	bl	3b4 <write_mbus_register>
 5c8:	2064      	movs	r0, #100	; 0x64
 5ca:	f7ff fd66 	bl	9a <delay>
 5ce:	8822      	ldrh	r2, [r4, #0]
 5d0:	4b79      	ldr	r3, [pc, #484]	; (7b8 <operation_cdc_run+0x25c>)
 5d2:	2100      	movs	r1, #0
 5d4:	4013      	ands	r3, r2
 5d6:	8023      	strh	r3, [r4, #0]
 5d8:	6822      	ldr	r2, [r4, #0]
 5da:	1c28      	adds	r0, r5, #0
 5dc:	f7ff feea 	bl	3b4 <write_mbus_register>
 5e0:	2064      	movs	r0, #100	; 0x64
 5e2:	f7ff fd5a 	bl	9a <delay>
 5e6:	8822      	ldrh	r2, [r4, #0]
 5e8:	4b74      	ldr	r3, [pc, #464]	; (7bc <operation_cdc_run+0x260>)
 5ea:	2100      	movs	r1, #0
 5ec:	4013      	ands	r3, r2
 5ee:	8023      	strh	r3, [r4, #0]
 5f0:	6822      	ldr	r2, [r4, #0]
 5f2:	1c28      	adds	r0, r5, #0
 5f4:	f7ff fede 	bl	3b4 <write_mbus_register>
 5f8:	2064      	movs	r0, #100	; 0x64
 5fa:	f7ff fd4e 	bl	9a <delay>
 5fe:	20c8      	movs	r0, #200	; 0xc8
 600:	f7ff fd4b 	bl	9a <delay>
 604:	8822      	ldrh	r2, [r4, #0]
 606:	4b6e      	ldr	r3, [pc, #440]	; (7c0 <operation_cdc_run+0x264>)
 608:	1c28      	adds	r0, r5, #0
 60a:	4013      	ands	r3, r2
 60c:	8023      	strh	r3, [r4, #0]
 60e:	6822      	ldr	r2, [r4, #0]
 610:	2100      	movs	r1, #0
 612:	f7ff fecf 	bl	3b4 <write_mbus_register>
 616:	2064      	movs	r0, #100	; 0x64
 618:	e052      	b.n	6c0 <operation_cdc_run+0x164>
 61a:	7823      	ldrb	r3, [r4, #0]
 61c:	2b01      	cmp	r3, #1
 61e:	d152      	bne.n	6c6 <operation_cdc_run+0x16a>
 620:	4e68      	ldr	r6, [pc, #416]	; (7c4 <operation_cdc_run+0x268>)
 622:	4c63      	ldr	r4, [pc, #396]	; (7b0 <operation_cdc_run+0x254>)
 624:	2500      	movs	r5, #0
 626:	7035      	strb	r5, [r6, #0]
 628:	8822      	ldrh	r2, [r4, #0]
 62a:	2380      	movs	r3, #128	; 0x80
 62c:	005b      	lsls	r3, r3, #1
 62e:	4313      	orrs	r3, r2
 630:	8023      	strh	r3, [r4, #0]
 632:	6822      	ldr	r2, [r4, #0]
 634:	1c29      	adds	r1, r5, #0
 636:	2005      	movs	r0, #5
 638:	f7ff febc 	bl	3b4 <write_mbus_register>
 63c:	2064      	movs	r0, #100	; 0x64
 63e:	f7ff fd2c 	bl	9a <delay>
 642:	20c8      	movs	r0, #200	; 0xc8
 644:	f7ff fd29 	bl	9a <delay>
 648:	8823      	ldrh	r3, [r4, #0]
 64a:	2280      	movs	r2, #128	; 0x80
 64c:	4313      	orrs	r3, r2
 64e:	8023      	strh	r3, [r4, #0]
 650:	6822      	ldr	r2, [r4, #0]
 652:	1c29      	adds	r1, r5, #0
 654:	2005      	movs	r0, #5
 656:	f7ff fead 	bl	3b4 <write_mbus_register>
 65a:	2064      	movs	r0, #100	; 0x64
 65c:	f7ff fd1d 	bl	9a <delay>
 660:	25fa      	movs	r5, #250	; 0xfa
 662:	006d      	lsls	r5, r5, #1
 664:	1c34      	adds	r4, r6, #0
 666:	7823      	ldrb	r3, [r4, #0]
 668:	2b00      	cmp	r3, #0
 66a:	d007      	beq.n	67c <operation_cdc_run+0x120>
 66c:	4a4e      	ldr	r2, [pc, #312]	; (7a8 <operation_cdc_run+0x24c>)
 66e:	2300      	movs	r3, #0
 670:	7023      	strb	r3, [r4, #0]
 672:	6013      	str	r3, [r2, #0]
 674:	4b4b      	ldr	r3, [pc, #300]	; (7a4 <operation_cdc_run+0x248>)
 676:	2203      	movs	r2, #3
 678:	701a      	strb	r2, [r3, #0]
 67a:	e092      	b.n	7a2 <operation_cdc_run+0x246>
 67c:	2064      	movs	r0, #100	; 0x64
 67e:	3d01      	subs	r5, #1
 680:	f7ff fd0b 	bl	9a <delay>
 684:	2d00      	cmp	r5, #0
 686:	d1ee      	bne.n	666 <operation_cdc_run+0x10a>
 688:	f7ff fefe 	bl	488 <release_cdc_meas>
 68c:	4c46      	ldr	r4, [pc, #280]	; (7a8 <operation_cdc_run+0x24c>)
 68e:	6823      	ldr	r3, [r4, #0]
 690:	2b00      	cmp	r3, #0
 692:	d00e      	beq.n	6b2 <operation_cdc_run+0x156>
 694:	6823      	ldr	r3, [r4, #0]
 696:	3301      	adds	r3, #1
 698:	6023      	str	r3, [r4, #0]
 69a:	f7ff ff07 	bl	4ac <assert_cdc_reset>
 69e:	6823      	ldr	r3, [r4, #0]
 6a0:	2b05      	cmp	r3, #5
 6a2:	d902      	bls.n	6aa <operation_cdc_run+0x14e>
 6a4:	4b3f      	ldr	r3, [pc, #252]	; (7a4 <operation_cdc_run+0x248>)
 6a6:	701d      	strb	r5, [r3, #0]
 6a8:	e077      	b.n	79a <operation_cdc_run+0x23e>
 6aa:	2002      	movs	r0, #2
 6ac:	2101      	movs	r1, #1
 6ae:	1c2a      	adds	r2, r5, #0
 6b0:	e06d      	b.n	78e <operation_cdc_run+0x232>
 6b2:	6823      	ldr	r3, [r4, #0]
 6b4:	3301      	adds	r3, #1
 6b6:	6023      	str	r3, [r4, #0]
 6b8:	f7ff fef8 	bl	4ac <assert_cdc_reset>
 6bc:	20fa      	movs	r0, #250	; 0xfa
 6be:	0040      	lsls	r0, r0, #1
 6c0:	f7ff fceb 	bl	9a <delay>
 6c4:	e06d      	b.n	7a2 <operation_cdc_run+0x246>
 6c6:	7823      	ldrb	r3, [r4, #0]
 6c8:	2b03      	cmp	r3, #3
 6ca:	d164      	bne.n	796 <operation_cdc_run+0x23a>
 6cc:	2104      	movs	r1, #4
 6ce:	2215      	movs	r2, #21
 6d0:	2005      	movs	r0, #5
 6d2:	f7ff fe7d 	bl	3d0 <read_mbus_register>
 6d6:	4d3c      	ldr	r5, [pc, #240]	; (7c8 <operation_cdc_run+0x26c>)
 6d8:	2064      	movs	r0, #100	; 0x64
 6da:	f7ff fcde 	bl	9a <delay>
 6de:	2106      	movs	r1, #6
 6e0:	2215      	movs	r2, #21
 6e2:	2005      	movs	r0, #5
 6e4:	682e      	ldr	r6, [r5, #0]
 6e6:	f7ff fe73 	bl	3d0 <read_mbus_register>
 6ea:	2064      	movs	r0, #100	; 0x64
 6ec:	f7ff fcd5 	bl	9a <delay>
 6f0:	4b36      	ldr	r3, [pc, #216]	; (7cc <operation_cdc_run+0x270>)
 6f2:	2700      	movs	r7, #0
 6f4:	682d      	ldr	r5, [r5, #0]
 6f6:	681a      	ldr	r2, [r3, #0]
 6f8:	601f      	str	r7, [r3, #0]
 6fa:	f7ff fec5 	bl	488 <release_cdc_meas>
 6fe:	f7ff fed5 	bl	4ac <assert_cdc_reset>
 702:	7027      	strb	r7, [r4, #0]
 704:	f7ff fee6 	bl	4d4 <cdc_power_off>
 708:	4b31      	ldr	r3, [pc, #196]	; (7d0 <operation_cdc_run+0x274>)
 70a:	781a      	ldrb	r2, [r3, #0]
 70c:	42ba      	cmp	r2, r7
 70e:	d003      	beq.n	718 <operation_cdc_run+0x1bc>
 710:	701f      	strb	r7, [r3, #0]
 712:	4b30      	ldr	r3, [pc, #192]	; (7d4 <operation_cdc_run+0x278>)
 714:	601d      	str	r5, [r3, #0]
 716:	e044      	b.n	7a2 <operation_cdc_run+0x246>
 718:	4b2f      	ldr	r3, [pc, #188]	; (7d8 <operation_cdc_run+0x27c>)
 71a:	681a      	ldr	r2, [r3, #0]
 71c:	3201      	adds	r2, #1
 71e:	601a      	str	r2, [r3, #0]
 720:	4b2e      	ldr	r3, [pc, #184]	; (7dc <operation_cdc_run+0x280>)
 722:	681a      	ldr	r2, [r3, #0]
 724:	2a27      	cmp	r2, #39	; 0x27
 726:	d80f      	bhi.n	748 <operation_cdc_run+0x1ec>
 728:	6819      	ldr	r1, [r3, #0]
 72a:	4a2d      	ldr	r2, [pc, #180]	; (7e0 <operation_cdc_run+0x284>)
 72c:	0089      	lsls	r1, r1, #2
 72e:	508e      	str	r6, [r1, r2]
 730:	6819      	ldr	r1, [r3, #0]
 732:	4a2c      	ldr	r2, [pc, #176]	; (7e4 <operation_cdc_run+0x288>)
 734:	0089      	lsls	r1, r1, #2
 736:	508d      	str	r5, [r1, r2]
 738:	4a26      	ldr	r2, [pc, #152]	; (7d4 <operation_cdc_run+0x278>)
 73a:	6015      	str	r5, [r2, #0]
 73c:	681a      	ldr	r2, [r3, #0]
 73e:	3201      	adds	r2, #1
 740:	601a      	str	r2, [r3, #0]
 742:	681a      	ldr	r2, [r3, #0]
 744:	4b28      	ldr	r3, [pc, #160]	; (7e8 <operation_cdc_run+0x28c>)
 746:	601a      	str	r2, [r3, #0]
 748:	4b28      	ldr	r3, [pc, #160]	; (7ec <operation_cdc_run+0x290>)
 74a:	681b      	ldr	r3, [r3, #0]
 74c:	2b00      	cmp	r3, #0
 74e:	d010      	beq.n	772 <operation_cdc_run+0x216>
 750:	24f0      	movs	r4, #240	; 0xf0
 752:	0424      	lsls	r4, r4, #16
 754:	1c20      	adds	r0, r4, #0
 756:	4330      	orrs	r0, r6
 758:	f7ff fe5a 	bl	410 <send_radio_data>
 75c:	20fa      	movs	r0, #250	; 0xfa
 75e:	0080      	lsls	r0, r0, #2
 760:	f7ff fc9b 	bl	9a <delay>
 764:	1c20      	adds	r0, r4, #0
 766:	4328      	orrs	r0, r5
 768:	f7ff fe52 	bl	410 <send_radio_data>
 76c:	2064      	movs	r0, #100	; 0x64
 76e:	f7ff fc94 	bl	9a <delay>
 772:	4b19      	ldr	r3, [pc, #100]	; (7d8 <operation_cdc_run+0x27c>)
 774:	681b      	ldr	r3, [r3, #0]
 776:	2b03      	cmp	r3, #3
 778:	d804      	bhi.n	784 <operation_cdc_run+0x228>
 77a:	481d      	ldr	r0, [pc, #116]	; (7f0 <operation_cdc_run+0x294>)
 77c:	f7ff fe48 	bl	410 <send_radio_data>
 780:	4b1c      	ldr	r3, [pc, #112]	; (7f4 <operation_cdc_run+0x298>)
 782:	e000      	b.n	786 <operation_cdc_run+0x22a>
 784:	4b1c      	ldr	r3, [pc, #112]	; (7f8 <operation_cdc_run+0x29c>)
 786:	6818      	ldr	r0, [r3, #0]
 788:	2101      	movs	r1, #1
 78a:	b280      	uxth	r0, r0
 78c:	2200      	movs	r2, #0
 78e:	f7ff fca9 	bl	e4 <set_wakeup_timer>
 792:	f7ff fecd 	bl	530 <operation_sleep>
 796:	f7ff fe89 	bl	4ac <assert_cdc_reset>
 79a:	f7ff fe9b 	bl	4d4 <cdc_power_off>
 79e:	f7ff fed3 	bl	548 <operation_sleep_notimer>
 7a2:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 7a4:	00000c80 	.word	0x00000c80
 7a8:	00000c74 	.word	0x00000c74
 7ac:	00000c58 	.word	0x00000c58
 7b0:	00000c5c 	.word	0x00000c5c
 7b4:	fffff7ff 	.word	0xfffff7ff
 7b8:	fffffbff 	.word	0xfffffbff
 7bc:	ffffefff 	.word	0xffffefff
 7c0:	fffffdff 	.word	0xfffffdff
 7c4:	00000ca4 	.word	0x00000ca4
 7c8:	a0001014 	.word	0xa0001014
 7cc:	00000ca0 	.word	0x00000ca0
 7d0:	00000c6c 	.word	0x00000c6c
 7d4:	00000ca8 	.word	0x00000ca8
 7d8:	00000c70 	.word	0x00000c70
 7dc:	00000c94 	.word	0x00000c94
 7e0:	00000d58 	.word	0x00000d58
 7e4:	00000cb8 	.word	0x00000cb8
 7e8:	00000c68 	.word	0x00000c68
 7ec:	00000c90 	.word	0x00000c90
 7f0:	00faf000 	.word	0x00faf000
 7f4:	00000c98 	.word	0x00000c98
 7f8:	00000cb0 	.word	0x00000cb0

Disassembly of section .text.handler_ext_int_0:

000007fc <handler_ext_int_0>:
 7fc:	4b03      	ldr	r3, [pc, #12]	; (80c <handler_ext_int_0+0x10>)
 7fe:	2201      	movs	r2, #1
 800:	601a      	str	r2, [r3, #0]
 802:	4b03      	ldr	r3, [pc, #12]	; (810 <handler_ext_int_0+0x14>)
 804:	2210      	movs	r2, #16
 806:	701a      	strb	r2, [r3, #0]
 808:	4770      	bx	lr
 80a:	46c0      	nop			; (mov r8, r8)
 80c:	e000e280 	.word	0xe000e280
 810:	00000ca4 	.word	0x00000ca4

Disassembly of section .text.handler_ext_int_1:

00000814 <handler_ext_int_1>:
 814:	4b03      	ldr	r3, [pc, #12]	; (824 <handler_ext_int_1+0x10>)
 816:	2202      	movs	r2, #2
 818:	601a      	str	r2, [r3, #0]
 81a:	4b03      	ldr	r3, [pc, #12]	; (828 <handler_ext_int_1+0x14>)
 81c:	2211      	movs	r2, #17
 81e:	701a      	strb	r2, [r3, #0]
 820:	4770      	bx	lr
 822:	46c0      	nop			; (mov r8, r8)
 824:	e000e280 	.word	0xe000e280
 828:	00000ca4 	.word	0x00000ca4

Disassembly of section .text.handler_ext_int_2:

0000082c <handler_ext_int_2>:
 82c:	4b03      	ldr	r3, [pc, #12]	; (83c <handler_ext_int_2+0x10>)
 82e:	2204      	movs	r2, #4
 830:	601a      	str	r2, [r3, #0]
 832:	4b03      	ldr	r3, [pc, #12]	; (840 <handler_ext_int_2+0x14>)
 834:	2212      	movs	r2, #18
 836:	701a      	strb	r2, [r3, #0]
 838:	4770      	bx	lr
 83a:	46c0      	nop			; (mov r8, r8)
 83c:	e000e280 	.word	0xe000e280
 840:	00000ca4 	.word	0x00000ca4

Disassembly of section .text.handler_ext_int_3:

00000844 <handler_ext_int_3>:
 844:	4b03      	ldr	r3, [pc, #12]	; (854 <handler_ext_int_3+0x10>)
 846:	2208      	movs	r2, #8
 848:	601a      	str	r2, [r3, #0]
 84a:	4b03      	ldr	r3, [pc, #12]	; (858 <handler_ext_int_3+0x14>)
 84c:	2213      	movs	r2, #19
 84e:	701a      	strb	r2, [r3, #0]
 850:	4770      	bx	lr
 852:	46c0      	nop			; (mov r8, r8)
 854:	e000e280 	.word	0xe000e280
 858:	00000ca4 	.word	0x00000ca4

Disassembly of section .text.startup.main:

0000085c <main>:
 85c:	4bd9      	ldr	r3, [pc, #868]	; (bc4 <main+0x368>)
 85e:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
 860:	270f      	movs	r7, #15
 862:	601f      	str	r7, [r3, #0]
 864:	4bd8      	ldr	r3, [pc, #864]	; (bc8 <main+0x36c>)
 866:	2000      	movs	r0, #0
 868:	601f      	str	r7, [r3, #0]
 86a:	4bd8      	ldr	r3, [pc, #864]	; (bcc <main+0x370>)
 86c:	2101      	movs	r1, #1
 86e:	9300      	str	r3, [sp, #0]
 870:	1c02      	adds	r2, r0, #0
 872:	1c03      	adds	r3, r0, #0
 874:	f7ff fc1c 	bl	b0 <config_timer>
 878:	4dd5      	ldr	r5, [pc, #852]	; (bd0 <main+0x374>)
 87a:	4ed6      	ldr	r6, [pc, #856]	; (bd4 <main+0x378>)
 87c:	682b      	ldr	r3, [r5, #0]
 87e:	42b3      	cmp	r3, r6
 880:	d100      	bne.n	884 <main+0x28>
 882:	e0b8      	b.n	9f6 <main+0x19a>
 884:	4ad4      	ldr	r2, [pc, #848]	; (bd8 <main+0x37c>)
 886:	4bd5      	ldr	r3, [pc, #852]	; (bdc <main+0x380>)
 888:	2064      	movs	r0, #100	; 0x64
 88a:	601a      	str	r2, [r3, #0]
 88c:	4ad4      	ldr	r2, [pc, #848]	; (be0 <main+0x384>)
 88e:	4bd5      	ldr	r3, [pc, #852]	; (be4 <main+0x388>)
 890:	2400      	movs	r4, #0
 892:	601a      	str	r2, [r3, #0]
 894:	f7ff fc01 	bl	9a <delay>
 898:	4bd3      	ldr	r3, [pc, #844]	; (be8 <main+0x38c>)
 89a:	2004      	movs	r0, #4
 89c:	701c      	strb	r4, [r3, #0]
 89e:	4bd3      	ldr	r3, [pc, #844]	; (bec <main+0x390>)
 8a0:	602e      	str	r6, [r5, #0]
 8a2:	601c      	str	r4, [r3, #0]
 8a4:	4bd2      	ldr	r3, [pc, #840]	; (bf0 <main+0x394>)
 8a6:	2501      	movs	r5, #1
 8a8:	601c      	str	r4, [r3, #0]
 8aa:	4bd2      	ldr	r3, [pc, #840]	; (bf4 <main+0x398>)
 8ac:	2602      	movs	r6, #2
 8ae:	701c      	strb	r4, [r3, #0]
 8b0:	f7ff fd68 	bl	384 <enumerate>
 8b4:	20fa      	movs	r0, #250	; 0xfa
 8b6:	0080      	lsls	r0, r0, #2
 8b8:	f7ff fbef 	bl	9a <delay>
 8bc:	2005      	movs	r0, #5
 8be:	f7ff fd61 	bl	384 <enumerate>
 8c2:	20fa      	movs	r0, #250	; 0xfa
 8c4:	0080      	lsls	r0, r0, #2
 8c6:	f7ff fbe8 	bl	9a <delay>
 8ca:	2006      	movs	r0, #6
 8cc:	f7ff fd5a 	bl	384 <enumerate>
 8d0:	20fa      	movs	r0, #250	; 0xfa
 8d2:	0080      	lsls	r0, r0, #2
 8d4:	f7ff fbe1 	bl	9a <delay>
 8d8:	4bc7      	ldr	r3, [pc, #796]	; (bf8 <main+0x39c>)
 8da:	2005      	movs	r0, #5
 8dc:	881a      	ldrh	r2, [r3, #0]
 8de:	432a      	orrs	r2, r5
 8e0:	801a      	strh	r2, [r3, #0]
 8e2:	881a      	ldrh	r2, [r3, #0]
 8e4:	43b2      	bics	r2, r6
 8e6:	801a      	strh	r2, [r3, #0]
 8e8:	8819      	ldrh	r1, [r3, #0]
 8ea:	4ac4      	ldr	r2, [pc, #784]	; (bfc <main+0x3a0>)
 8ec:	400a      	ands	r2, r1
 8ee:	801a      	strh	r2, [r3, #0]
 8f0:	681a      	ldr	r2, [r3, #0]
 8f2:	1c21      	adds	r1, r4, #0
 8f4:	f7ff fd5e 	bl	3b4 <write_mbus_register>
 8f8:	4bc1      	ldr	r3, [pc, #772]	; (c00 <main+0x3a4>)
 8fa:	1c29      	adds	r1, r5, #0
 8fc:	681a      	ldr	r2, [r3, #0]
 8fe:	2005      	movs	r0, #5
 900:	0a92      	lsrs	r2, r2, #10
 902:	0292      	lsls	r2, r2, #10
 904:	432a      	orrs	r2, r5
 906:	601a      	str	r2, [r3, #0]
 908:	681a      	ldr	r2, [r3, #0]
 90a:	f7ff fd53 	bl	3b4 <write_mbus_register>
 90e:	4bbd      	ldr	r3, [pc, #756]	; (c04 <main+0x3a8>)
 910:	4abd      	ldr	r2, [pc, #756]	; (c08 <main+0x3ac>)
 912:	6819      	ldr	r1, [r3, #0]
 914:	2005      	movs	r0, #5
 916:	400a      	ands	r2, r1
 918:	21c8      	movs	r1, #200	; 0xc8
 91a:	0249      	lsls	r1, r1, #9
 91c:	430a      	orrs	r2, r1
 91e:	601a      	str	r2, [r3, #0]
 920:	681a      	ldr	r2, [r3, #0]
 922:	1c31      	adds	r1, r6, #0
 924:	f7ff fd46 	bl	3b4 <write_mbus_register>
 928:	4bb8      	ldr	r3, [pc, #736]	; (c0c <main+0x3b0>)
 92a:	4ab9      	ldr	r2, [pc, #740]	; (c10 <main+0x3b4>)
 92c:	6819      	ldr	r1, [r3, #0]
 92e:	2005      	movs	r0, #5
 930:	400a      	ands	r2, r1
 932:	601a      	str	r2, [r3, #0]
 934:	6819      	ldr	r1, [r3, #0]
 936:	22c0      	movs	r2, #192	; 0xc0
 938:	01d2      	lsls	r2, r2, #7
 93a:	430a      	orrs	r2, r1
 93c:	601a      	str	r2, [r3, #0]
 93e:	6819      	ldr	r1, [r3, #0]
 940:	4ab4      	ldr	r2, [pc, #720]	; (c14 <main+0x3b8>)
 942:	400a      	ands	r2, r1
 944:	2180      	movs	r1, #128	; 0x80
 946:	0349      	lsls	r1, r1, #13
 948:	430a      	orrs	r2, r1
 94a:	601a      	str	r2, [r3, #0]
 94c:	681a      	ldr	r2, [r3, #0]
 94e:	210c      	movs	r1, #12
 950:	438a      	bics	r2, r1
 952:	601a      	str	r2, [r3, #0]
 954:	6819      	ldr	r1, [r3, #0]
 956:	4ab0      	ldr	r2, [pc, #704]	; (c18 <main+0x3bc>)
 958:	400a      	ands	r2, r1
 95a:	2180      	movs	r1, #128	; 0x80
 95c:	40b1      	lsls	r1, r6
 95e:	430a      	orrs	r2, r1
 960:	601a      	str	r2, [r3, #0]
 962:	681a      	ldr	r2, [r3, #0]
 964:	2112      	movs	r1, #18
 966:	f7ff fd25 	bl	3b4 <write_mbus_register>
 96a:	2123      	movs	r1, #35	; 0x23
 96c:	1c22      	adds	r2, r4, #0
 96e:	2004      	movs	r0, #4
 970:	f7ff fd20 	bl	3b4 <write_mbus_register>
 974:	261f      	movs	r6, #31
 976:	2064      	movs	r0, #100	; 0x64
 978:	f7ff fb8f 	bl	9a <delay>
 97c:	2126      	movs	r1, #38	; 0x26
 97e:	1c32      	adds	r2, r6, #0
 980:	2004      	movs	r0, #4
 982:	f7ff fd17 	bl	3b4 <write_mbus_register>
 986:	2064      	movs	r0, #100	; 0x64
 988:	f7ff fb87 	bl	9a <delay>
 98c:	1c32      	adds	r2, r6, #0
 98e:	2120      	movs	r1, #32
 990:	2004      	movs	r0, #4
 992:	f7ff fd0f 	bl	3b4 <write_mbus_register>
 996:	2064      	movs	r0, #100	; 0x64
 998:	f7ff fb7f 	bl	9a <delay>
 99c:	2121      	movs	r1, #33	; 0x21
 99e:	1c22      	adds	r2, r4, #0
 9a0:	2004      	movs	r0, #4
 9a2:	f7ff fd07 	bl	3b4 <write_mbus_register>
 9a6:	2064      	movs	r0, #100	; 0x64
 9a8:	f7ff fb77 	bl	9a <delay>
 9ac:	2122      	movs	r1, #34	; 0x22
 9ae:	1c22      	adds	r2, r4, #0
 9b0:	2004      	movs	r0, #4
 9b2:	f7ff fcff 	bl	3b4 <write_mbus_register>
 9b6:	2064      	movs	r0, #100	; 0x64
 9b8:	f7ff fb6f 	bl	9a <delay>
 9bc:	2125      	movs	r1, #37	; 0x25
 9be:	2205      	movs	r2, #5
 9c0:	2004      	movs	r0, #4
 9c2:	f7ff fcf7 	bl	3b4 <write_mbus_register>
 9c6:	2064      	movs	r0, #100	; 0x64
 9c8:	f7ff fb67 	bl	9a <delay>
 9cc:	1c22      	adds	r2, r4, #0
 9ce:	2004      	movs	r0, #4
 9d0:	2127      	movs	r1, #39	; 0x27
 9d2:	f7ff fcef 	bl	3b4 <write_mbus_register>
 9d6:	4b91      	ldr	r3, [pc, #580]	; (c1c <main+0x3c0>)
 9d8:	2264      	movs	r2, #100	; 0x64
 9da:	601a      	str	r2, [r3, #0]
 9dc:	4b90      	ldr	r3, [pc, #576]	; (c20 <main+0x3c4>)
 9de:	601d      	str	r5, [r3, #0]
 9e0:	4b90      	ldr	r3, [pc, #576]	; (c24 <main+0x3c8>)
 9e2:	601c      	str	r4, [r3, #0]
 9e4:	4b90      	ldr	r3, [pc, #576]	; (c28 <main+0x3cc>)
 9e6:	601c      	str	r4, [r3, #0]
 9e8:	4b90      	ldr	r3, [pc, #576]	; (c2c <main+0x3d0>)
 9ea:	601c      	str	r4, [r3, #0]
 9ec:	4b90      	ldr	r3, [pc, #576]	; (c30 <main+0x3d4>)
 9ee:	701c      	strb	r4, [r3, #0]
 9f0:	4b90      	ldr	r3, [pc, #576]	; (c34 <main+0x3d8>)
 9f2:	701c      	strb	r4, [r3, #0]
 9f4:	e022      	b.n	a3c <main+0x1e0>
 9f6:	2668      	movs	r6, #104	; 0x68
 9f8:	6834      	ldr	r4, [r6, #0]
 9fa:	0e25      	lsrs	r5, r4, #24
 9fc:	0a23      	lsrs	r3, r4, #8
 9fe:	2d01      	cmp	r5, #1
 a00:	d11e      	bne.n	a40 <main+0x1e4>
 a02:	27ff      	movs	r7, #255	; 0xff
 a04:	4e86      	ldr	r6, [pc, #536]	; (c20 <main+0x3c4>)
 a06:	403b      	ands	r3, r7
 a08:	2064      	movs	r0, #100	; 0x64
 a0a:	6033      	str	r3, [r6, #0]
 a0c:	f7ff fb45 	bl	9a <delay>
 a10:	4b77      	ldr	r3, [pc, #476]	; (bf0 <main+0x394>)
 a12:	403c      	ands	r4, r7
 a14:	681a      	ldr	r2, [r3, #0]
 a16:	42a2      	cmp	r2, r4
 a18:	d209      	bcs.n	a2e <main+0x1d2>
 a1a:	681a      	ldr	r2, [r3, #0]
 a1c:	3201      	adds	r2, #1
 a1e:	601a      	str	r2, [r3, #0]
 a20:	6818      	ldr	r0, [r3, #0]
 a22:	4b85      	ldr	r3, [pc, #532]	; (c38 <main+0x3dc>)
 a24:	18c0      	adds	r0, r0, r3
 a26:	f7ff fcf3 	bl	410 <send_radio_data>
 a2a:	6830      	ldr	r0, [r6, #0]
 a2c:	e077      	b.n	b1e <main+0x2c2>
 a2e:	2200      	movs	r2, #0
 a30:	601a      	str	r2, [r3, #0]
 a32:	6818      	ldr	r0, [r3, #0]
 a34:	4b80      	ldr	r3, [pc, #512]	; (c38 <main+0x3dc>)
 a36:	18c0      	adds	r0, r0, r3
 a38:	f7ff fcea 	bl	410 <send_radio_data>
 a3c:	f7ff fd84 	bl	548 <operation_sleep_notimer>
 a40:	0c22      	lsrs	r2, r4, #16
 a42:	2d02      	cmp	r5, #2
 a44:	d12b      	bne.n	a9e <main+0x242>
 a46:	21ff      	movs	r1, #255	; 0xff
 a48:	400b      	ands	r3, r1
 a4a:	400c      	ands	r4, r1
 a4c:	021b      	lsls	r3, r3, #8
 a4e:	18e4      	adds	r4, r4, r3
 a50:	4872      	ldr	r0, [pc, #456]	; (c1c <main+0x3c0>)
 a52:	4b73      	ldr	r3, [pc, #460]	; (c20 <main+0x3c4>)
 a54:	4017      	ands	r7, r2
 a56:	6004      	str	r4, [r0, #0]
 a58:	601f      	str	r7, [r3, #0]
 a5a:	2310      	movs	r3, #16
 a5c:	401a      	ands	r2, r3
 a5e:	4b73      	ldr	r3, [pc, #460]	; (c2c <main+0x3d0>)
 a60:	2064      	movs	r0, #100	; 0x64
 a62:	601a      	str	r2, [r3, #0]
 a64:	4a75      	ldr	r2, [pc, #468]	; (c3c <main+0x3e0>)
 a66:	4b5d      	ldr	r3, [pc, #372]	; (bdc <main+0x380>)
 a68:	601a      	str	r2, [r3, #0]
 a6a:	f7ff fb16 	bl	9a <delay>
 a6e:	4c71      	ldr	r4, [pc, #452]	; (c34 <main+0x3d8>)
 a70:	7822      	ldrb	r2, [r4, #0]
 a72:	2a00      	cmp	r2, #0
 a74:	d106      	bne.n	a84 <main+0x228>
 a76:	2005      	movs	r0, #5
 a78:	2101      	movs	r1, #1
 a7a:	f7ff fb33 	bl	e4 <set_wakeup_timer>
 a7e:	2301      	movs	r3, #1
 a80:	7023      	strb	r3, [r4, #0]
 a82:	e051      	b.n	b28 <main+0x2cc>
 a84:	4a59      	ldr	r2, [pc, #356]	; (bec <main+0x390>)
 a86:	2300      	movs	r3, #0
 a88:	6013      	str	r3, [r2, #0]
 a8a:	4a6d      	ldr	r2, [pc, #436]	; (c40 <main+0x3e4>)
 a8c:	6013      	str	r3, [r2, #0]
 a8e:	4a65      	ldr	r2, [pc, #404]	; (c24 <main+0x3c8>)
 a90:	6013      	str	r3, [r2, #0]
 a92:	4a65      	ldr	r2, [pc, #404]	; (c28 <main+0x3cc>)
 a94:	6013      	str	r3, [r2, #0]
 a96:	4a6b      	ldr	r2, [pc, #428]	; (c44 <main+0x3e8>)
 a98:	6033      	str	r3, [r6, #0]
 a9a:	6013      	str	r3, [r2, #0]
 a9c:	e08e      	b.n	bbc <main+0x360>
 a9e:	2d03      	cmp	r5, #3
 aa0:	d11b      	bne.n	ada <main+0x27e>
 aa2:	495f      	ldr	r1, [pc, #380]	; (c20 <main+0x3c4>)
 aa4:	b2db      	uxtb	r3, r3
 aa6:	600b      	str	r3, [r1, #0]
 aa8:	4b62      	ldr	r3, [pc, #392]	; (c34 <main+0x3d8>)
 aaa:	2100      	movs	r1, #0
 aac:	7019      	strb	r1, [r3, #0]
 aae:	07d3      	lsls	r3, r2, #31
 ab0:	d502      	bpl.n	ab8 <main+0x25c>
 ab2:	4a49      	ldr	r2, [pc, #292]	; (bd8 <main+0x37c>)
 ab4:	4b49      	ldr	r3, [pc, #292]	; (bdc <main+0x380>)
 ab6:	601a      	str	r2, [r3, #0]
 ab8:	4b4d      	ldr	r3, [pc, #308]	; (bf0 <main+0x394>)
 aba:	b2e4      	uxtb	r4, r4
 abc:	681a      	ldr	r2, [r3, #0]
 abe:	42a2      	cmp	r2, r4
 ac0:	d209      	bcs.n	ad6 <main+0x27a>
 ac2:	681a      	ldr	r2, [r3, #0]
 ac4:	3201      	adds	r2, #1
 ac6:	601a      	str	r2, [r3, #0]
 ac8:	4b48      	ldr	r3, [pc, #288]	; (bec <main+0x390>)
 aca:	6818      	ldr	r0, [r3, #0]
 acc:	4b5a      	ldr	r3, [pc, #360]	; (c38 <main+0x3dc>)
 ace:	18c0      	adds	r0, r0, r3
 ad0:	f7ff fc9e 	bl	410 <send_radio_data>
 ad4:	e05d      	b.n	b92 <main+0x336>
 ad6:	2200      	movs	r2, #0
 ad8:	e06e      	b.n	bb8 <main+0x35c>
 ada:	2d04      	cmp	r5, #4
 adc:	d168      	bne.n	bb0 <main+0x354>
 ade:	22ff      	movs	r2, #255	; 0xff
 ae0:	494f      	ldr	r1, [pc, #316]	; (c20 <main+0x3c4>)
 ae2:	4013      	ands	r3, r2
 ae4:	600b      	str	r3, [r1, #0]
 ae6:	4b58      	ldr	r3, [pc, #352]	; (c48 <main+0x3ec>)
 ae8:	4014      	ands	r4, r2
 aea:	4a4e      	ldr	r2, [pc, #312]	; (c24 <main+0x3c8>)
 aec:	601c      	str	r4, [r3, #0]
 aee:	6819      	ldr	r1, [r3, #0]
 af0:	6812      	ldr	r2, [r2, #0]
 af2:	4291      	cmp	r1, r2
 af4:	d301      	bcc.n	afa <main+0x29e>
 af6:	2200      	movs	r2, #0
 af8:	601a      	str	r2, [r3, #0]
 afa:	4c3d      	ldr	r4, [pc, #244]	; (bf0 <main+0x394>)
 afc:	6823      	ldr	r3, [r4, #0]
 afe:	2b02      	cmp	r3, #2
 b00:	d814      	bhi.n	b2c <main+0x2d0>
 b02:	6823      	ldr	r3, [r4, #0]
 b04:	3301      	adds	r3, #1
 b06:	6023      	str	r3, [r4, #0]
 b08:	6820      	ldr	r0, [r4, #0]
 b0a:	4b4b      	ldr	r3, [pc, #300]	; (c38 <main+0x3dc>)
 b0c:	18c0      	adds	r0, r0, r3
 b0e:	f7ff fc7f 	bl	410 <send_radio_data>
 b12:	6822      	ldr	r2, [r4, #0]
 b14:	4b42      	ldr	r3, [pc, #264]	; (c20 <main+0x3c4>)
 b16:	6818      	ldr	r0, [r3, #0]
 b18:	2a02      	cmp	r2, #2
 b1a:	d100      	bne.n	b1e <main+0x2c2>
 b1c:	0040      	lsls	r0, r0, #1
 b1e:	b280      	uxth	r0, r0
 b20:	2101      	movs	r1, #1
 b22:	2200      	movs	r2, #0
 b24:	f7ff fade 	bl	e4 <set_wakeup_timer>
 b28:	f7ff fcf8 	bl	51c <operation_sleep_noirqreset>
 b2c:	4c3e      	ldr	r4, [pc, #248]	; (c28 <main+0x3cc>)
 b2e:	4b47      	ldr	r3, [pc, #284]	; (c4c <main+0x3f0>)
 b30:	6822      	ldr	r2, [r4, #0]
 b32:	25f0      	movs	r5, #240	; 0xf0
 b34:	0092      	lsls	r2, r2, #2
 b36:	58d7      	ldr	r7, [r2, r3]
 b38:	6822      	ldr	r2, [r4, #0]
 b3a:	4b45      	ldr	r3, [pc, #276]	; (c50 <main+0x3f4>)
 b3c:	0092      	lsls	r2, r2, #2
 b3e:	042d      	lsls	r5, r5, #16
 b40:	2064      	movs	r0, #100	; 0x64
 b42:	58d6      	ldr	r6, [r2, r3]
 b44:	f7ff faa9 	bl	9a <delay>
 b48:	1c28      	adds	r0, r5, #0
 b4a:	4338      	orrs	r0, r7
 b4c:	f7ff fc60 	bl	410 <send_radio_data>
 b50:	4840      	ldr	r0, [pc, #256]	; (c54 <main+0x3f8>)
 b52:	f7ff faa2 	bl	9a <delay>
 b56:	1c28      	adds	r0, r5, #0
 b58:	4330      	orrs	r0, r6
 b5a:	f7ff fc59 	bl	410 <send_radio_data>
 b5e:	4a3a      	ldr	r2, [pc, #232]	; (c48 <main+0x3ec>)
 b60:	2500      	movs	r5, #0
 b62:	6813      	ldr	r3, [r2, #0]
 b64:	42ab      	cmp	r3, r5
 b66:	d102      	bne.n	b6e <main+0x312>
 b68:	6825      	ldr	r5, [r4, #0]
 b6a:	1e6b      	subs	r3, r5, #1
 b6c:	419d      	sbcs	r5, r3
 b6e:	6811      	ldr	r1, [r2, #0]
 b70:	2300      	movs	r3, #0
 b72:	4299      	cmp	r1, r3
 b74:	d007      	beq.n	b86 <main+0x32a>
 b76:	6813      	ldr	r3, [r2, #0]
 b78:	4a2a      	ldr	r2, [pc, #168]	; (c24 <main+0x3c8>)
 b7a:	6821      	ldr	r1, [r4, #0]
 b7c:	6812      	ldr	r2, [r2, #0]
 b7e:	18cb      	adds	r3, r1, r3
 b80:	429a      	cmp	r2, r3
 b82:	419b      	sbcs	r3, r3
 b84:	425b      	negs	r3, r3
 b86:	431d      	orrs	r5, r3
 b88:	4c27      	ldr	r4, [pc, #156]	; (c28 <main+0x3cc>)
 b8a:	d005      	beq.n	b98 <main+0x33c>
 b8c:	6823      	ldr	r3, [r4, #0]
 b8e:	3b01      	subs	r3, #1
 b90:	6023      	str	r3, [r4, #0]
 b92:	4b23      	ldr	r3, [pc, #140]	; (c20 <main+0x3c4>)
 b94:	6818      	ldr	r0, [r3, #0]
 b96:	e7c2      	b.n	b1e <main+0x2c2>
 b98:	482e      	ldr	r0, [pc, #184]	; (c54 <main+0x3f8>)
 b9a:	f7ff fa7e 	bl	9a <delay>
 b9e:	4826      	ldr	r0, [pc, #152]	; (c38 <main+0x3dc>)
 ba0:	f7ff fc36 	bl	410 <send_radio_data>
 ba4:	4b12      	ldr	r3, [pc, #72]	; (bf0 <main+0x394>)
 ba6:	601d      	str	r5, [r3, #0]
 ba8:	4b1e      	ldr	r3, [pc, #120]	; (c24 <main+0x3c8>)
 baa:	681b      	ldr	r3, [r3, #0]
 bac:	6023      	str	r3, [r4, #0]
 bae:	e745      	b.n	a3c <main+0x1e0>
 bb0:	2d11      	cmp	r5, #17
 bb2:	d103      	bne.n	bbc <main+0x360>
 bb4:	4a21      	ldr	r2, [pc, #132]	; (c3c <main+0x3e0>)
 bb6:	4b09      	ldr	r3, [pc, #36]	; (bdc <main+0x380>)
 bb8:	601a      	str	r2, [r3, #0]
 bba:	e73f      	b.n	a3c <main+0x1e0>
 bbc:	f7ff fcce 	bl	55c <operation_cdc_run>
 bc0:	e7fc      	b.n	bbc <main+0x360>
 bc2:	46c0      	nop			; (mov r8, r8)
 bc4:	e000e280 	.word	0xe000e280
 bc8:	e000e100 	.word	0xe000e100
 bcc:	000f4240 	.word	0x000f4240
 bd0:	00000c7c 	.word	0x00000c7c
 bd4:	deadbeef 	.word	0xdeadbeef
 bd8:	8f77184b 	.word	0x8f77184b
 bdc:	a200000c 	.word	0xa200000c
 be0:	00202d08 	.word	0x00202d08
 be4:	a2000008 	.word	0xa2000008
 be8:	00000c80 	.word	0x00000c80
 bec:	00000c70 	.word	0x00000c70
 bf0:	00000cac 	.word	0x00000cac
 bf4:	00000ca4 	.word	0x00000ca4
 bf8:	00000c5c 	.word	0x00000c5c
 bfc:	fffffeff 	.word	0xfffffeff
 c00:	00000c60 	.word	0x00000c60
 c04:	00000c64 	.word	0x00000c64
 c08:	fff003ff 	.word	0xfff003ff
 c0c:	00000c58 	.word	0x00000c58
 c10:	fffffbff 	.word	0xfffffbff
 c14:	ffe07fff 	.word	0xffe07fff
 c18:	fffffc0f 	.word	0xfffffc0f
 c1c:	00000cb0 	.word	0x00000cb0
 c20:	00000c98 	.word	0x00000c98
 c24:	00000c94 	.word	0x00000c94
 c28:	00000c68 	.word	0x00000c68
 c2c:	00000c90 	.word	0x00000c90
 c30:	00000c6c 	.word	0x00000c6c
 c34:	00000cb4 	.word	0x00000cb4
 c38:	00faf000 	.word	0x00faf000
 c3c:	8f77183b 	.word	0x8f77183b
 c40:	00000ca0 	.word	0x00000ca0
 c44:	00000c74 	.word	0x00000c74
 c48:	00000c9c 	.word	0x00000c9c
 c4c:	00000d58 	.word	0x00000d58
 c50:	00000cb8 	.word	0x00000cb8
 c54:	00001770 	.word	0x00001770

