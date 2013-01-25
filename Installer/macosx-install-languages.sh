#!/bin/bash

LANGUAGES_SRC='../resources/translations'
LANGUAGES_PRE='../bin/languages'
LANGUAGES_END='../bin/xVideoServiceThief.app/Contents/Resources/languages'

# change to script directory
cd `dirname "$0"`

# remove languages cache directory
if [ -d $LANGUAGES_PRE ]; then
	echo "Removing languages from: $LANGUAGES_PRE"
	rm -rv $LANGUAGES_PRE
fi

# remove languages directory from application bundle
if [ -d $LANGUAGES_END ]; then
	echo "Removing languages from: $LANGUAGES_END"
	rm -rv $LANGUAGES_END
fi

# create language directories
echo "Creating directory: $LANGUAGES_PRE"
mkdir $LANGUAGES_PRE

echo "Creating directory: $LANGUAGES_END"
mkdir $LANGUAGES_END

# find and copy all languages from source directories to languages cache
echo "Copying languages (QM) to cache directory"
find $LANGUAGES_SRC -name \*.qm -exec cp -v {} $LANGUAGES_PRE \;

# delete the language template
echo "Deleting QM template file"
rm -v $LANGUAGES_PRE/template_for_new_translations.qm

echo "Copying languages (Language) to cache directory"
find $LANGUAGES_SRC -name \*.language -exec cp -v {} $LANGUAGES_PRE \;

# install all detected plugins into application bundle
echo "Installing all languages to Application bundle"
cp -vR $LANGUAGES_PRE $LANGUAGES_END/..