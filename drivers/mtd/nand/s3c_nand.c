/* linux/drivers/mtd/nand/s3c_nand.c
 *
 * Copyright (c) 2007 Samsung Electronics
 *
 * Samsung S3C NAND driver
 *
 * $Id: s3c_nand.c,v 1.3 2008/11/19 10:07:24 jsgood Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Based on nand driver from Ben Dooks <ben@simtec.co.uk>
 * modified by scsuh. based on au1550nd.c
 *
 * Many functions for hardware ecc are implemented by jsgood.
 */

/* Simple H/W Table for Implementation of S3C nand driver
 * by scsuh
 * ------------------------------------------------------------------
 * |    En/Dis CE           |  required  |                          |
 * |    En/Dis ALE          |      X     | * nand controller does   |
 * |    En/Dis CLE          |      X     | * nand controller does   |
 * |    Wait/Ready          |  required  |                          |
 * |    Write Command       |  required  |                          |
 * |    Write Address       |  required  |                          |
 * |    Write Data          |  required  |                          |
 * |    Read Data           |  required  |                          |
 * |    WP on/off           |  required  | * board specific         |
 * |    AP Specific Init    |  required  |                          |
 * ------------------------------------------------------------------
 */

#include <common.h>

#ifdef CONFIG_CMD_NAND
#include <nand.h>
#include <regs.h>
#include <asm/io.h>
#include <asm/errno.h>

/* When NAND is used as boot device, below is set to 1. */
int boot_nand = 0;

static struct mtd_info *s3c_mtd = NULL;

/* Nand flash definition values by jsgood */
#define S3C_NAND_TYPE_UNKNOWN	0x0
#define S3C_NAND_TYPE_SLC	0x1
#define S3C_NAND_TYPE_MLC_4BIT	0x2
#define S3C_NAND_TYPE_MLC_8BIT	0x3

/*
 * Cached progamming disabled for now, Not sure if its worth the
 * trouble. The speed gain is not very impressive. (2.3->2.6Mib/s)
 *
 * if want to use cached program, define next
 * by jsgood (modified to keep prevent rule)
 */
#undef	CONFIG_MTD_NAND_S3C_CACHEDPROG

/* Nand flash global values by jsgood */
int cur_ecc_mode = 0;
int nand_type = S3C_NAND_TYPE_UNKNOWN;

/* Nand flash oob definition for SLC 512b page size by jsgood */
static struct nand_ecclayout s3c_nand_oob_16 = {
    .eccbytes = 4,
    .eccpos = {1, 2, 3, 4},
    .oobfree = {
        {.offset = 6,
         .length = 10}}
};

/* Nand flash oob definition for SLC 2k page size by jsgood */
static struct nand_ecclayout s3c_nand_oob_64 = {
    .eccbytes = 16,
    .eccpos = {40, 41, 42, 43, 44, 45, 46, 47,
               48, 49, 50, 51, 52, 53, 54, 55},
    .oobfree = {
        {.offset = 2,
         .length = 38}}
};

/* Nand flash oob definition for MLC 2k page size by jsgood */
static struct nand_ecclayout s3c_nand_oob_mlc_64 = {
    .eccbytes = 32,
    .eccpos = {
        32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55,
        56, 57, 58, 59, 60, 61, 62, 63},
    .oobfree = {
        {.offset = 2,
         .length = 28}}
};


static struct nand_ecclayout s3c_nand_oob_mlc_128_4bit = {
    .eccbytes = 64,
    .eccpos = {
        64,65,66,67,68,69,70,71,72,73,
        74,75,76,77,78,79,80,81,82,83,
        84,85,86,87,88,89,90,91,92,93,
        94,95,96,97,98,99,100,101,102,103,
        104,105,106,107,108,109,110,111,112,113,
        114,115,116,117,118,119,120,121,122,123,
        124,125,126,127},
    .oobfree = {
        {.offset = 2,
         .length = 60}}
};


static struct nand_ecclayout s3c_nand_oob_mlc_128_8bit = {
    .eccbytes = 104,
    .eccpos = {
        24,25,26,27,28,29,30,31,32,33,
        34,35,36,37,38,39,40,41,42,43,
        44,45,46,47,48,49,50,51,52,53,
        54,55,56,57,58,59,60,61,62,63,
        64,65,66,67,68,69,70,71,72,73,
        74,75,76,77,78,79,80,81,82,83,
        84,85,86,87,88,89,90,91,92,93,
        94,95,96,97,98,99,100,101,102,103,
        104,105,106,107,108,109,110,111,112,113,
        114,115,116,117,118,119,120,121,122,123,
        124,125,126,127},
    .oobfree = {
        {.offset = 2,
         .length = 20}}
};

// samsung  K9GAG08U0E  MLC nand flash  oob size =436 Byte
static struct nand_ecclayout s3c_nand_oob_mlc_232_8bit = {
    .eccbytes = 208,
    .eccpos = {
        24,25,26,27,28,29,30,31,32,33,
        34,35,36,37,38,39,40,41,42,43,
        44,45,46,47,48,49,50,51,52,53,
        54,55,56,57,58,59,60,61,62,63,
        64,65,66,67,68,69,70,71,72,73,
        74,75,76,77,78,79,80,81,82,83,
        84,85,86,87,88,89,90,91,92,93,
        94,95,96,97,98,99,100,101,102,103,
        104,105,106,107,108,109,110,111,112,113,
        114,115,116,117,118,119,120,121,122,123,
        124,125,126,127,


        128,129,130,131,132,133,134,135,136,137,
        138,139,140,141,142,143,144,145,146,147,
        148,149,150,151,152,153,154,155,156,157,
        158,159,160,161,162,163,164,165,166,167,
        168,169,170,171,172,173,174,175,176,177,
        178,179,180,181,182,183,184,185,186,187,
        188,189,190,191,192,193,194,195,196,197,
        198,199,200,201,202,203,204,205,206,207,
        208,209,210,211,212,213,214,215,216,217,
        218,219,220,221,222,223,224,225,226,227,
        228,229,230,231
    },
    .oobfree = {
        {.offset = 2,
         .length = 22}}
};

