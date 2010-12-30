/*
From:
http://www.spinics.net/lists/hotplug/msg04269.html

Subject: Re: Sysfs properties with libudev (for example capabilities/key)
From: Laszlo Papp <djszapi@xxxxxxxxxxxx>
Date: Sun, 31 Oct 2010 07:52:44 -0700

Modified version

gcc -Wall -o input3 -ggdb -ludev input3.c
*/

#include <libudev.h>
#include <stdio.h>
#include <string.h>

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
    /* Match the devices provide keys such as Power Button, Sleep Button, and of course keyboards */
    udev_enumerate_add_match_property(enumerate, "ID_INPUT_KEY", "1");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        /* The parent device contains the name, VID, and PID and could be useful. */
        parent_dev = udev_device_get_parent_with_subsystem_devtype(dev, "input", 0);
        if (!parent_dev) {
            udev_device_unref(parent_dev);
            continue;
        }

        /* Information from the device node */
        printf("Device Node Path: %s\n", path);
        printf("Devnode: %s\n",
                udev_device_get_devnode (dev));
        printf("Devpath: %s\n",
                udev_device_get_devpath (dev));
        printf("Devlinks: %s\n",
                udev_device_get_property_value (dev, "DEVLINKS"));

        /* Information from the parent device node */
        printf("Name: %s\r\n",
                udev_device_get_sysattr_value(parent_dev, "name"));
        printf("VID/PID: %s %s\r\n",
                udev_device_get_sysattr_value(parent_dev,"id/vendor"),
                udev_device_get_sysattr_value(parent_dev, "id/product"));
        printf("Devpath: %s\n\n",
                udev_device_get_devpath (parent_dev));

        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return 0;
}

