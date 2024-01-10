# Configure your Linux Computer as an AP of the project
Please watch the figure on assets/network-diagram.png for understanding this article.

## Hardware you need:
  - A wifi adapter network that supports AP virtual interface (we used an Alfa AWUS036ACH-C, but some integrated wifi cards can handle promiscuous mode and AP virtual interfaces)
  - A computer running Linux with systemd.

## Software you need:
  - [Linux Wifi Hotspot](https://github.com/lakinduakash/linux-wifi-hotspot)

## Brief overview of the steps:

  - Patch the create_ap program that comes with Linux Wifi Hotspot for enabling an static IP for the raspberry pi.
  - Change the Internet interface and the Wifi interface on the ap.conf.
  - Execute!

### Patch the create_ap program

By default, [Linux Wifi Hotspot](https://github.com/lakinduakash/linux-wifi-hotspot) executes:
  - [hostapd](https://wiki.gentoo.org/wiki/Hostapd): For creating the wifi access point
  - [dnsmasq](https://wiki.archlinux.org/title/Dnsmasq): For adding DHCP server to the network.
  - [iptables](https://wiki.archlinux.org/title/Iptables): For forwarding all the traffic that comes from the AP network (in other words, in your AP network your computer acts as a router, forwarding the packets).

Using the **create_ap** script that comes with the Linux Wifi Hotspot packages become very handy, since we don't have to create the hostapd, dnsmasq and iptables ourselves, buuut has a catch: **it doesn't allow us to put dhcp rules (such as static ip for the raspberry pi) on his config file**.

This is because the **create_ap** script automatically generates the configuration for hostapd, dnsmasq and iptables given the configuration file you gave it (and obviously this config file doesn't have an option for adding dhcp rules...), [you can watch the code that generetes the dnsmasq rules here](https://github.com/lakinduakash/linux-wifi-hotspot/blob/d73242ab812284b5ed65275f630b8bc306b725c5/src/scripts/create_ap#L1762).

So, we had to patch the **create_ap** script for adding our custom rule! Yay! First of all find where is the **create_ap** script living on your computer:

```bash
$ whereis create_ap
# /usr/bin/create_ap if you're in archlinux
```

Then, patch it with:
```bash
$ patch /usr/bin/create_ap < create_ap_raspberry_pi.patch
```

Be aware that this will add a custom rule for the dnsmasq program, which is:

```
dhcp-host=e4:5f:01:f2:b4:df,192.168.33.213
```

If you don't have the same raspberry we're using, the raspberry mac address will change, this means you will have to tweak this patch file. If you're following our [network diagram]() you don't have to change the ip address.


### Change the interfaces of the config file
Every linux distribution names his network devices differently (and everyone has different devices), so execute the following command:

```bash
$ ip a
....
17: wlp0s20f0u2: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 2312 qdisc mq state UP group default qlen 1000
18: enp0s20f0u6: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
```

In my case I see that the Alfa Wifi Card is named wlp0s20f0u2, and the ethernet connection to the internet is named enp0s20f0u6, so change the ap.conf acording to this interface names:

```
....
WIFI_IFACE=wlp0s20f0u2
INTERNET_IFACE=enp0s20f0u6
```

### Run the access point!
Everything is ready now! Execute:

```bash
$ sudo create_ap --config ap.conf
```

If any problem occurs is 99% problem of your wifi card. See the requirements on the hardware section.