#if defined(CONFIG_MTD_NAND_S3C_DEBUG)
/*
 * Function to print out oob buffer for debugging
 * Written by jsgood
 */
void print_oob(const char *header, struct mtd_info *mtd)
{
    int i;
    struct nand_chip *chip = mtd->priv;

    printk("%s:\t", header);

    for(i = 0; i < 64; i++)
        printk("%02x ", chip->oob_poi[i]);

    printk("\n");
}
EXPORT_SYMBOL(print_oob);
#endif

#if defined(CONFIG_MTD_NAND_HWECC)
/*
 * Hardware specific access to control-lines function
 * Written by jsgood
 */
static void s3c_nand_hwcontrol(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
	unsigned int cur;

	if (ctrl & NAND_CTRL_CHANGE) {
		if (ctrl & NAND_NCE) {
			if (dat != NAND_CMD_NONE) {
				cur = readl(NFCONT);
				if (boot_nand)
					cur &= ~NFCONT_CS;
				else
					cur &= ~NFCONT_CS_ALT;
				
				writel(cur,  NFCONT);
			}
			} else {
				cur = readl( NFCONT);
				 if (boot_nand)
					cur |= NFCONT_CS;
				else
					cur |= NFCONT_CS_ALT;
				writel(cur, NFCONT);
			}
		}

		if (dat != NAND_CMD_NONE) {
			if (ctrl & NAND_CLE)
				writeb(dat,  NFCMMD);
			else if (ctrl & NAND_ALE)
				writeb(dat, NFADDR);
	}
}

/*
 * Function for checking device ready pin
 * Written by jsgood
 */
static int s3c_nand_device_ready(struct mtd_info *mtd)
{
	/* it's to check the RnB nand signal bit and return to device ready condition in nand_base.c */
	return (!(readl(NFSTAT) & NFSTAT_RnB));
}

static void s3c_nand_wait_ecc_busy_8bit(void)
{
    while (readl(NF8ECCERR0) & NFESTAT0_ECCBUSY);
}

/*
 * Function for checking device ready pin
 * Written by jsgood
 */
static int s3c_nand_device_wait_ready(struct mtd_info *mtdinfo)
{
	while (!(readl(NFSTAT) & NFSTAT_RnB))
	{
		printf("NandFlash is busying.........\n");
	}
	return 1;
}

/*
 * We don't use a bad block table
 */
static int s3c_nand_scan_bbt(struct mtd_info *mtdinfo)
{
   	return nand_default_bbt(mtdinfo);
}

/*
 * Function for checking ECCEncDone in NFSTAT
 * Written by jsgood
 */
static void s3c_nand_wait_enc(void)
{
	while (!(readl(NFSTAT) & NFSTAT_ECCENCDONE));
}

/*
 * Function for checking ECCDecDone in NFSTAT
 * Written by jsgood
 */
static void s3c_nand_wait_dec(void)
{
	while (!(readl(NFSTAT) & NFSTAT_ECCDECDONE));
}

/*
 * Function for checking ECC Busy
 * Written by jsgood
 */
static void s3c_nand_wait_ecc_busy(void)
{
	while (readl(NFESTAT0) & NFESTAT0_ECCBUSY);
}

/*
 * This function is called before encoding ecc codes to ready ecc engine.
 * Written by jsgood
 */
static void s3c_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	u_long nfcont;
	u_long nfconf;

	cur_ecc_mode = mode;

	nfconf = readl(NFCONF);

	nfconf &= ~(0x3 << 23);


	if (nand_type == S3C_NAND_TYPE_SLC)
		nfconf |= NFCONF_ECC_1BIT;
	else if(nand_type == S3C_NAND_TYPE_MLC_4BIT)
		nfconf |= NFCONF_ECC_4BIT;
	else if(nand_type == S3C_NAND_TYPE_MLC_8BIT)
		nfconf |= NFCONF_ECC_8BIT;

    writel(nfconf, NFCONF);

    /* Init main ECC & unlock */
    nfcont = readl(NFCONT);
    nfcont |= NFCONT_INITMECC;
    nfcont &= ~NFCONT_MECCLOCK;

    if (nand_type == S3C_NAND_TYPE_MLC_4BIT || nand_type == S3C_NAND_TYPE_MLC_8BIT) {
        if (mode == NAND_ECC_WRITE)
            nfcont |= NFCONT_ECC_ENC;
        else if (mode == NAND_ECC_READ)
            nfcont &= ~NFCONT_ECC_ENC;
    }

    writel(nfcont, NFCONT);
}

/*
 * This function is called immediately after encoding ecc codes.
 * This function returns encoded ecc codes.
 * Written by jsgood
 */
