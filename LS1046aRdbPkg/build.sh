#!/bin/bash

# UEFI build script for LS1046A SoC from Freescale
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

# Global Defaults
ARCH=AARCH64
TARGET_TOOLS=`echo $GCC_ARCH_PREFIX | cut -d _ -f 1`

[ -z "$TARGET_TOOLS" ] && {
  echo "TARGET_TOOLS not found. Please run \"source ls1046a_env.cshrc\" ."
  exit 1
}

BOOT_VIA_QSPI_FLASH="FALSE"
print_usage_banner()
{
	echo "This shell script expects:"
	echo "	Arg 1 (mandatory): Build candidate (can be RELEASE or DEBUG). By
		default we build the RELEASE candidate."
	echo "	Arg 2 (mandatory): Boot source type (can be XIP, QSPI, SD or NAND. By default
		we build the XIP candidate. SD and NAND is used for
		NONXIP boot sources like SD and NAND boot currently
		QSPI - To indicate that the booting source is QSPI flash."
	echo "	Arg 3 (mandatory for SD or NAND boot): absolute path of mkimage utility."
	echo "	Arg 3 (optional): clean - To do a 'make clean' operation."
}

# Actual stuff starts from here
echo ".........................................."
echo "Welcome to LS1046A UEFI Build environment"
echo ".........................................."

# Check for input arguments
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

# Check for input arguments
if [[ $1 != "RELEASE" ]]; then
	if [[ $1 != "DEBUG" ]]; then
		echo "Error ! Incorrect build target specified."
		print_usage_banner
		exit
	fi
fi

if [[ $2 == "NAND" || $2 == "SD" ]]; then
	BootSuffix="NonXipBoot.dsc"
	echo "Compiling for $1 boot"
	if [[ $3 == "" ]]; then
		echo "Error ! Incorrect 3rd argument to build script."
		print_usage_banner
		exit
	fi

	if [[ $3 != "clean" ]]; then
		echo "Using mkimage utility is available at this path: $3"
		echo "(Hint: Usually this is the absolute path of the u-boot/tools directory)"
	fi
else
	if [[ $2 == "XIP" ]]; then
			BootSuffix="FatXipBoot.dsc"
			echo "Compiling for booting via NOR flash."
	else
		if [[ $2 == "QSPI" ]]; then
			BootSuffix="FatXipBoot.dsc"
			BOOT_VIA_QSPI_FLASH="TRUE"
			echo "Compiling for booting via QSPI flash."
		else
			echo "Bad boot type argument. Use NONXIP, QSPI or XIP"
			print_usage_banner
			exit
		fi
	fi
fi

if [[ $3 == "clean" ]]; then
	echo "Cleaning up the build directory '../Build/LS1046aRdb/'.."
	rm -rf ../Build/LS1046aRdb/*
	exit
else
	if [[ $3 == "" ]]; then
		# Do nothing as argument 3 is optional.
		echo " "
	else
		if [[ $2 != "NAND" && $2 != "SD" ]]; then
			echo "Error ! Incorrect 3rd argument to build script."
			print_usage_banner
			exit
		fi
	fi
fi

# Clean-up
set -e
shopt -s nocasematch

#
# Setup workspace now
#
echo Initializing workspace
cd ..

# Use the BaseTools in edk2
export EDK_TOOLS_PATH=`pwd`/BaseTools
source edksetup.sh BaseTools

# Actual build command
if [[ $2 == "NAND" ]]; then
build -p "$WORKSPACE/LS1046aRdbPkg/LS1046aRdbPkg$BootSuffix" -a $ARCH -t $TARGET_TOOLS -b $1 -DNAND_BOOT_ENABLE=TRUE
elif [[ $2 == "SD" ]]; then
build -p "$WORKSPACE/LS1046aRdbPkg/LS1046aRdbPkg$BootSuffix" -a $ARCH -t $TARGET_TOOLS -b $1 -DSD_BOOT_ENABLE=TRUE
else
build -p "$WORKSPACE/LS1046aRdbPkg/LS1046aRdbPkg$BootSuffix" -a $ARCH -t $TARGET_TOOLS -b $1 -DBOOT_VIA_QSPI_FLASH=$BOOT_VIA_QSPI_FLASH
fi

if [[ $2 == "XIP" || $2 == "QSPI" ]]; then
	cat $WORKSPACE/Build/LS1046aRdb/$1"_"$TARGET_TOOLS/FV/LS1046ARDB_EFI.fd >> $WORKSPACE/Build/LS1046aRdb/$1"_"$TARGET_TOOLS/FV/LS1046ARDBPI_EFI.fd
	mv $WORKSPACE/Build/LS1046aRdb/$1"_"$TARGET_TOOLS/FV/LS1046ARDBPI_EFI.fd $WORKSPACE/Build/LS1046aRdb/$1"_"$TARGET_TOOLS/FV/LS1046ARDB_EFI.fd
fi

if [[ $2 == "NAND" ]]; then
$3/mkimage -n $WORKSPACE/LS1046aRdbPkg/Library/LS1046aPrePiOcram/ls1046ardb_rcw_nand.cfg -R $WORKSPACE/LS1046aRdbPkg/Library/LS1046aPrePiOcram/ls1046ardb_pbi.cfg -T pblimage -A arm -a 0x10000000 -d $WORKSPACE/Build/LS1046aRdb/$1"_"$TARGET_TOOLS/FV/LS1046ARDBPI_EFI.fd $WORKSPACE/LS1046aRdbPkg/Library/LS1046aPrePiOcram/LS1046ARDBPI_NAND_EFI.pbl
echo "PBL image created at $WORKSPACE/LS1046aRdbPkg/Library/LS1046aPrePiOcram/LS1046ARDBPI_NAND_EFI.pbl"
elif [[ $2 == "SD" ]]; then
$3/mkimage -n $WORKSPACE/LS1046aRdbPkg/Library/LS1046aPrePiOcram/ls1046ardb_rcw_sd.cfg -R $WORKSPACE/LS1046aRdbPkg/Library/LS1046aPrePiOcram/ls1046ardb_pbi.cfg -T pblimage -A arm -a 0x10000000 -d $WORKSPACE/Build/LS1046aRdb/$1"_"$TARGET_TOOLS/FV/LS1046ARDBPI_EFI.fd $WORKSPACE/LS1046aRdbPkg/Library/LS1046aPrePiOcram/LS1046ARDBPI_SD_EFI.pbl
echo "PBL image created at $WORKSPACE/LS1046aRdbPkg/Library/LS1046aPrePiOcram/LS1046ARDBPI_SD_EFI.pbl"
fi
