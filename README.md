# dwm_battery_status

This project is essentially another version of this:

https://notabug.org/kl3/dwmstat

But for FreeBSD.  I wanted to try a similar thing, but there is much less functionality.

The difference here is that this version uses sysctl to get the battery life.  Whereas, the original project supports OpenBSD, and uses the /dev/apm device, which didn't look like a default option in FreeBSD.
