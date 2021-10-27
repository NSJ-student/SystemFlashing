#!/bin/bash

BASE_PATH=/home/mik21/nvidia_old/nvidia_sdk/JetPack_4.4.1_Linux_JETSON_TX2/Linux_for_Tegra
LAST_CMD=$BASE_PATH/last_cmd
REMOTE_UPGRADE=RemoteUpgrade_2_1_1
HDMI_DISP_APP=DisplayPanel_Ver_2_0_6
DSI2_DISP_APP=DISP_2CH_CTRL_3_9_5
DSI4_DISP_APP=DISP_2CH_CTRL_8_9_3
DSI1_DISP_APP=DISP_2CH_CTRL_3_9_5

sudo cp ./work_obj/$REMOTE_UPGRADE ./rootfs/home/mik21/Desktop/RemoteUpgrade
PROJECT=$1

if   [ "$PROJECT" == "dsi_two" ]; then
	sudo ./kernel/dtc -I dts -O dtb -o temp.dtb ./work_obj/dsi_R32_2.dts
	sudo cp temp.dtb ./kernel/dtb/tegra186-quill-p3310-1000-c03-00-base.dtb
	sudo cp ./work_obj/GDE_DSI/$DSI2_DISP_APP ./rootfs/home/mik21/Desktop/DISP_CTRL
	echo "** set for K-6402GDE/K-6404GT/K-6402GP"
	
	shift
	
	if [ "$1" == "right" ]; then
		sudo cp ./work_obj/interfaces_193 ./rootfs/etc/network/interfaces
		echo "** set ip 192.168.20.193"
	elif [ "$1" == "left" ]; then
		sudo cp ./work_obj/interfaces_192 ./rootfs/etc/network/interfaces
		echo "** set ip 192.168.20.192"
	else
		echo "EE Invalid IP [ $1 ]"
		exit
	fi

elif [ "$PROJECT" == "dsi_four" ]; then
	sudo ./kernel/dtc -I dts -O dtb -o temp.dtb ./work_obj/dsi_R32_2.dts
	sudo cp temp.dtb ./kernel/dtb/tegra186-quill-p3310-1000-c03-00-base.dtb
	sudo cp ./work_obj/GDE_DSI/$DSI4_DISP_APP ./rootfs/home/mik21/Desktop/DISP_CTRL
	echo "** set for K-6408GT"

	shift
	
	if [ "$1" == "right" ]; then
		sudo cp ./work_obj/interfaces_193 ./rootfs/etc/network/interfaces
		echo "** set ip 192.168.20.193"
	elif [ "$1" == "left" ]; then
		sudo cp ./work_obj/interfaces_192 ./rootfs/etc/network/interfaces
		echo "** set ip 192.168.20.192"
	else
		echo "EE Invalid IP [ $1 ]"
		exit
	fi

elif [ "$PROJECT" == "dsi_one" ]; then
	sudo ./kernel/dtc -I dts -O dtb -o temp.dtb ./work_obj/1ch_dsi_R32_2.dts
	sudo cp temp.dtb ./kernel/dtb/tegra186-quill-p3310-1000-c03-00-base.dtb
	sudo cp ./work_obj/GDE_DSI/$DSI1_DISP_APP ./rootfs/home/mik21/Desktop/DISP_CTRL
	echo "** set for K-6401GDR"
	sudo cp ./work_obj/interfaces_192 ./rootfs/etc/network/interfaces
	echo "** set ip 192.168.20.192"

elif [ "$PROJECT" == "hdmi" ]; then
	sudo ./kernel/dtc -I dts -O dtb -o temp.dtb ./work_obj/hdmi_ct_R32_2.dts
	sudo cp temp.dtb ./kernel/dtb/tegra186-quill-p3310-1000-c03-00-base.dtb
	sudo cp ./work_obj/CT_HDMI/$HDMI_DISP_APP ./rootfs/home/mik21/Desktop/DISP_CTRL
	echo "** set for K-6408GT eLCV"
	sudo cp ./work_obj/interfaces_192 ./rootfs/etc/network/interfaces
	echo "** set ip 192.168.20.192"

else
	echo "EE Invalid Selection [ $PROJECT ]"
	exit
fi

# load last cmd
last_cmd='none';
if [ -f ${LAST_CMD} ]; then
	last_cmd=`cat ${LAST_CMD}`
fi

echo "last flashed image = $last_cmd"


if [ $# -eq 2 ]; then
	if   [ "$2" == "flash_dts" ]; then
		sudo ./flash.sh -r -k kernel-dtb jetson-tx2 mmcblk0p1
		echo "** flash_dts => done"

	elif [ "$2" == "flash_no_make_image" ]; then
		sudo ./flash.sh -r kernel-dtb jetson-tx2 mmcblk0p1
		echo "** flash without making image => done"

	elif [ "$2" == "flash_all" ]; then
		if [ $last_cmd == "$PROJECT"  ]; then
			echo "flash without make image"
			sudo ./flash.sh -r kernel-dtb jetson-tx2 mmcblk0p1
		else
			echo "flash make image"
			sudo ./flash.sh kernel-dtb jetson-tx2 mmcblk0p1

			# save last cmd
			echo "$PROJECT" > ${LAST_CMD}
		fi
		echo "** flash_all done"

	else
		echo "EE do not flash [ $2 ]"
	fi
else
	echo "** do not flash"
fi