static int s3c_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
	u_long nfcont, nfmecc0, nfmecc1, nfm8ecc0, nfm8ecc1, nfm8ecc2, nfm8ecc3;;

	/* Lock */
	nfcont = readl(NFCONT);
	nfcont |= NFCONT_MECCLOCK;
	writel(nfcont, NFCONT);

	if (nand_type == S3C_NAND_TYPE_SLC) {
		nfmecc0 = readl(NFMECC0);

		ecc_code[0] = nfmecc0 & 0xff;
		ecc_code[1] = (nfmecc0 >> 8) & 0xff;
		ecc_code[2] = (nfmecc0 >> 16) & 0xff;
		ecc_code[3] = (nfmecc0 >> 24) & 0xff;
	} else if(nand_type == S3C_NAND_TYPE_MLC_4BIT){
		if (cur_ecc_mode == NAND_ECC_READ)
			s3c_nand_wait_dec();
		else {
			s3c_nand_wait_enc();

			nfmecc0 = readl(NFMECC0);
			nfmecc1 = readl(NFMECC1);

			ecc_code[0] = nfmecc0 & 0xff;
			ecc_code[1] = (nfmecc0 >> 8) & 0xff;
			ecc_code[2] = (nfmecc0 >> 16) & 0xff;
			ecc_code[3] = (nfmecc0 >> 24) & 0xff;
			ecc_code[4] = nfmecc1 & 0xff;
			ecc_code[5] = (nfmecc1 >> 8) & 0xff;
			ecc_code[6] = (nfmecc1 >> 16) & 0xff;
			ecc_code[7] = (nfmecc1 >> 24) & 0xff;
		}
	}
	else if(nand_type == S3C_NAND_TYPE_MLC_8BIT){
		if (cur_ecc_mode == NAND_ECC_READ)
			s3c_nand_wait_dec();
		else {
			s3c_nand_wait_enc();

			nfm8ecc0 = readl(NFM8ECC0);
			nfm8ecc1 = readl(NFM8ECC1);
			nfm8ecc2 = readl(NFM8ECC2);
			nfm8ecc3 = readl(NFM8ECC3);

			ecc_code[0] = nfm8ecc0 & 0xff;
			ecc_code[1] = (nfm8ecc0 >> 8) & 0xff;
			ecc_code[2] = (nfm8ecc0 >> 16) & 0xff;
			ecc_code[3] = (nfm8ecc0 >> 24) & 0xff;
			ecc_code[4] = nfm8ecc1 & 0xff;
			ecc_code[5] = (nfm8ecc1 >> 8) & 0xff;
			ecc_code[6] = (nfm8ecc1 >> 16) & 0xff;
			ecc_code[7] = (nfm8ecc1 >> 24) & 0xff;
			ecc_code[8] = nfm8ecc2 & 0xff;
			ecc_code[9] = (nfm8ecc2 >> 8) & 0xff;
			ecc_code[10] = (nfm8ecc2 >> 16) & 0xff;
			ecc_code[11] = (nfm8ecc2 >> 24) & 0xff;
			ecc_code[12] = nfm8ecc3 & 0xff;
		}
	}

	return 0;
}


/*
 * This function determines whether read data is good or not.
 * If SLC, must write ecc codes to controller before reading status bit.
 * If MLC, status bit is already set, so only reading is needed.
 * If status bit is good, return 0.
 * If correctable errors occured, do that.
 * If uncorrectable errors occured, return -1.
 * Written by jsgood
 */
static int s3c_nand_correct_data(struct mtd_info *mtd, u_char *dat, u_char *read_ecc, u_char *calc_ecc)
{
	int ret = -1;
	u_long nfestat0, nfestat1, nfmeccdata0, nfmeccdata1, nfmlcbitpt;
	u_long nf8eccerr0, nf8eccerr1, nf8eccerr2, nfmlc8bitpt0, nfmlc8bitpt1;
	u_char err_type;

	if (nand_type == S3C_NAND_TYPE_SLC) {
		/* SLC: Write ECC data to compare */
		nfmeccdata0 = (read_ecc[1] << 16) | read_ecc[0];
		nfmeccdata1 = (read_ecc[3] << 16) | read_ecc[2];
		writel(nfmeccdata0, NFMECCDATA0);
		writel(nfmeccdata1, NFMECCDATA1);

		/* Read ECC status */
		nfestat0 = readl(NFESTAT0_OFFSET);
		err_type = nfestat0 & 0x3;

		switch (err_type) {
			case 0: /* No error */
				ret = 0;
				break;

			case 1: /* 1 bit error (Correctable)
				(nfestat0 >> 7) & 0x7ff	:error byte number
				(nfestat0 >> 4) & 0x7	:error bit number */
				printf("s3c-nand: SLC 1 bit error detected at byte %ld, correcting from "
				"0x%02x ", (nfestat0 >> 7) & 0x7ff, dat[(nfestat0 >> 7) & 0x7ff]);
				dat[(nfestat0 >> 7) & 0x7ff] ^= (1 << ((nfestat0 >> 4) & 0x7));
				printf("to 0x%02x...OK\n", dat[(nfestat0 >> 7) & 0x7ff]);
				ret = 1;
				break;

			case 2: /* Multiple error */
			case 3: /* ECC area error */
				printf("s3c-nand: SLC ECC uncorrectable error detected ???\n");
				ret = -1;
				break;
		}
	} 
	else if(nand_type == S3C_NAND_TYPE_MLC_4BIT){
		/* MLC: */
		s3c_nand_wait_ecc_busy();

		nfestat0 = readl(NFESTAT0_OFFSET);
		nfestat1 = readl(NFESTAT1_OFFSET);
		nfmlcbitpt = readl(NFMLCBITPT);

		err_type = (nfestat0 >> 26) & 0x7;

		/* No error, If free page (all 0xff) */
		if ((nfestat0 >> 29) & 0x1) {
			err_type = 0;
		} else {
			/* No error, If all 0xff from 17th byte in oob (in case of JFFS2 format) */
			if (dat) {
				if (dat[17] == 0xff && dat[26] == 0xff && dat[35] == 0xff && dat[44] == 0xff && dat[54] == 0xff)
					err_type = 0;
			}
		}

		switch (err_type) {
			case 5: /* Uncorrectable */
				printf("s3c-nand: MLC4 ECC uncorrectable error detected\n");
				ret = -1;
				break;

			case 4: /* 4 bit error (Correctable) */
				dat[(nfestat1 >> 16) & 0x3ff] ^= ((nfmlcbitpt >> 24) & 0xff);

			case 3: /* 3 bit error (Correctable) */
				dat[nfestat1 & 0x3ff] ^= ((nfmlcbitpt >> 16) & 0xff);

			case 2: /* 2 bit error (Correctable) */
				dat[(nfestat0 >> 16) & 0x3ff] ^= ((nfmlcbitpt >> 8) & 0xff);

			case 1: /* 1 bit error (Correctable) */
				printf("s3c-nand: MLC4 %d bit(s) error detected, corrected successfully\n", err_type);
				dat[nfestat0 & 0x3ff] ^= (nfmlcbitpt & 0xff);
				ret = err_type;
				break;

			case 0: /* No error */
				ret = 0;
				break;
		}
	}
	else if(nand_type == S3C_NAND_TYPE_MLC_8BIT){
		while (readl(NF8ECCERR0) & (1<<31)) {}
		nf8eccerr0 = readl(NF8ECCERR0);
		nf8eccerr1 = readl(NF8ECCERR1);
		nf8eccerr2 = readl(NF8ECCERR2);
		nfmlc8bitpt0 = readl(NFMLC8BITPT0);
		nfmlc8bitpt1 = readl(NFMLC8BITPT1);
		err_type = (nf8eccerr0 >> 25) & 0xf;

		/* No error, If free page (all 0xff) */
		if ((nf8eccerr0 >> 29) & 0x1)
			err_type = 0;

		switch (err_type) {
			case 9: /* Uncorrectable */
				printf("s3c-nand: MLC8 ECC uncorrectable error detected\n");
				ret = -1;
				break;

			case 8: /* 8 bit error (Correctable) */
				printf("s3c-nand: MLC8 %d bit(s) error detected, corrected successfully\n", err_type);
				dat[(nf8eccerr2 >> 22) & 0x3ff] ^= ((nfmlc8bitpt1 >> 24) & 0xff);

			case 7: /* 7 bit error (Correctable) */
				printf("s3c-nand: MLC8 %d bit(s) error detected, corrected successfully\n", err_type);
				dat[(nf8eccerr2 >> 11) & 0x3ff] ^= ((nfmlc8bitpt1 >> 16) & 0xff);

			case 6: /* 6 bit error (Correctable) */
				printf("s3c-nand: MLC8 %d bit(s) error detected, corrected successfully\n", err_type);
				dat[nf8eccerr2 & 0x3ff] ^= ((nfmlc8bitpt1 >> 8) & 0xff);

			case 5: /* 5 bit error (Correctable) */
				printf("s3c-nand: MLC8 %d bit(s) error detected, corrected successfully\n", err_type);
				dat[(nf8eccerr1 >> 22) & 0x3ff] ^= (nfmlc8bitpt1 & 0xff);

			case 4: /* 4 bit error (Correctable) */
				printf("s3c-nand: MLC8 %d bit(s) error detected, corrected successfully\n", err_type);
				dat[(nf8eccerr1 >> 11) & 0x3ff] ^= ((nfmlc8bitpt0 >> 24) & 0xff);

			case 3: /* 3 bit error (Correctable) */
				printf("s3c-nand: MLC8 %d bit(s) error detected, corrected successfully\n", err_type);
				dat[nf8eccerr1 & 0x3ff] ^= ((nfmlc8bitpt0 >> 16) & 0xff);

			case 2: /* 2 bit error (Correctable) */
				printf("s3c-nand: MLC8 %d bit(s) error detected, corrected successfully\n", err_type);
				dat[(nf8eccerr0 >> 15) & 0x3ff] ^= ((nfmlc8bitpt0 >> 8) & 0xff);

			case 1: /* 1 bit error (Correctable) */
				printf("s3c-nand: MLC8 %d bit(s) error detected, corrected successfully\n", err_type);
				dat[nf8eccerr0 & 0x3ff] ^= (nfmlc8bitpt0 & 0xff);
				ret = err_type;
				break;

			case 0: /* No error */
				ret = 0;
				break;
			}
	}

	return ret;
}

