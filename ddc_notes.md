# Notes on environment
Use udev rule from 

    sudo cp /usr/share/ddcutil/data/45-ddcutils-i2c.rules /etc/udev/rules.d

To make the i2c devices readable by group i2c. Then add that group and add
yourself according to

    ddcutil environment

# Kernel module
A kernel module must be loaded for the devices to show up. Again refer to the
environment command

# Capabilities
"ddcutil capabilities" shows brightness as 

    Feature: 10 (Brightness)

we should check for this

# Changing brightness:

    ddcutil setvcp 10 <level>

# Main documentation
https://www.ddcutil.com/api_main/

The api seems to be installed automatically on arch under /usr/include


