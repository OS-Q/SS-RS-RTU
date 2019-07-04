#!/bin/bash

export WorkPath=`pwd`

## Root Password
for ((i = 0; i < 5; i++)); do
	PASSWD=$(whiptail --title "freeCAD for ubuntu system" \
		--passwordbox "Enter root password. Don't use root or sudo run it" \
		10 60 3>&1 1>&2 2>&3)
	if [ $i = "4" ]; then
		whiptail --title "Note Qitas" --msgbox "Invalid password" 10 40 0	
		exit 0
	fi

	sudo -k
	if sudo -lS &> /dev/null << EOF
$PASSWD
EOF
	then
		i=10
	else
		whiptail --title "freeCAD Config System" --msgbox "Invalid password, Pls input corrent password" \
		10 40 0	--cancel-button Exit --ok-button Retry
	fi
done

echo $PASSWD | sudo ls &> /dev/null 2>&1


function apt_install()
{
	sudo apt-get -y build-dep freecad-daily
	sudo apt-get install -y freecad-daily
	sudo apt build-dep freecad -y 
	sudo add-apt-repository -y  --enable-source ppa:freecad-maintainers/freecad-daily && sudo apt-get update -y 
	sudo apt install -y libocct*-dev # For the official version
	sudo apt install -y liboce*-dev # For the community edition
	sudo apt install -y build-essential libtool cmake debhelper dh-exec dh-python libboost-date-time-dev libboost-dev libboost-filesystem-dev libboost-graph-dev 
	sudo apt install -y libboost-iostreams-dev libboost-program-options-dev libboost-python-dev libboost-regex-dev libboost-serialization-dev libboost-signals-dev 
	sudo apt install -y libboost-thread-dev libeigen3-dev libgts-bin libgts-dev libkdtree++-dev libmedc-dev libopencv-dev libproj-dev libqt5opengl5-dev libqt5svg5-dev 
	sudo apt install -y libqt5xmlpatterns5-dev libqt5x11extras5-dev libspnav-dev libvtk7-dev libx11-dev libxerces-c-dev libzipios++-dev lsb-release python3-matplotlib 
	sudo apt install -y python3-dev python3-ply qtbase5-dev qttools5-dev libqt5webkit5-dev swig	
	sudo apt install -y libqtwebkit-dev qtwebengine5-dev
	sudo apt autoremove -y 
}

function set_freeCAD()
{

	if [ ! -d $WorkPath/freeCAD-source ]; then	
		cd $WorkPath
		git clone  --depth=1 https://github.com/FreeCAD/FreeCAD.git freeCAD-source
	fi	
}

function make_freeCAD()
{
	if [ -d $WorkPath/freeCAD-source ]; then	
		cd $WorkPath/freeCAD-source 
		git pull
		cmake .
		make -j$(nproc)
	fi	
}

OPTION=$(whiptail --title "freeCAD config system" \
	--menu "$MENUSTR" 20 60 12 --cancel-button Finish --ok-button Select \
	"0"   "AUTO all" \
	"1"   "prepare" \
	"2"   "make" \
	"3"   "config" \
	3>&1 1>&2 2>&3)



if [ $OPTION = '0' ]; then
	clear
	echo -e "AUTO \n${Line}"
	apt_install	
	set_freeCAD
	make_freeCAD
	exit 0
elif [ $OPTION = '1' ]; then
	clear
	echo -e "prepare\n${Line}"
	apt_install
	set_freeCAD
	exit 0
elif [ $OPTION = '2' ]; then
	clear
	echo -e "make\n${Line}"
	make_freeCAD
	exit 0
elif [ $OPTION = '3' ]; then
	clear
	echo -e "config\n${Line}"
	
	exit 0	
else
	whiptail --title "freeCAD config system" \
		--msgbox "Please select correct option" 10 50 0
	exit 0
fi



exit 0