static int s3c_nand_write_oob_1bit(struct mtd_info *mtd, struct nand_chip *chip,
                                   int page)
{
	uint8_t *ecc_calc = chip->buffers->ecccalc;
	int status = 0;
	int eccbytes = chip->ecc.bytes;
	int secc_start = mtd->oobsize - eccbytes;
	int i;

	chip->cmdfunc(mtd, NAND_CMD_SEQIN, mtd->writesize, page);

	/* spare area */
	chip->ecc.hwctl(mtd, NAND_ECC_WRITE);
	chip->write_buf(mtd, chip->oob_poi, secc_start);
	chip->ecc.calculate(mtd, 0, &ecc_calc[chip->ecc.total]);

	for (i = 0; i < eccbytes; i++)
	chip->oob_poi[secc_start + i] = ecc_calc[chip->ecc.total + i];

	chip->write_buf(mtd, chip->oob_poi + secc_start, eccbytes);

	/* Send command to program the OOB data */
	chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);

	status = chip->waitfunc(mtd, chip);

	return status & NAND_STATUS_FAIL ? -EIO : 0;
}

static int s3c_nand_read_oob_1bit(struct mtd_info *mtd, struct nand_chip *chip,
                                  int page, int sndcmd)
{
	uint8_t *ecc_calc = chip->buffers->ecccalc;
	int eccbytes = chip->ecc.bytes;
	int secc_start = mtd->oobsize - eccbytes;

	if (sndcmd) {
		chip->cmdfunc(mtd, NAND_CMD_READOOB, 0, page);
		sndcmd = 0;
	}

	chip->ecc.hwctl(mtd, NAND_ECC_READ);
	chip->read_buf(mtd, chip->oob_poi, secc_start);
	chip->ecc.calculate(mtd, 0, &ecc_calc[chip->ecc.total]);
	chip->read_buf(mtd, chip->oob_poi + secc_start, eccbytes);

	/* jffs2 special case */
	if (!(chip->oob_poi[2] == 0x85 && chip->oob_poi[3] == 0x19))
		chip->ecc.correct(mtd, chip->oob_poi, chip->oob_poi + secc_start, 0);

	return sndcmd;
}

static void s3c_nand_write_page_1bit(struct mtd_info *mtd, struct nand_chip *chip,
                                     const uint8_t *buf)
{
	int i, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	int secc_start = mtd->oobsize - eccbytes;
	uint8_t *ecc_calc = chip->buffers->ecccalc;
	const uint8_t *p = buf;

	uint32_t *eccpos = chip->ecc.layout->eccpos;

	/* main area */
	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		chip->ecc.hwctl(mtd, NAND_ECC_WRITE);
		chip->write_buf(mtd, p, eccsize);
		chip->ecc.calculate(mtd, p, &ecc_calc[i]);
	}

	for (i = 0; i < chip->ecc.total; i++)
		chip->oob_poi[eccpos[i]] = ecc_calc[i];

	/* spare area */
	chip->ecc.hwctl(mtd, NAND_ECC_WRITE);
	chip->write_buf(mtd, chip->oob_poi, secc_start);
	chip->ecc.calculate(mtd, p, &ecc_calc[chip->ecc.total]);

	for (i = 0; i < eccbytes; i++)
		chip->oob_poi[secc_start + i] = ecc_calc[chip->ecc.total + i];

	chip->write_buf(mtd, chip->oob_poi + secc_start, eccbytes);
}

