#!/bin/bash

# UEFI build script for LS2088A SoC from Freescale
#
# Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
#
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

# Defaults
ARCH=AARCH64
TARGET_TOOLS=`echo $GCC_ARCH_PREFIX | cut -d _ -f 1`

[ -z "$TARGET_TOOLS" ] && {
  echo "TARGET_TOOLS not found. Please run \"source ls1043a_env.cshrc\" ."
  exit 1
}

BOOT_VIA_QSPI_FLASH="FALSE"
print_usage_banner()
{
	echo "This shell script expects:"
	echo "	Arg 1 (mandatory): Build candidate
		(By default we build the RELEASE candidate)
		RELEASE - Provides reduced DEBUG information.
		DEBUG - Provides sufficient DEBUG information."
	echo "	Arg 2 (mandatory): Boot source type (can be XIP, QSPI). By default
		we build the XIP candidate.
		QSPI - To indicate that the booting source is QSPI flash."
	echo "	Arg 3 (optional):
		clean - To do a 'make clean' operation."
}

# Actual stuff starts from here
echo ".........................................."
echo "Welcome to LS2088A UEFI Build environment"
echo ".........................................."

# Check for input arguments:
#   We need atleast 1 input argument
if [[ $1 == "" ]]; then
	echo "Error ! No build target specified."
	print_usage_banner
	exit
fi
if [[ $2 == "" ]]; then
	echo "Error ! No boot source type specified."
	print_usage_banner
	exit
fi

# Check the 1st argument:
#   1st argument can be one of the following:
#   	- RELEASE: e.g. - ./build.sh RELEASE
#   	- DEBUG: e.g. - ./build.sh DEBUG
if [[ $1 != "RELEASE" ]]; then
	if [[ $1 != "DEBUG" ]]; then
		echo "Error ! Incorrect build target specified."
		print_usage_banner
		exit
	fi
fi

# Check the 2nd argument:
#   2nd argument can be one of the following:
#   	- clean: e.g. - ./build.sh RELEASE clean
#   	- QSPI: e.g. - ./build.sh RELEASE QSPI
#   	- XIP: e.g. - ./build.sh RELEASE XIP
#   	- earlyprintk: e.g. - ./build.sh RELEASE earlyprintk
#   	- no argument: e.g. - ./build.sh RELEASE
if [[ $2 == "QSPI" ]]; then
	echo "Compiling for booting via QSPI flash."
	BOOT_VIA_QSPI_FLASH="TRUE"
elif [[ $2 == "XIP" ]]; then
	echo "Compiling for booting via NOR flash."
	BOOT_VIA_QSPI_FLASH="FALSE"
else
	echo "Error ! Incorrect build arguments specified."
	print_usage_banner
	exit
fi

# Check the 3rd argument:
#   3rd argument can be one of the following:
#   	- clean: e.g. - ./build.sh RELEASE QSPI clean
#   		 e.g. - ./build.sh RELEASE XIP clean
if [[ $3 == "clean" ]]; then
	echo "Cleaning up the build directory '../Build/LS2088aRdb/'.."
	rm -rf ../Build/LS2088aRdb/*
	exit
else
	if [[ $3 == "" ]]; then
		# Do nothing as argument 3 is optional.
		echo " "
	else
		echo "Error ! Incorrect build arguments specified."
		print_usage_banner
		exit
	fi
fi

if [[ $4 != "" ]]; then
	echo "Error ! Incorrect build arguments specified."
	print_usage_banner
	exit
fi

#
# Setup workspace now
#
echo Initializing workspace
cd ..

# Use the BaseTools in edk2
export EDK_TOOLS_PATH=`pwd`/BaseTools
source edksetup.sh BaseTools

# Clean-up
set -e
shopt -s nocasematch
rm -rf $WORKSPACE/Build/LS2088aRdb/$1_$TARGET_TOOLS/$ARCH/LS2088aRdbPkg/Drivers/LanIntelE1000Dxe

# Actual build command
build -p "$WORKSPACE/LS2088aRdbPkg/LS2088aRdbPkg.dsc" -a $ARCH -t $TARGET_TOOLS -b $1 -DBOOT_VIA_QSPI_FLASH=$BOOT_VIA_QSPI_FLASH

if [[ $2 == "XIP" || $2 == "QSPI" ]]; then
	cat $WORKSPACE/Build/LS2088aRdb/$1_$TARGET_TOOLS/FV/LS2088ARDB_EFI.fd >> $WORKSPACE/Build/LS2088aRdb/$1_$TARGET_TOOLS/FV/LS2088ARDBPI_EFI.fd
	mv $WORKSPACE/Build/LS2088aRdb/$1_$TARGET_TOOLS/FV/LS2088ARDBPI_EFI.fd $WORKSPACE/Build/LS2088aRdb/$1_$TARGET_TOOLS/FV/LS2088ARDB_EFI.fd
fi

