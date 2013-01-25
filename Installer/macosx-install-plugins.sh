#!/bin/bash

PLUGINS_SRC='../resources/services'
PLUGINS_PRE='../bin/plugins'
PLUGINS_END='../bin/xVideoServiceThief.app/Contents/Resources/plugins'

# change to script directory
cd `dirname "$0"`

# remove plugins cache directory
if [ -d $PLUGINS_PRE ]; then
	echo "Removing plugins from: $PLUGINS_PRE"
	rm -rv $PLUGINS_PRE
fi

# remove plugins directory from application bundle
if [ -d $PLUGINS_END ]; then
	echo "Removing plugins from: $PLUGINS_END"
	rm -rv $PLUGINS_END
fi

# create plguins directories
echo "Creating directory: $PLUGINS_PRE"
mkdir $PLUGINS_PRE

echo "Creating directory: $PLUGINS_END"
mkdir $PLUGINS_END

# find and copy all plugins from source directories to plugins cache
echo "Copying plugins to cache directory"
find $PLUGINS_SRC -name \*.js -exec cp -v {} $PLUGINS_PRE \;

# install all detected plugins into application bundle
echo "Installing all plugins to Application bundle"
cp -vR $PLUGINS_PRE $PLUGINS_END/..