static int s3c_nand_read_page_1bit(struct mtd_info *mtd, struct nand_chip *chip,
                                   uint8_t *buf)
{
	int i, stat, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	int secc_start = mtd->oobsize - eccbytes;
	int col = 0;
	uint8_t *p = buf;
	uint32_t *mecc_pos = chip->ecc.layout->eccpos;
	uint8_t *ecc_calc = chip->buffers->ecccalc;

	col = mtd->writesize;
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, col, -1);

	/* spare area */
	chip->ecc.hwctl(mtd, NAND_ECC_READ);
	chip->read_buf(mtd, chip->oob_poi, secc_start);
	chip->ecc.calculate(mtd, p, &ecc_calc[chip->ecc.total]);
	chip->read_buf(mtd, chip->oob_poi + secc_start, eccbytes);

	/* jffs2 special case */
	if (!(chip->oob_poi[2] == 0x85 && chip->oob_poi[3] == 0x19))
		chip->ecc.correct(mtd, chip->oob_poi, chip->oob_poi + secc_start, 0);

	col = 0;

	/* main area */
	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		chip->cmdfunc(mtd, NAND_CMD_RNDOUT, col, -1);
		chip->ecc.hwctl(mtd, NAND_ECC_READ);
		chip->read_buf(mtd, p, eccsize);
		chip->ecc.calculate(mtd, p, &ecc_calc[i]);

		stat = chip->ecc.correct(mtd, p, chip->oob_poi + mecc_pos[0] + ((chip->ecc.steps - eccsteps) * eccbytes), 0);
		if (stat == -1)
		mtd->ecc_stats.failed++;

		col = eccsize * (chip->ecc.steps + 1 - eccsteps);
	}

	return 0;
}

/* 
 * Hardware specific page read function for MLC.
 * Written by jsgood
 */
static int s3c_nand_read_page_4bit(struct mtd_info *mtd, struct nand_chip *chip,
                                   uint8_t *buf)
{
	int i, stat, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	int col = 0;
	uint8_t *p = buf;
	uint32_t *mecc_pos = chip->ecc.layout->eccpos;

	/* Step1: read whole oob */
	col = mtd->writesize;
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, col, -1);
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);

	col = 0;
	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		chip->cmdfunc(mtd, NAND_CMD_RNDOUT, col, -1);
		chip->ecc.hwctl(mtd, NAND_ECC_READ);
		chip->read_buf(mtd, p, eccsize);
		chip->write_buf(mtd, chip->oob_poi + mecc_pos[0] + ((chip->ecc.steps - eccsteps) * eccbytes), eccbytes);
		chip->ecc.calculate(mtd, 0, 0);
		stat = chip->ecc.correct(mtd, p, 0, 0);

		if (stat == -1)
		  	mtd->ecc_stats.failed++;

		col = eccsize * (chip->ecc.steps + 1 - eccsteps);
	}

	return 0;
}

/* 
 * Hardware specific page write function for MLC.
 * Written by jsgood
 */
static void s3c_nand_write_page_4bit(struct mtd_info *mtd, struct nand_chip *chip,
                                     const uint8_t *buf)
{
	int i, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	const uint8_t *p = buf;
	uint8_t *ecc_calc = chip->buffers->ecccalc;
	uint32_t *mecc_pos = chip->ecc.layout->eccpos;

	/* Step1: write main data and encode mecc */
	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		chip->ecc.hwctl(mtd, NAND_ECC_WRITE);
		chip->write_buf(mtd, p, eccsize);
		chip->ecc.calculate(mtd, p, &ecc_calc[i]);
	}

	/* Step2: save encoded mecc */
	for (i = 0; i < chip->ecc.total; i++)
		chip->oob_poi[mecc_pos[i]] = ecc_calc[i];

	chip->write_buf(mtd, chip->oob_poi, mtd->oobsize);
}

int s3c_nand_calculate_ecc_8bit(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
	u_long nfcont, nfm8ecc0, nfm8ecc1, nfm8ecc2, nfm8ecc3;

	/* Lock */
	nfcont = readl(NFCONT);
	nfcont |= NFCONT_MECCLOCK;
	writel(nfcont, NFCONT);

	if (cur_ecc_mode == NAND_ECC_READ)
		s3c_nand_wait_dec();
	else {
		s3c_nand_wait_enc();

		nfm8ecc0 = readl(NFM8ECC0);
		nfm8ecc1 = readl(NFM8ECC1);
		nfm8ecc2 = readl(NFM8ECC2);
		nfm8ecc3 = readl(NFM8ECC3);

		ecc_code[0] = nfm8ecc0 & 0xff;
		ecc_code[1] = (nfm8ecc0 >> 8) & 0xff;
		ecc_code[2] = (nfm8ecc0 >> 16) & 0xff;
		ecc_code[3] = (nfm8ecc0 >> 24) & 0xff;
		ecc_code[4] = nfm8ecc1 & 0xff;
		ecc_code[5] = (nfm8ecc1 >> 8) & 0xff;
		ecc_code[6] = (nfm8ecc1 >> 16) & 0xff;
		ecc_code[7] = (nfm8ecc1 >> 24) & 0xff;
		ecc_code[8] = nfm8ecc2 & 0xff;
		ecc_code[9] = (nfm8ecc2 >> 8) & 0xff;
		ecc_code[10] = (nfm8ecc2 >> 16) & 0xff;
		ecc_code[11] = (nfm8ecc2 >> 24) & 0xff;
		ecc_code[12] = nfm8ecc3 & 0xff;
	}

	return 0;
}



