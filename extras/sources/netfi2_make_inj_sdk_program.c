/*****************************************************************************/
/**
*
* @file		SET.c
*
* This file contains a design to make injections on the BM Slice, based on LUT456
* architecture.
*
* The example uses the next setting in the Uartlite driver:
*	. baud rate 115200
*	. 8 bit data
*	. 1 stop bit
*	. no parity
*
* @note
*
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- -------------------------------------------------
* 1.00  Dardo  27/06/17 First Release
******************************************************************************/

/***************************** Include Files *********************************/
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xtmrctr.h"
#include "float.h"
#include "Netfi2_lib.h"
/************************** Constant Definitions *****************************/

#define	AddrAXI4Memory		XPAR_M01_AXI_BASEADDR
#define SLV_REG_INJ_NUM		26
#define NUM_INJ				812
#define NUM_INPUT			8192
#define LAST_REG_SIZE		12

/**************************** Type Definitions *******************************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

int  *baseaddr_p = (int *)AddrAXI4Memory;/* The instance of the UART Driver */

/*****************************************************************************
* Main function
*
* @param	None
*
* @return
*		- XST_FAILURE if the Test Failed .
*		- A non-negative number indicating the number of characters
*		  sent.
* @note		None
******************************************************************************/
int main()
{
    init_platform();

//	Booleans for the different program tasks
    int campaign = TRUE;
    int make_inj,print_PM,print_results;

    int option = 2; //select task to perform

    if(option == 1) //No-inj and print PM
    {
    	make_inj = FALSE;
    	print_PM = TRUE;
    	print_results = FALSE;
    }else if (option == 2) { //Make injections and print PM
		make_inj = TRUE;
		print_PM = TRUE;
		print_results = FALSE;
	}else if (option == 3) { //Make injections and calculate results
		make_inj = TRUE;
		print_PM = FALSE;
		print_results = TRUE;
	}

    if(print_results)
	{
    	printf("-------- NETFI2 - SET INJECTION TOOL ---------\n\n");
        printf("Platform initiated...\n");
	}

//	Time consuming task
    int c = 1, d = 1, i=0;
	for ( c = 1 ; c <= 100000 ; c++ )
	for ( d = 1 ; d <= 100 ; d++ )
	{}
// 	Printing with thousands separating point for easy visualization of values
	int numberSeperated[8];
	if(make_inj)
	{
		if(print_results)
		{
			printf("Total number of injections = " );
			getNumWcommas((NUM_INPUT*NUM_INJ), numberSeperated );
		}
	}

//	Assign injection's addresses and sets them to 0
	int *inj[SLV_REG_INJ_NUM];
	for (i = 0; i < SLV_REG_INJ_NUM; ++i)
	{
		inj[i]= (baseaddr_p+i);
		*inj[i] = 0;
		//printf("inj_addr[%d] = %#x --- value = %#x\n", i, inj[i], *inj[i]);
	}


//	Assign input_z's and PM's addresses and sets input_z to 0
	int *reset;
	reset =(baseaddr_p+28) ;
	int *input_z;
	input_z =(baseaddr_p+29) ;
	int * PM = (baseaddr_p+30) ;
	*input_z = 0;

	int to_finish = 0;
	int m,l= 0,j= 0;

	int inj_value;		  //identifies the injection position on a particular register
	int slv_reg_num=0; 	  //identifies the particular slv_reg where the injection is being made
	int inj_num = 0; 	  //identifies the injection ID over the total of NUM_INJ
	double err_count = 0; //amount of errors per input
	double err_total = 0; //total amount of errors
	int org_PM_temp =0;   //identifies the value of PM without injections on a particular input

	*input_z = 0;

	if(print_results)
	{
		if(make_inj)
		{
			printf("\nInjections campaign started...\n\n");
		}
	}
//	Injection campaign. For every input, every possible injection available
	if(campaign)
	{
		if(print_results)
		xil_printf("             ");
//	For every input, makes an injection on all the different positions, and gets the PM
		for ( i = 0 ; i < NUM_INPUT ; i++)
		{
		//	Print results or print the PM to save on a file?
			if(print_results)
			{
				to_finish++;
				if(to_finish == (NUM_INPUT/10))
				{
					xil_printf("° ");
					to_finish = 0;
				}
			}
		//	Sets the input to the next value of the pile

			for (m = 0; m < 27; ++m) {
				*inj[m] = 0;
			}
			*input_z = i;
			org_PM_temp = *PM;
		//	Makes the NUM_INJ amount of injections for the input selected of the pile
			for (slv_reg_num = 0; slv_reg_num < SLV_REG_INJ_NUM-1; slv_reg_num++)
			{
				inj_value=1;
				for ( j= 0 ; j < 32 ; j++ )
				{
					if(make_inj)
					{
						*inj[slv_reg_num] = inj_value;
					}else{
						*inj[slv_reg_num] = 0;
					}
					for (m = 0; m < 100; ++m){}
					if(*PM != org_PM_temp)
					{
						err_count = err_count + 1;
					}
					inj_num = inj_num +1;
					if(print_PM)
					{printf("%d\n",*PM);}
					inj_value= inj_value<<1;
				}
			}
			inj_value=1;
			for ( l= 0 ; l < LAST_REG_SIZE ; l++ )
			{
				if(make_inj)
				{
					*inj[slv_reg_num] = inj_value;
				}else{
					*inj[slv_reg_num] = 0;
				}
				if(*PM != org_PM_temp)
				{
					err_count = err_count + 1;
				}
				inj_num = inj_num + 1;

				if(print_PM)
				{printf("%d\n",*PM);}
				inj_value= inj_value<<1;
			}
			inj_num = 0;
			err_total = err_total + err_count;
			err_count = 0;
		}

		if(print_results)
		{
			if(make_inj)
			{
				printf("\n\n----------- Completed successfully -----------\n");
				printf("\nTotal number of errors = " );
				getNumWcommas(err_total, numberSeperated );
				printf("\n\nPorcentage of errors = %.2f%%\n", ((err_total/(NUM_INPUT*NUM_INJ))*100));
				printf("\nProgram terminated...\n");
			}
		}
	}
	cleanup_platform();
	return 0;
}
