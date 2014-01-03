#!/bin/bash

# configure DMG
DMG_NAME='xVST_2_5_1-intel.dmg'
DMG_TITLE='xVideoServiceThief'
DMG_SIZE='100m'

# DMG template
DMG_TEMPLATE='xVST-template.dmg'
DMG_TEMPLATE_BZ2='xVST-template.dmg.bz2'

# files to copy
FILES=( 'xVideoServiceThief.app' )

# configure dirs
APP_DIR='../bin'
TMP_DIR='TMP'

# change to script directory
cd `dirname "$0"`

# remove temporal dir function
function remove_temporal_dir {
	if [ -d $TMP_DIR ]; then
		echo "Removing temporal directory: $TMP_DIR"
		rm -rv $TMP_DIR
	fi
}

# remove temporal dir
remove_temporal_dir

# create temporal dir
echo "Creating directory: $TMP_DIR"
mkdir $TMP_DIR

# copy DMG compressed templete
echo "Copying compressed template to: $TMP_DIR"
cp -v $DMG_TEMPLATE_BZ2 $TMP_DIR

# uncompressing DMG template
echo "Decompressing DMG template"
bzip2 -d "$TMP_DIR/$DMG_TEMPLATE_BZ2" -v

# mount DMG template
echo "Attaching DMG..."
hdiutil attach "$TMP_DIR/$DMG_TEMPLATE" -noautoopen -mountroot "$TMP_DIR"

# copy files into DMG
echo "Copying files to $TMP_DIR/$DMG_TITLE"
for FILE in ${FILES[@]}; do
	cp -vR $APP_DIR/$FILE $TMP_DIR/$DMG_TITLE
done

# unmount DMG
echo "Detaching DMG..."
hdiutil detach "$TMP_DIR/$DMG_TITLE" -force

# check if final DMG already exsits
if [ -a $DMG_NAME ]; then
	echo "Removing DMG: $DMG_NAME"
	rm -v $DMG_NAME
fi

# compress DMG
hdiutil convert "$TMP_DIR/$DMG_TEMPLATE" -format UDZO -imagekey zlib-level=9 -o "$DMG_NAME"

# remove temporal dir
remove_temporal_dir
