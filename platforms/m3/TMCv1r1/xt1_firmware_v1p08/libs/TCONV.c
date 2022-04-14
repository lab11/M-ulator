//*******************************************************************************************
// TCONV HEADER FILE
//-------------------------------------------------------------------------------------------
//  Temperature Conversion for XT1
//  This implementation is based on Zhiyoong's work: 20220321 - Temperature Conversion.docx
//-------------------------------------------------------------------------------------------
// < AUTHOR > 
//  Yejoong Kim (yejoong@cubeworks.io)
//******************************************************************************************* 

//*********************************************************
// HEADER AND OTHER SOURCE FILES
//*********************************************************
#include "TCONV.h"

//-------------------------
// Index       MSBs    Hex
// -----------------------
//   0 -   91  3'b000  0x0
//  92 -  192  3'b001  0x1
// 193 -  302  3'b010  0x2
// 303 -  422  3'b011  0x3
// 423 -  553  3'b100  0x4
// 554 -  696  3'b101  0x5
// 697 -  852  3'b110  0x6
// 853 - 1022  3'b111  0x7
//-------------------------

// tconv_log2_lut[] provides the LSB only.
// You need to prepend the MSBs shown above,
// which is handled in tconv_log2()
volatile uint32_t tconv_log2_lut[] = {
    // Index    Value           Table Index
    /*   0 */   0x0C090603, //    3 -    0
    /*   1 */   0x1714110E, //    7 -    4
    /*   2 */   0x22201D1A, //   11 -    8
    /*   3 */   0x2E2B2825, //   15 -   12
    /*   4 */   0x39363331, //   19 -   16
    /*   5 */   0x44423F3C, //   23 -   20
    /*   6 */   0x504D4A47, //   27 -   24
    /*   7 */   0x5B585553, //   31 -   28
    /*   8 */   0x6663615E, //   35 -   32
    /*   9 */   0x716E6C69, //   39 -   36
    /*  10 */   0x7C7A7774, //   43 -   40
    /*  11 */   0x8785827F, //   47 -   44
    /*  12 */   0x92908D8A, //   51 -   48
    /*  13 */   0x9D9B9895, //   55 -   52
    /*  14 */   0xA8A6A3A0, //   59 -   56
    /*  15 */   0xB3B0AEAB, //   63 -   60
    /*  16 */   0xBEBBB9B6, //   67 -   64
    /*  17 */   0xC9C6C3C1, //   71 -   68
    /*  18 */   0xD4D1CECB, //   75 -   72
    /*  19 */   0xDEDCD9D6, //   79 -   76
    /*  20 */   0xE9E6E4E1, //   83 -   80
    /*  21 */   0xF4F1EEEC, //   87 -   84
    /*  22 */   0xFEFCF9F6, //   91 -   88
    /*  23 */   0x09060301, //   95 -   92
    /*  24 */   0x13110E0B, //   99 -   96
    /*  25 */   0x1E1B1916, //  103 -  100
    /*  26 */   0x28262320, //  107 -  104
    /*  27 */   0x33302D2B, //  111 -  108
    /*  28 */   0x3D3A3835, //  115 -  112
    /*  29 */   0x47454240, //  119 -  116
    /*  30 */   0x524F4D4A, //  123 -  120
    /*  31 */   0x5C595754, //  127 -  124
    /*  32 */   0x6664615F, //  131 -  128
    /*  33 */   0x706E6B69, //  135 -  132
    /*  34 */   0x7B787673, //  139 -  136
    /*  35 */   0x8582807D, //  143 -  140
    /*  36 */   0x8F8C8A87, //  147 -  144
    /*  37 */   0x99969491, //  151 -  148
    /*  38 */   0xA3A09E9B, //  155 -  152
    /*  39 */   0xADAAA8A5, //  159 -  156
    /*  40 */   0xB7B4B2AF, //  163 -  160
    /*  41 */   0xC1BEBCB9, //  167 -  164
    /*  42 */   0xCBC8C6C3, //  171 -  168
    /*  43 */   0xD5D2D0CD, //  175 -  172
    /*  44 */   0xDEDCDAD7, //  179 -  176
    /*  45 */   0xE8E6E3E1, //  183 -  180
    /*  46 */   0xF2F0EDEB, //  187 -  184
    /*  47 */   0xFCF9F7F4, //  191 -  188
    /*  48 */   0x050301FE, //  195 -  192
    /*  49 */   0x0F0D0A08, //  199 -  196
    /*  50 */   0x19161412, //  203 -  200
    /*  51 */   0x22201E1B, //  207 -  204
    /*  52 */   0x2C2A2725, //  211 -  208
    /*  53 */   0x3633312E, //  215 -  212
    /*  54 */   0x3F3D3A38, //  219 -  216
    /*  55 */   0x49464441, //  223 -  220
    /*  56 */   0x52504D4B, //  227 -  224
    /*  57 */   0x5B595754, //  231 -  228
    /*  58 */   0x6562605E, //  235 -  232
    /*  59 */   0x6E6C6967, //  239 -  236
    /*  60 */   0x77757370, //  243 -  240
    /*  61 */   0x817E7C7A, //  247 -  244
    /*  62 */   0x8A888583, //  251 -  248
    /*  63 */   0x93918F8C, //  255 -  252
    /*  64 */   0x9D9A9896, //  259 -  256
    /*  65 */   0xA6A3A19F, //  263 -  260
    /*  66 */   0xAFADAAA8, //  267 -  264
    /*  67 */   0xB8B6B3B1, //  271 -  268
    /*  68 */   0xC1BFBDBA, //  275 -  272
    /*  69 */   0xCAC8C6C3, //  279 -  276
    /*  70 */   0xD3D1CFCC, //  283 -  280
    /*  71 */   0xDCDAD8D6, //  287 -  284
    /*  72 */   0xE5E3E1DF, //  291 -  288
    /*  73 */   0xEEECEAE8, //  295 -  292
    /*  74 */   0xF7F5F3F0, //  299 -  296
    /*  75 */   0x00FEFCF9, //  303 -  300
    /*  76 */   0x09070502, //  307 -  304
    /*  77 */   0x12100D0B, //  311 -  308
    /*  78 */   0x1B181614, //  315 -  312
    /*  79 */   0x23211F1D, //  319 -  316
    /*  80 */   0x2C2A2826, //  323 -  320
    /*  81 */   0x3533312E, //  327 -  324
    /*  82 */   0x3E3C3937, //  331 -  328
    /*  83 */   0x46444240, //  335 -  332
    /*  84 */   0x4F4D4B49, //  339 -  336
    /*  85 */   0x58565351, //  343 -  340
    /*  86 */   0x605E5C5A, //  347 -  344
    /*  87 */   0x69676563, //  351 -  348
    /*  88 */   0x726F6D6B, //  355 -  352
    /*  89 */   0x7A787674, //  359 -  356
    /*  90 */   0x83817E7C, //  363 -  360
    /*  91 */   0x8B898785, //  367 -  364
    /*  92 */   0x94928F8D, //  371 -  368
    /*  93 */   0x9C9A9896, //  375 -  372
    /*  94 */   0xA5A2A09E, //  379 -  376
    /*  95 */   0xADABA9A7, //  383 -  380
    /*  96 */   0xB5B3B1AF, //  387 -  384
    /*  97 */   0xBEBCB9B7, //  391 -  388
    /*  98 */   0xC6C4C2C0, //  395 -  392
    /*  99 */   0xCECCCAC8, //  399 -  396
    /* 100 */   0xD7D5D2D0, //  403 -  400
    /* 101 */   0xDFDDDBD9, //  407 -  404
    /* 102 */   0xE7E5E3E1, //  411 -  408
    /* 103 */   0xEFEDEBE9, //  415 -  412
    /* 104 */   0xF8F5F3F1, //  419 -  416
    /* 105 */   0x00FEFCFA, //  423 -  420
    /* 106 */   0x08060402, //  427 -  424
    /* 107 */   0x100E0C0A, //  431 -  428
    /* 108 */   0x18161412, //  435 -  432
    /* 109 */   0x201E1C1A, //  439 -  436
    /* 110 */   0x28262422, //  443 -  440
    /* 111 */   0x302E2C2A, //  447 -  444
    /* 112 */   0x38363432, //  451 -  448
    /* 113 */   0x403E3C3A, //  455 -  452
    /* 114 */   0x48464442, //  459 -  456
    /* 115 */   0x504E4C4A, //  463 -  460
    /* 116 */   0x58565452, //  467 -  464
    /* 117 */   0x605E5C5A, //  471 -  468
    /* 118 */   0x68666462, //  475 -  472
    /* 119 */   0x706E6C6A, //  479 -  476
    /* 120 */   0x78767472, //  483 -  480
    /* 121 */   0x7F7E7C7A, //  487 -  484
    /* 122 */   0x87858381, //  491 -  488
    /* 123 */   0x8F8D8B89, //  495 -  492
    /* 124 */   0x97959391, //  499 -  496
    /* 125 */   0x9F9D9B99, //  503 -  500
    /* 126 */   0xA6A4A2A1, //  507 -  504
    /* 127 */   0xAEACAAA8, //  511 -  508
    /* 128 */   0xB6B4B2B0, //  515 -  512
    /* 129 */   0xBDBBBAB8, //  519 -  516
    /* 130 */   0xC5C3C1BF, //  523 -  520
    /* 131 */   0xCDCBC9C7, //  527 -  524
    /* 132 */   0xD4D2D0CF, //  531 -  528
    /* 133 */   0xDCDAD8D6, //  535 -  532
    /* 134 */   0xE3E1E0DE, //  539 -  536
    /* 135 */   0xEBE9E7E5, //  543 -  540
    /* 136 */   0xF2F1EFED, //  547 -  544
    /* 137 */   0xFAF8F6F4, //  551 -  548
    /* 138 */   0x0100FEFC, //  555 -  552
    /* 139 */   0x09070503, //  559 -  556
    /* 140 */   0x100F0D0B, //  563 -  560
    /* 141 */   0x18161412, //  567 -  564
    /* 142 */   0x1F1D1C1A, //  571 -  568
    /* 143 */   0x27252321, //  575 -  572
    /* 144 */   0x2E2C2A28, //  579 -  576
    /* 145 */   0x35343230, //  583 -  580
    /* 146 */   0x3D3B3937, //  587 -  584
    /* 147 */   0x4442403F, //  591 -  588
    /* 148 */   0x4B494846, //  595 -  592
    /* 149 */   0x53514F4D, //  599 -  596
    /* 150 */   0x5A585654, //  603 -  600
    /* 151 */   0x615F5E5C, //  607 -  604
    /* 152 */   0x68676563, //  611 -  608
    /* 153 */   0x706E6C6A, //  615 -  612
    /* 154 */   0x77757371, //  619 -  616
    /* 155 */   0x7E7C7A79, //  623 -  620
    /* 156 */   0x85838280, //  627 -  624
    /* 157 */   0x8C8A8987, //  631 -  628
    /* 158 */   0x9392908E, //  635 -  632
    /* 159 */   0x9B999795, //  639 -  636
    /* 160 */   0xA2A09E9C, //  643 -  640
    /* 161 */   0xA9A7A5A3, //  647 -  644
    /* 162 */   0xB0AEACAA, //  651 -  648
    /* 163 */   0xB7B5B3B1, //  655 -  652
    /* 164 */   0xBEBCBAB9, //  659 -  656
    /* 165 */   0xC5C3C1C0, //  663 -  660
    /* 166 */   0xCCCAC8C7, //  667 -  664
    /* 167 */   0xD3D1CFCE, //  671 -  668
    /* 168 */   0xDAD8D6D5, //  675 -  672
    /* 169 */   0xE1DFDDDB, //  679 -  676
    /* 170 */   0xE8E6E4E2, //  683 -  680
    /* 171 */   0xEFEDEBE9, //  687 -  684
    /* 172 */   0xF5F4F2F0, //  691 -  688
    /* 173 */   0xFCFBF9F7, //  695 -  692
    /* 174 */   0x030100FE, //  699 -  696
    /* 175 */   0x0A080705, //  703 -  700
    /* 176 */   0x110F0D0C, //  707 -  704
    /* 177 */   0x18161413, //  711 -  708
    /* 178 */   0x1E1D1B19, //  715 -  712
    /* 179 */   0x25242220, //  719 -  716
    /* 180 */   0x2C2A2927, //  723 -  720
    /* 181 */   0x33312F2E, //  727 -  724
    /* 182 */   0x3A383634, //  731 -  728
    /* 183 */   0x403F3D3B, //  735 -  732
    /* 184 */   0x47454442, //  739 -  736
    /* 185 */   0x4E4C4A49, //  743 -  740
    /* 186 */   0x5453514F, //  747 -  744
    /* 187 */   0x5B595856, //  751 -  748
    /* 188 */   0x62605E5D, //  755 -  752
    /* 189 */   0x68676563, //  759 -  756
    /* 190 */   0x6F6D6C6A, //  763 -  760
    /* 191 */   0x75747271, //  767 -  764
    /* 192 */   0x7C7A7977, //  771 -  768
    /* 193 */   0x83817F7E, //  775 -  772
    /* 194 */   0x89888684, //  779 -  776
    /* 195 */   0x908E8C8B, //  783 -  780
    /* 196 */   0x96959391, //  787 -  784
    /* 197 */   0x9D9B9A98, //  791 -  788
    /* 198 */   0xA3A2A09E, //  795 -  792
    /* 199 */   0xAAA8A7A5, //  799 -  796
    /* 200 */   0xB0AFADAB, //  803 -  800
    /* 201 */   0xB7B5B3B2, //  807 -  804
    /* 202 */   0xBDBCBAB8, //  811 -  808
    /* 203 */   0xC4C2C0BF, //  815 -  812
    /* 204 */   0xCAC8C7C5, //  819 -  816
    /* 205 */   0xD0CFCDCC, //  823 -  820
    /* 206 */   0xD7D5D4D2, //  827 -  824
    /* 207 */   0xDDDCDAD8, //  831 -  828
    /* 208 */   0xE4E2E0DF, //  835 -  832
    /* 209 */   0xEAE8E7E5, //  839 -  836
    /* 210 */   0xF0EFEDEB, //  843 -  840
    /* 211 */   0xF7F5F3F2, //  847 -  844
    /* 212 */   0xFDFBFAF8, //  851 -  848
    /* 213 */   0x030200FE, //  855 -  852                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 214 */   0x09080605, //  859 -  856                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 215 */   0x100E0D0B, //  863 -  860                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 216 */   0x16141311, //  867 -  864                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 217 */   0x1C1B1917, //  871 -  868                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 218 */   0x22211F1E, //  875 -  872                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 219 */   0x29272524, //  879 -  876                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 220 */   0x2F2D2C2A, //  883 -  880                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    /* 221 */   0x35333230, //  887 -  884                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    /* 222 */   0x3B3A3837, //  891 -  888                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    /* 223 */   0x41403E3D, //  895 -  892                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    /* 224 */   0x47464443, //  899 -  896                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    /* 225 */   0x4E4C4B49, //  903 -  900                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 226 */   0x5452514F, //  907 -  904                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 227 */   0x5A585755, //  911 -  908                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    /* 228 */   0x605E5D5B, //  915 -  912                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 229 */   0x66646361, //  919 -  916                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    /* 230 */   0x6C6B6968, //  923 -  920                                                                                                                                                                                                                                                                                                                                                                                                                                    
    /* 231 */   0x72716F6E, //  927 -  924                                                                                                                                                                                                                                                                                                                                                                                                                                    
    /* 232 */   0x78777574, //  931 -  928                                                                                                                                                                                                                                                                                             
    /* 233 */   0x7E7D7B7A, //  935 -  932
    /* 234 */   0x84838180, //  939 -  936
    /* 235 */   0x8A898786, //  943 -  940
    /* 236 */   0x908F8D8C, //  947 -  944
    /* 237 */   0x96959392, //  951 -  948
    /* 238 */   0x9C9B9998, //  955 -  952
    /* 239 */   0xA2A19F9E, //  959 -  956
    /* 240 */   0xA8A7A5A4, //  963 -  960
    /* 241 */   0xAEADABAA, //  967 -  964
    /* 242 */   0xB4B3B1B0, //  971 -  968
    /* 243 */   0xBAB8B7B5, //  975 -  972
    /* 244 */   0xC0BEBDBB, //  979 -  976
    /* 245 */   0xC6C4C3C1, //  983 -  980
    /* 246 */   0xCCCAC9C7, //  987 -  984
    /* 247 */   0xD1D0CFCD, //  991 -  988
    /* 248 */   0xD7D6D4D3, //  995 -  992
    /* 249 */   0xDDDCDAD9, //  999 -  996
    /* 250 */   0xE3E2E0DF, // 1003 - 1000
    /* 251 */   0xE9E7E6E4, // 1007 - 1004
    /* 252 */   0xEFEDECEA, // 1011 - 1008
    /* 253 */   0xF4F3F2F0, // 1015 - 1012
    /* 254 */   0xFAF9F7F6, // 1019 - 1016
    /* 255 */   0x00FFFDFC, // 1023 - 1020 (1023 is dummy)
    };


