#!/bin/sh

INSTALL_PATH = `pwd`/../../install

for IMAGE_PATH in '/tftpboot/images/compute_powerinsight/usr' '/tftpboot/images/compute.x86_64/usr' '/usr'
do
	echo "Copying include files to $IMAGE_PATH"
	cp $INSTALL_PATH/include/* $IMAGE_PATH/include

	echo "Copying libraries to $IMAGE_PATH"
	cp -R $INSTALL_PATH/lib/* $IMAGE_PATH/lib
	cp -R $INSTALL_PATH/lib/* $IMAGE_PATH/lib64

	echo "Copying executables to $IMAGE_PATH"
	cp $INSTALL_PATH/bin/* $IMAGE_PATH/bin

	echo "Copying share to $IMAGE_PATH"
	cp -R $INSTALL_PATH/share/* $IMAGE_PATH/share
done

