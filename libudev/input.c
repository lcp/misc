/*
From:
http://www.spinics.net/lists/hotplug/msg04269.html

Subject: Re: Sysfs properties with libudev (for example capabilities/key)
From: Laszlo Papp <djszapi@xxxxxxxxxxxx>
Date: Sun, 31 Oct 2010 07:52:44 -0700

gcc -Wall -o input -ggdb -ludev input.c
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
        if (parent_dev) {
            udev_device_unref(parent_dev);
            continue;
        }

        printf("Device Node Path: %s, Name: %s\r\n", path,
                udev_device_get_sysattr_value(dev, "name"));
        printf("VID/PID: %s %s\r\n",
                udev_device_get_sysattr_value(dev,"id/vendor"),
                udev_device_get_sysattr_value(dev, "id/product"));
        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return 0;
}

