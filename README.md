camera_demo
===========

Software for the phase 2 camera demo


notes
===========

You need to run this command on the odroid to make your udp 
buffer size big enough to send a full frame. Otherwise the
video will look all wonky:

```
sudo /sbin/sysctl -w net.core.rmem_max=5524288 \
net.core.wmem_max=552428 \
net.core.rmem_default=552428 \
net.core.wmem_default=5524288
```
