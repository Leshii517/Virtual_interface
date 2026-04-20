# Virtual_interface

## запуск
*sudo insmod virtual_interface.ko*  
*echo "192.168.123.1" > /proc/vping/ip* 
*sudo ip link set vping0 up*

*sudo ip route add 192.168.123.1/32 dev vping0*

*sudo ip route add 192.168.123.1/32 dev vping0*

*ping -c 4 192.168.123.1*

## Вывод кода

> PING 192.168.123.1 (192.168.123.1) 56(84) bytes of data.
64 bytes from 192.168.123.1: icmp_seq=1 ttl=64 time=0.210 ms
64 bytes from 192.168.123.1: icmp_seq=2 ttl=64 time=0.203 ms
64 bytes from 192.168.123.1: icmp_seq=3 ttl=64 time=0.203 ms
64 bytes from 192.168.123.1: icmp_seq=4 ttl=64 time=0.221 ms

--- 192.168.123.1 ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3069ms
rtt min/avg/max/mdev = 0.203/0.209/0.221/0.007 ms