void s3c_nand_enable_hwecc_8bit(struct mtd_info *mtd, int mode)
{
	u_long nfcont, nfconf;

	cur_ecc_mode = mode;

	/* 8 bit selection */
	nfconf = readl(NFCONF);

	nfconf &= ~(0x3 << 23);
	nfconf |= (0x1 << 23);

	writel(nfconf, NFCONF);

	/* Initialize & unlock */
	nfcont = readl(NFCONT);
	nfcont |= NFCONT_INITECC;
	nfcont &= ~NFCONT_MECCLOCK;

	if (mode == NAND_ECC_WRITE)
		nfcont |= NFCONT_ECC_ENC;
	else if (mode == NAND_ECC_READ)
		nfcont &= ~NFCONT_ECC_ENC;

	writel(nfcont, NFCONT);
}



int s3c_nand_correct_data_8bit(struct mtd_info *mtd, u_char *dat, u_char *read_ecc, u_char *calc_ecc)
{
	int ret = -1;
	u_long nf8eccerr0, nf8eccerr1, nf8eccerr2, nfmlc8bitpt0, nfmlc8bitpt1;
	u_char err_type;

	s3c_nand_wait_ecc_busy_8bit();

	nf8eccerr0 = readl(NF8ECCERR0);
	nf8eccerr1 = readl(NF8ECCERR1);
	nf8eccerr2 = readl(NF8ECCERR2);
	nfmlc8bitpt0 = readl(NFMLC8BITPT0);
	nfmlc8bitpt1 = readl(NFMLC8BITPT1);

	err_type = (nf8eccerr0 >> 25) & 0xf;

	/* No error, If free page (all 0xff) */
	if ((nf8eccerr0 >> 29) & 0x1)
		err_type = 0;

	switch (err_type) {
		case 12: /* Uncorrectable */
			printf("s3c-nand: 12 bit(s) error detected ECC uncorrectable error detected\n");
			ret = -1;
			break;
		case 11: /* Uncorrectable */
			printf("s3c-nand: 11 bit(s) error detected ECC uncorrectable error detected\n");
			ret = -1;
			break;
		case 10: /* Uncorrectable */
			printf("s3c-nand: 10 bit(s) error detected ECC uncorrectable error detected\n");
			ret = -1;
			break;
		case 9: /* Uncorrectable */
			printf("s3c-nand: 9 bit(s) error detected  ECC uncorrectable error detected\n");
			ret = -1;
			break;

		case 8: /* 8 bit error (Correctable) */
			printf("s3c-nand: %d bit(s) error detected, corrected successfully\n", err_type);
			dat[(nf8eccerr2 >> 22) & 0x3ff] ^= ((nfmlc8bitpt1 >> 24) & 0xff);

		case 7: /* 7 bit error (Correctable) */
			printf("s3c-nand: %d bit(s) error detected, corrected successfully\n", err_type);
			dat[(nf8eccerr2 >> 11) & 0x3ff] ^= ((nfmlc8bitpt1 >> 16) & 0xff);

		case 6: /* 6 bit error (Correctable) */
			printf("s3c-nand: %d bit(s) error detected, corrected successfully\n", err_type);
			dat[nf8eccerr2 & 0x3ff] ^= ((nfmlc8bitpt1 >> 8) & 0xff);

		case 5: /* 5 bit error (Correctable) */
			printf("s3c-nand: %d bit(s) error detected, corrected successfully\n", err_type);
			dat[(nf8eccerr1 >> 22) & 0x3ff] ^= (nfmlc8bitpt1 & 0xff);

		case 4: /* 4 bit error (Correctable) */
			printf("s3c-nand: %d bit(s) error detected, corrected successfully\n", err_type);
			dat[(nf8eccerr1 >> 11) & 0x3ff] ^= ((nfmlc8bitpt0 >> 24) & 0xff);

		case 3: /* 3 bit error (Correctable) */

			printf("s3c-nand: %d bit(s) error detected, corrected successfully\n", err_type);
			dat[nf8eccerr1 & 0x3ff] ^= ((nfmlc8bitpt0 >> 16) & 0xff);

		case 2: /* 2 bit error (Correctable) */
			printf("s3c-nand: %d bit(s) error detected, corrected successfully\n", err_type);

			dat[(nf8eccerr0 >> 15) & 0x3ff] ^= ((nfmlc8bitpt0 >> 8) & 0xff);

		case 1: /* 1 bit error (Correctable) */
			printf("s3c-nand: %d bit(s) error detected, corrected successfully\n", err_type);
			dat[nf8eccerr0 & 0x3ff] ^= (nfmlc8bitpt0 & 0xff);
			ret = err_type;
			break;

		case 0: /* No error */
			ret = 0;
			break;
	}

	return ret;
}

void s3c_nand_write_page_8bit(struct mtd_info *mtd, struct nand_chip *chip,
                              const uint8_t *buf)
{
	int i, eccsize = 512;
	int eccbytes = 13;
	int eccsteps = mtd->writesize / eccsize;
	uint8_t *ecc_calc = chip->buffers->ecccalc;
	uint8_t *p = buf;
	uint32_t *mecc_pos = chip->ecc.layout->eccpos;

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		chip->ecc.hwctl(mtd, NAND_ECC_WRITE);
		chip->write_buf(mtd, p, eccsize);
		chip->ecc.calculate(mtd, p, &ecc_calc[i]);
	}

	for (i = 0; i < eccbytes * (mtd->writesize / eccsize); i++)
		chip->oob_poi[mecc_pos[i]] = ecc_calc[i];

	chip->write_buf(mtd, chip->oob_poi, mtd->oobsize);
}


