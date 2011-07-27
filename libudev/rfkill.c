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

#define RFKILL0 "/sys/class/rfkill/rfkill0"

int main()
{
    struct udev *udev;
    struct udev_device *dev;
    struct udev_device *parent_dev;
    struct udev_device *grandparent_dev;

    udev = udev_new ();
    if (!udev) {
        printf ("Cannot create udev\n");
        return -1;
    }

    dev = udev_device_new_from_syspath (udev, RFKILL0);
    if (!dev) {
        printf ("Failed to get device for %s\n", RFKILL0);
        return -1;
    }

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
    udev_unref (udev);

    return 0;
}

