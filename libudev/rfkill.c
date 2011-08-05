/*
From:
http://www.spinics.net/lists/hotplug/msg04269.html

Subject: Re: Sysfs properties with libudev (for example capabilities/key)
From: Laszlo Papp <djszapi@xxxxxxxxxxxx>
Date: Sun, 31 Oct 2010 07:52:44 -0700

gcc -Wall -o rfkill -ggdb -ludev rfkill.c
*/

#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>

#define RFKILL0 "/sys/class/rfkill/rfkill0"

int main()
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices;
    struct udev_list_entry *dev_list_entry;
    struct udev_device *dev;
    struct udev_device *parent_dev;
    struct udev_device *grandparent_dev;

    int index = 0;

    udev = udev_new ();
    if (!udev) {
        printf ("Cannot create udev\n");
        return -1;
    }
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "rfkill");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path, *index_c;
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        index_c = udev_device_get_sysattr_value (dev, "index");
        if (index_c && atoi(index_c) == index)
            break;

        udev_device_unref (dev);
    }

    if (!dev)
        goto out;

    printf ("Device Node Path: %s, Name: %s\r\n",
            udev_device_get_devpath (dev),
            udev_device_get_sysattr_value (dev, "name"));
    printf ("Subsystem: %s\n", udev_device_get_subsystem (dev));

    parent_dev = udev_device_get_parent (dev);

    if (parent_dev) {
        printf ("Parent subsystem: %s\n",
                udev_device_get_subsystem (parent_dev));
        grandparent_dev = udev_device_get_parent (parent_dev);
        if (grandparent_dev) {
            printf ("Grandparent subsystem: %s\n",
                    udev_device_get_subsystem (grandparent_dev));
        }
    }

    udev_device_unref (dev);

out:
    udev_unref (udev);

    return 0;
}

