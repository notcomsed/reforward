# reforward
simple tcp forward tool. use epoll events.

### start

```bash
make
```

```
reforward -d [listen] [listen Port] [connect] [connect Port] [IP version] [deny IP 1] [deny IP 2]
```

#### IPv4
```bash
reforward -d 0.0.0.0 8080 192.168.1.1 80 4 0 0
reforward -d 192.168.1.100 2222 192.168.1.127 22 4 0 0
```

listen 0.0.0.0:8080, forward to 192.168.1.1:80

listen 192.168.1.100:2222, forward to 192.168.1.127:22

#### IPv6
```bash
reforward -d :: 8080 192.168.1.1 80 6 0 0
reforward -d fd20::1 8888 192.168.1.2 8888 6 0 0
```

listen [::]:8080, forward to 192.168.1.1:80

listen [fd20::1]:8888, forward to 192.168.1.2:8888

#### NOTICE

reforward -d 0.0.0.0 8080 192.168.1.1 80 `4` 0 0

reforward -d :: 8080 192.168.1.1 80 ``6`` 0 0

This is difference!

#### logged

```bash
setsid reforward -d 0.0.0.0 8080 192.168.1.1 80 4 0 0 >> /var/log/reforward.log
```

use `-h` to get help

--------------------
## iperf SpeedTest

#### direct iperf
```
------------------------------------------------------------

 Client connecting to 127.0.0.1, TCP port 8888
 
 TCP window size: 2.50 MByte (default)
 
------------------------------------------------------------

[  3] local 127.0.0.1 port 51902 connected with 127.0.0.1 port 8888

[ ID] Interval       Transfer     Bandwidth

[  3] 0.0000-1.0001 sec  5.30 GBytes  45.5 Gbits/sec
```

-------------------
#### rinetd

```
------------------------------------------------------------

Client connecting to 127.0.0.1, TCP port 8088

TCP window size: 4.00 MByte (default)

------------------------------------------------------------

[  3] local 127.0.0.1 port 37908 connected with 127.0.0.1 port 8088

[ ID] Interval       Transfer     Bandwidth

[  3] 0.0000-2.0038 sec   741 MBytes  3.10 Gbits/sec


------------------------------------------------------------

Client connecting to 127.0.0.1, TCP port 8088

TCP window size: 4.00 MByte (default)

------------------------------------------------------------

[  3] local 127.0.0.1 port 56576 connected with 127.0.0.1 port 8088

[ ID] Interval       Transfer     Bandwidth

[  3] 0.0000-30.0031 sec  10.5 GBytes  3.00 Gbits/sec
```

-------------------
#### reforward

```
------------------------------------------------------------

Client connecting to 127.0.0.1, TCP port 8080

TCP window size: 2.50 MByte (default)

------------------------------------------------------------

[  3] local 127.0.0.1 port 48900 connected with 127.0.0.1 port 8080

[ ID] Interval       Transfer     Bandwidth

[  3] 0.0000-2.0001 sec  1.87 GBytes  8.04 Gbits/sec



------------------------------------------------------------

Client connecting to 127.0.0.1, TCP port 8080

TCP window size: 2.50 MByte (default)

------------------------------------------------------------

[  3] local 127.0.0.1 port 35470 connected with 127.0.0.1 port 8080

[ ID] Interval       Transfer     Bandwidth

[  3] 0.0000-30.0012 sec  27.3 GBytes  7.81 Gbits/sec

```

### Developing

reforward2 is a independent code which not rely on reforward. but reforward2 still testing.

```bash
make reforward2
```
