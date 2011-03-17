/*
based on:
http://www.spinics.net/lists/hotplug/msg04269.html

Subject: Re: Sysfs properties with libudev (for example capabilities/key)
From: Laszlo Papp <djszapi@xxxxxxxxxxxx>
Date: Sun, 31 Oct 2010 07:52:44 -0700

gcc -Wall -o dmi -ggdb -ludev dmi.c
*/

#include <libudev.h>
#include <stdio.h>

int main()
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices;
    struct udev_list_entry *dev_list_entry;
    struct udev_device *dev;

    udev = udev_new();
    if (!udev) {
        printf("Cannot create udev\n");
        return -1;
    }

    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "dmi");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

	printf("Path: %s\n", path);

        printf("[Product]\n Name: %s\n Version: %s\n\n",
		udev_device_get_sysattr_value(dev, "product_name"),
                udev_device_get_sysattr_value(dev, "product_version"));
        printf("[BIOS]\n Date: %s\n Vendor: %s\n Version: %s\n\n",
		udev_device_get_sysattr_value(dev, "bios_date"),
		udev_device_get_sysattr_value(dev, "bios_vendor"),
                udev_device_get_sysattr_value(dev, "bios_version"));
        printf("[Board]\n Asset Tag: %s\n Name: %s\n Serial: %s\n Vendor: %s\n Version: %s\n\n",
		udev_device_get_sysattr_value(dev, "board_asset_tag"),
		udev_device_get_sysattr_value(dev, "board_name"),
		udev_device_get_sysattr_value(dev, "board_serial"),
		udev_device_get_sysattr_value(dev, "board_vendor"),
                udev_device_get_sysattr_value(dev, "board_version"));
        printf("[Chassis]\n Asset Tag: %s\n Serial: %s\n Type: %s\n Vendor: %s\n Version: %s\n\n",
		udev_device_get_sysattr_value(dev, "chassis_asset_tag"),
		udev_device_get_sysattr_value(dev, "chassis_serial"),
		udev_device_get_sysattr_value(dev, "chassis_type"),
		udev_device_get_sysattr_value(dev, "chassis_vendor"),
                udev_device_get_sysattr_value(dev, "chassis_version"));
	printf("[SYS]\n Vendor: %s\n",
                udev_device_get_sysattr_value(dev, "sys_vendor"));
        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return 0;
}

