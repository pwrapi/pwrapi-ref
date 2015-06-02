#!/bin/sh

INSTALL_PATH = `pwd`/../../install

for IMAGE_PATH in '/images/rootfs/TOSS-3.1.4_nfsroot_H_OFED/usr' '/images/rootfs/TOSS-3.1.4_nfsroot_H_OFED-LOGIN/usr' '/usr' '/images/rootfs/AMD_HSA_FINAL_OFED/usr' '/images/rootfs/HSA_LOGIN/usr'
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