int s3c_nand_read_page_8bit(struct mtd_info *mtd, struct nand_chip *chip,
                            uint8_t *buf)
{
	int i, stat, eccsize = 512;
	int eccbytes = 13;
	int eccsteps = mtd->writesize / eccsize;
	int col = 0;
	uint8_t *p = buf;
	uint32_t *mecc_pos = chip->ecc.layout->eccpos;

	/* Step1: read whole oob */
	col = mtd->writesize;
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, col, -1);
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);

	col = 0;
	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		chip->cmdfunc(mtd, NAND_CMD_RNDOUT, col, -1);
		//s3c_nand_enable_hwecc_8bit(mtd, NAND_ECC_READ);
		chip->ecc.hwctl(mtd, NAND_ECC_READ);
		chip->read_buf(mtd, p, eccsize);
		chip->write_buf(mtd, chip->oob_poi + mecc_pos[0] + ((chip->ecc.steps - eccsteps) * eccbytes), eccbytes);

		//s3c_nand_calculate_ecc_8bit(mtd, 0, 0);
		//stat = s3c_nand_correct_data_8bit(mtd, p, 0, 0);
		chip->ecc.calculate(mtd, 0, 0);
		stat = chip->ecc.correct(mtd, p, 0, 0);

		if (stat == -1)
			mtd->ecc_stats.failed++;

		col = eccsize * ((mtd->writesize / eccsize) + 1 - eccsteps);
	}

	return 0;
}
#endif

static bool find_full_id_nand(struct mtd_info *mtd, struct nand_chip *chip,
		   struct nand_flash_dev *type, u8 *id_data, int *busw)
{
	if (!strncmp(type->id, id_data, type->id_len)) {
		mtd->writesize = type->pagesize;
		mtd->erasesize = type->erasesize;
		mtd->oobsize = type->oobsize;

		chip->bits_per_cell = id_data[2];
		chip->chipsize = (uint64_t)type->chipsize << 20;
		chip->options |= type->options;
		chip->ecc_strength_ds = NAND_ECC_STRENGTH(type);
		chip->ecc_step_ds = NAND_ECC_STEP(type);

		*busw = type->options & NAND_BUSWIDTH_16;

		return true;
	}
	return false;
}

/* s3c_nand_probe
 *
 * called by device layer when it finds a device matching
 * one our driver can handled. This code checks to see if
 * it can allocate all necessary resources then calls the
 * nand layer to look for devices
 */
