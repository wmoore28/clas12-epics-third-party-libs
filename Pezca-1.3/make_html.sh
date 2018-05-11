#!/bin/sh

CSS_SRC_FILE=docStyle.css 
DEST_DIR=blib/html

mkdir -p $DEST_DIR
cp $CSS_SRC_FILE $DEST_DIR
pod2html -css $CSS_SRC_FILE Pezca.pm > $DEST_DIR/Pezca.html

echo "html documentation created in $DEST_DIR"

if [ "$1" == "--index" ]; then
  rst2html --stylesheet-path=docStyle.css --cloak-email-addresses index.rst > $DEST_DIR/index.html
fi