uint32_t tconv_log2 (uint32_t idx) {
    uint32_t msb, lsb;

    if (idx==1023) return 0;

    lsb = (tconv_log2_lut[idx>>2]>>((idx&0x3)<<3))&0xFF;

    if      (idx <  92) msb = 0x0;
    else if (idx < 193) msb = 0x1;
    else if (idx < 303) msb = 0x2;
    else if (idx < 423) msb = 0x3;
    else if (idx < 554) msb = 0x4;
    else if (idx < 697) msb = 0x5;
    else if (idx < 853) msb = 0x6;
    else                msb = 0x7;

    return (msb<<8)|lsb;
}

uint32_t div_n11 (uint32_t dividend, uint32_t divisor) {
#ifdef TCONV_DEBUG
    mbus_write_message32(0xC0, dividend);
    mbus_write_message32(0xC1, divisor);
#endif

    // Find out the position of leading 1
    uint32_t dividend_m = 32;
    while (dividend_m>0) {
        dividend_m--;
        if (dividend&(0x1<<dividend_m)) break;
    }
    uint32_t divisor_m = 32;
    while (divisor_m>0) {
        divisor_m--;
        if (divisor&(0x1<<divisor_m)) break;
    }

    uint32_t inc = (0x1 << 11);

    if (dividend_m > divisor_m) {
        divisor <<= (dividend_m - divisor_m);
        inc     <<= (dividend_m - divisor_m);
    }
    else if (divisor_m > dividend_m) {
        dividend <<= (divisor_m - dividend_m);
        inc      >>= (divisor_m - dividend_m);
    }

    uint32_t result = 0;
    while ((dividend!=0)&&(inc!=0)) {
        if (dividend>=divisor) {
            dividend -= divisor;
            result |= inc;
        }
        divisor >>= 1;
        inc     >>= 1;
    }

#ifdef TCONV_DEBUG
    mbus_write_message32(0xC2, result);
#endif

    return result;
}