int board_nand_init(struct nand_chip *nand)
{
#if defined(CONFIG_MTD_NAND_HWECC)
	struct nand_flash_dev *type = NULL;
	u_char tmp;
	u_char dev_id;
	int i;
#endif
		
	NFCONT_REG		&= ~NFCONT_WP;
	nand->IO_ADDR_R		=  (void __iomem *)(NFDATA);
	nand->IO_ADDR_W		=  (void __iomem *)(NFDATA);
	nand->cmd_ctrl		= s3c_nand_hwcontrol;
	nand->dev_ready		= s3c_nand_device_ready;
	nand->scan_bbt		= s3c_nand_scan_bbt;
	nand->options		= 0;

#if defined(CONFIG_MTD_NAND_S3C_CACHEDPROG)
        nand->options		|= NAND_CACHEPRG;
#endif

#if defined(CONFIG_MTD_NAND_HWECC)
        nand->ecc.mode		= NAND_ECC_HW;
        nand->ecc.hwctl		= s3c_nand_enable_hwecc;
        nand->ecc.calculate	= s3c_nand_calculate_ecc;
        nand->ecc.correct	= s3c_nand_correct_data;


        // K9GAG08U0E must add below codes
        {
            s3c_nand_hwcontrol(0, NAND_CMD_RESET, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
            s3c_nand_device_wait_ready(0);

        }

        s3c_nand_hwcontrol(0, NAND_CMD_READID, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
        s3c_nand_hwcontrol(0, 0x00, NAND_CTRL_CHANGE | NAND_NCE | NAND_ALE);
        s3c_nand_hwcontrol(0, 0x00, NAND_NCE | NAND_ALE);
        s3c_nand_hwcontrol(0, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);
        //s3c_nand_device_ready(0);

        s3c_nand_device_wait_ready(0);

         u32 manuf=tmp = readb(nand->IO_ADDR_R); /* Maf. ID */
       

        dev_id = tmp = readb(nand->IO_ADDR_R); /* Device ID */

       // printk("forlinx nandflash dev_id=%x\n",dev_id);

        /*for (j = 0; nand_flash_ids[j].name != NULL; j++) {
            if (tmp == nand_flash_ids[j].dev_id) {
                type = &nand_flash_ids[j];
                break;
            }
        }*/
        u8 id_data[8];
	int my_busw;
        s3c_nand_hwcontrol(0, NAND_CMD_READID, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
        s3c_nand_hwcontrol(0, 0x00, NAND_CTRL_CHANGE | NAND_NCE | NAND_ALE);
        s3c_nand_hwcontrol(0, 0x00, NAND_NCE | NAND_ALE);
        s3c_nand_hwcontrol(0, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);
	
	for (i = 0; i < 8; i++)
		id_data[i] = readb(nand->IO_ADDR_R);

	if (!type)
		type = nand_flash_ids;

	my_busw = nand->options & NAND_BUSWIDTH_16;
	for (; type->name != NULL; type++) {
		if (type->id_len) {
			if (find_full_id_nand(s3c_mtd, nand, type, id_data, &my_busw))
			{
			//	pr_info("1. Nand name: %s, dev_di : %x\n", type->name, type->dev_id);
				
				break;
			}
		} else if (tmp == type->dev_id) {
			//pr_info("2. Nand name: %s, dev_di : %x\n", type->name, type->dev_id);
			//type = &nand_flash_ids[j];
				break;
		}
	}

        if (!type) {
            printf("Unknown NAND Device.\n");
            return 1;
        }
	//pr_info("nand pagesize : %d\n", type->pagesize);
	//pr_info("nand name : %s\n", type->name);

        nand->bits_per_cell = readb(nand->IO_ADDR_R);	/* the 3rd byte */
        tmp = readb(nand->IO_ADDR_R);			/* the 4th byte */
        if (!type->pagesize)
        {
            if (((nand->bits_per_cell >> 2) & 0x3) == 0)
            {

                nand_type = S3C_NAND_TYPE_SLC;
                nand->ecc.size = 512;
                nand->ecc.bytes	= 4;

                if ((1024 << (tmp & 0x3)) > 512)
                {
                    nand->ecc.read_page = s3c_nand_read_page_1bit;
                    nand->ecc.write_page = s3c_nand_write_page_1bit;
                    nand->ecc.read_oob = s3c_nand_read_oob_1bit;
                    nand->ecc.write_oob = s3c_nand_write_oob_1bit;
                    nand->ecc.layout = &s3c_nand_oob_64;

                    printf("forlinx********Nandflash:Type=SLC  ChipName:samsung-K9F2G08U0B or hynix-HY27UF082G2B****** \n");


                } else
                {
                    nand->ecc.layout = &s3c_nand_oob_16;
                }

            } else
            {
                // int childType=tmp & 0x03; //Page size
                int childType = nand->bits_per_cell;

                if(dev_id == 0xd5)
                {

                    nand_type = S3C_NAND_TYPE_MLC_8BIT;
                    nand->ecc.read_page = s3c_nand_read_page_8bit;
                    nand->ecc.write_page = s3c_nand_write_page_8bit;
                    nand->ecc.size = 512;
                    nand->ecc.bytes = 13;

                    if(childType==0x94)
                    {
                        //K9GAG08U0D     size=2GB  type=MLC  Page=4K
                        nand->ecc.layout = &s3c_nand_oob_mlc_128_8bit;
                        printf("forlinx****Nandflash:ChipType= MLC  ChipName=samsung-K9GAG08U0D************ \n");
                    }
                    else if(childType==0x14)
                    {
                        //K9GAG08U0M     size=2GB  type=MLC  Page=4K
                        nand->ecc.layout = &s3c_nand_oob_mlc_128_8bit;
                        printf("forlinx****Nandflash:ChipType= MLC  ChipName=samsung-K9GAG08U0M************ \n");
                    }
                    else if(childType==0x84)
                    {
                        //K9GAG08U0E     size=2GB  type=MLC  Page=8K
                        nand->ecc.layout = &s3c_nand_oob_mlc_232_8bit;
                        printf("forlinx****Nandflash:ChipType= MLC  ChipName=samsung-K9GAG08U0E************ \n");

                    }


                }else if(dev_id == 0xd7)
                {
                    nand_type = S3C_NAND_TYPE_MLC_8BIT;
                    nand->ecc.read_page = s3c_nand_read_page_8bit;
                    nand->ecc.write_page = s3c_nand_write_page_8bit;
                    nand->ecc.size = 512;
                    nand->ecc.bytes = 13;
		    nand->ecc.strength = 4;
                    nand->ecc.layout = &s3c_nand_oob_mlc_128_8bit;
                    printf("forlinx****Nandflash:ChipType= MLC  ChipName=samsung-K9LBG08U0D************ \n");


                }
                else if(dev_id == 0xd3)
                {
                    nand_type = S3C_NAND_TYPE_MLC_4BIT;
                    nand->options |= NAND_NO_SUBPAGE_WRITE;	/* NOP = 1 if MLC */
                    nand->ecc.read_page = s3c_nand_read_page_4bit;
                    nand->ecc.write_page = s3c_nand_write_page_4bit;
                    nand->ecc.size = 512;
                    nand->ecc.bytes = 8;	/* really 7 bytes */
                    nand->ecc.layout = &s3c_nand_oob_mlc_64;

                    printf("forlinx****Nandflash:ChipType=MLC  ChipName=samsung-K9G8G08U0A************** \n");

                }

            }

        } else  if(dev_id == 0xd7)
        {
            nand_type = S3C_NAND_TYPE_MLC_8BIT;
            nand->ecc.read_page = s3c_nand_read_page_8bit;
            nand->ecc.write_page = s3c_nand_write_page_8bit;
            nand->ecc.size = 512;
            nand->ecc.bytes = 13;
            nand->ecc.layout = &s3c_nand_oob_mlc_128_8bit;
            printf("forlinx****Nandflash:ChipType= MLC  ChipName=samsung-K9LBG08U0D************ \n");

        }
        else if(manuf==0x2c  && dev_id ==0x48)
        {
            //MT29F16G08ABACAWP      size=2GB  type=SLC  Page=4K
            nand_type = S3C_NAND_TYPE_MLC_8BIT;
            nand->ecc.read_page = s3c_nand_read_page_8bit;
            nand->ecc.write_page = s3c_nand_write_page_8bit;
            nand->ecc.size = 512;
            nand->ecc.bytes = 13;
            nand->ecc.layout = &s3c_nand_oob_mlc_128_8bit;

            printf("forlinx******Nandflash:ChipType= SLC  ChipName=MT29F16G08ABACAWP\n");

        } else if(manuf==0x2c  && dev_id ==0x38)
        {
                //MT29F16G08ABACAWP      size=1GB  type=SLC  Page=4K
                nand_type = S3C_NAND_TYPE_MLC_8BIT;
                nand->ecc.read_page = s3c_nand_read_page_8bit;
                nand->ecc.write_page = s3c_nand_write_page_8bit;
                nand->ecc.size = 512;
                nand->ecc.bytes = 13;
                nand->ecc.layout = &s3c_nand_oob_mlc_128_8bit;

                printf("forlinx******Nandflash:ChipType= SLC  ChipName=MT29F8G08ABABAWP\n");

        }else
        {
            nand_type = S3C_NAND_TYPE_SLC;
            nand->ecc.size = 512;
            nand->bits_per_cell = 0;
            nand->ecc.bytes = 4;
            nand->ecc.layout = &s3c_nand_oob_16;
            printf("forlinx *****Nandflash:ChipType= Unknow\n");

        }

        printf("S3C NAND Driver is using hardware ECC.\n");
#else
        nand->ecc.mode = NAND_ECC_SOFT;
        printf("S3C NAND Driver is using software ECC.\n");
#endif

    return 0;
}
#endif
