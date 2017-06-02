#  @ccn504.S
#
#  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights
#  reserved.
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#

#include <AutoGen.h>

.text
.align 2

GCC_ASM_EXPORT(Ccn504AddMastersToDvmDomain)
GCC_ASM_EXPORT(Ccn504SetQosVal)
GCC_ASM_EXPORT(Ccn504SetAux)

/*************************************************************************
 *
 * void Ccn504AddMastersToDvmDomain(CCI_MN_BASE_ADDR, CCI_MN_RNF_NODEID_LIST_OFFSET,
 * 				  CCI_MN_DVM_DOMAIN_CTL_SET);
 *
 * This function adds fully-coherent masters to DVM domain
 *
 *************************************************************************/
ASM_PFX(Ccn504AddMastersToDvmDomain):
	/*
	 * x0: CCI_MN_BASE_ADDR
	 * x1: CCI_MN_RNF_NODEID_LIST_OFFSET
	 * x2: CCI_MN_DVM_DOMAIN_CTL_SET
	 */

	/* Adding fully-coherent masters to DVM domain here*/
	ldr	x9, [x0, x1]
	str	x9, [x0, x2]
2:	ldr	x10, [x0, x2]
	mvn	x11, x10
	tst	x11, x10
	b.ne	2b

	ret


/*************************************************************************
 *
 * void Ccn504SetQosVal(CCI_Sx_QOS_CONTROL_BASE_ADDR, QoS Value);
 *
 * This function Initializes QoS settings for AR/AW override.
 * It sets the same QoS value for all RN-I ports for now
 *
 *************************************************************************/
ASM_PFX(Ccn504SetQosVal):
	/*
	 * x0: CCI_Sx_QOS_CONTROL_BASE_ADDR
	 * x1: QoS Value
	 */

	/* Set all RN-I ports to QoS value stored in x1 */
	ldr	x9, [x0]
	mov	x10, x1
	orr	x9, x9, x10
	str	x9, [x0]

	ret

/*************************************************************************
 *
 * void ccn504_set_aux(CCI_AUX_CONTROL_BASE, Value);
 *
 * Initialize AUX control settings
 *
 *************************************************************************/
ASM_PFX(Ccn504SetAux):
       /*
        * x0: CCI_AUX_CONTROL_BASE
        * x1: Value
        */

       ldr    x9, [x0]
       mov    x10, x1
       orr    x9, x9, x10
       str    x9, [x0]

       ret


ASM_FUNCTION_REMOVE_IF_UNREFERENCED