uint32_t tconv (uint32_t dout, uint32_t a, uint32_t b, uint32_t offset) {
    // NOTE: dout is a 16-bit raw code
    //       a is a fixed-point number with N=10 accuracy
    //       b is a fixed-point number with N=10 accuracy

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA0, dout);
    mbus_write_message32(0xA0, a);
    mbus_write_message32(0xA0, b);
    mbus_write_message32(0xA0, offset);
#endif

    // If dout=0, just return 0.
    if (dout==0) return 0;

    // Find out the position of leading 1
    uint32_t m = 16;
    while (m>0) {
        m--;
        if (dout&(0x1<<m)) break;
    }

    if (m>10) {
        if ((dout>>(m-11))&0x1) { 
            dout += (0x1<<(m-11));
            if (((dout>>(m-10))&0x3FF)==0) 
                m++;
        }
    }

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA1, m);
#endif

    // Calculate log2(dout)
    // NOTE: idx=1023 returns 0 (i.e., table[1023]=0)
    uint32_t idx;
    //--- If dout is a power of 2
    if ((dout&(~(0x1<<m)))==0) {
        idx = 1024;
    }
    //--- If dout is not a power of 2
    else {
        // CASE III) m>10
        if (m>10) {
            idx = ((dout>>(m-10))&0x3FF);
            if (idx==0) idx=1024;
        }
        // CASE I) m=10 or CASE II) m<10
        else {
            idx = ((dout<<(10-m))&0x3FF);
        }
    }
    // Decrement idx
    idx--;

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA2, idx);
#endif

    // NOTE: log2_dout is N=11 accuracy
    uint32_t log2_dout = (m<<11) + tconv_log2(idx);

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA3, log2_dout);
#endif

    // NOTE: c = log2(Fclk / 2^(ConvTime+5)) = 2.09506730160705 (for Fclk=17500, ConvTime=7)
    //         = ~2.0947265625
    //         = 0000_0000_0000_0000_0000_1000_0110_0001 (N=10 fixed-point)
    uint32_t c = 0x00000861;

    // Division:  d = b / (a - c - log2_dout)
    // NOTE: a, b, c have N=10 accuracy, but log2_dout has N=11 accuracy
    //       -> Multiply a, b, c by 2.
    // NOTE: temp_n11 has N=11 accuracy.
    uint32_t temp_n11 = div_n11(b<<1, (a<<1)-(c<<1)-log2_dout);

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA4, temp_n11);
#endif

    // temp10 = (temp_n11 x 10) with decimal points truncated
    uint32_t temp10 = ((temp_n11<<3) + (temp_n11<<1));
    if ((temp10>>10)&0x1) temp10+=(0x1<<10); // (optional) Add a round-up
    temp10 = temp10 >> 11;  // truncate the decimal points

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA5, temp10);
#endif

    // 'result' subtracts the offset from temp10, 
    uint32_t offset_10k = (offset<<3) + (offset<<1);
    if (temp10<offset_10k) temp10 = offset_10k;
    uint32_t result = temp10 - offset_10k;

#ifdef TCONV_DEBUG
    mbus_write_message32(0xA6, result);
#endif

    return result&0xFFFF;  // 10x(T+273-OFFSET)
}
