ip netns add red
ip netns add green
ip netns add blue
ip link add eth0 type veth peer name eth1
ip link add eth2 type veth peer name eth3
ip link set eth0 netns red
ip link set eth1 netns green
ip link set eth2 netns green
ip link set eth3 netns blue
ip netns exec red ip link set eth0 up
ip netns exec green ip link set eth1 up
ip netns exec green ip link set eth2 up
ip netns exec blue ip link set eth3 up
ip netns exec red ip address add 10.0.0.1/24 dev eth0
ip netns exec green ip address add 10.0.0.2/24 dev eth1
ip netns exec green ip address add 10.0.1.3/24 dev eth2
ip netns exec blue ip address add 10.0.1.4/24 dev eth3
ip netns exec red ip route add default via 10.0.0.2 dev eth0
ip netns exec blue ip route add default via 10.0.1.3 dev eth3
ip netns exec green sysctl -w net.ipv4.ip_forward=1
ip netns exec green tc qdisc add dev eth2 root tbf rate 1mbit burst 32kbit latency 400ms
ip netns exec blue tc qdisc add dev eth3 root tbf rate 1mbit burst 32kbit latency 400ms
# ip netns exec green iperf3 -s
# ip netns exec blue iperf3 -c 10.0.1.3
ip netns exec red tc qdisc add dev eth0 root netem delay 0.100ms
ip netns exec green tc qdisc add dev eth1 root codel 