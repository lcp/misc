/*
From:
http://www.spinics.net/lists/hotplug/msg04269.html

Subject: Re: Sysfs properties with libudev (for example capabilities/key)
From: Laszlo Papp <djszapi@xxxxxxxxxxxx>
Date: Sun, 31 Oct 2010 07:52:44 -0700

gcc -Wall -o platform-input -ggdb -ludev platform-input.c
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
    struct udev_device *parent_dev;
    struct udev_device *grandparent_dev;

    udev = udev_new();
    if (!udev) {
        printf("Cannot create udev\n");
        return -1;
    }

    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);
        parent_dev = udev_device_get_parent_with_subsystem_devtype(dev, "input", 0);
        if (!parent_dev) {
            udev_device_unref(dev);
            continue;
        }

        grandparent_dev = udev_device_get_parent_with_subsystem_devtype(dev, "platform", 0);
        if (!grandparent_dev) {
            udev_device_unref(dev);
            continue;
	}

        printf("Device Node Path: %s\n", path);
	printf("Node: %s\n", udev_device_get_devnode (dev));
        printf("Subsystem: %s\n", udev_device_get_subsystem (dev));
        printf("Parent's VID/PID: %s %s\n",
                udev_device_get_sysattr_value(parent_dev,"id/vendor"),
                udev_device_get_sysattr_value(parent_dev, "id/product"));
        printf("Parent's name: %s\n",udev_device_get_sysattr_value (parent_dev, "name"));
        printf("Parent's Subsystem: %s\n", udev_device_get_subsystem (parent_dev));
        printf("Grandparent's Driver: %s\n", udev_device_get_driver (grandparent_dev));
        printf("Grandparent's Subsystem: %s\n", udev_device_get_subsystem (grandparent_dev));
	printf("\n");
        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return 0;
}

