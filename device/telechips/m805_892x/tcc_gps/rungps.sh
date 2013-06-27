#!/system/bin/sh

source_dir=/system/TccGps
target_dir=/data/TccGps
almanac=Almanac.cg
hca=Hca.cg
ionosphere=Ionosphere.cg

gps_engine=tcc_gps
gps_driver=cgxdrv.ko


if  ! /system/bin/busybox test -d "$target_dir"; then
    echo "make GPS base directory: " "$target_dir"$
	mkdir "$target_dir"
fi 

if  ! /system/bin/busybox test -e "$target_dir/$almanac"; then
    echo "copy :" "$almanac"
	cp "$source_dir/$almanac" "$target_dir/$almanac"
fi 

if  ! /system/bin/busybox test -e "$target_dir/$hca"; then
    echo "copy :" "$hca"
	cp "$source_dir/$hca" "$target_dir/$hca"
fi

if  ! /system/bin/busybox test -e "$target_dir/$ionosphere"; then
    echo "copy :" "$ionosphere"
	cp "$source_dir/$ionosphere" "$target_dir/$ionosphere"
fi


if  ! /system/bin/busybox test -e "$target_dir/$gps_engine"; then
    echo "copy :" "$gps_engine"
	cp "$source_dir/$gps_engine" "$target_dir/$gps_engine"
fi

if  ! /system/bin/busybox test -e "$target_dir/$gps_driver"; then
    echo "copy :" "$gps_driver"
	cp "$source_dir/$gps_driver" "$target_dir/$gps_driver"
fi

chown shell "$target_dir"
chown shell "$target_dir/*"
chmod 777 "$target_dir/cgxdrv.ko"
chmod 777 "$target_dir/tcc_gps"
insmod    "$target_dir/cgxdrv.ko"
/system/bin/busybox mknod /dev/cgxdrv c 251 0
"$target_dir/tcc_gps"
