#!/bin/bash

echo -n "Creating tap network device for SGX-LKL..."
sudo ip tuntap add dev sgxlkl_tap0 mode tap user "$(whoami)"
sudo ip link set dev sgxlkl_tap0 up
sudo ip addr add dev sgxlkl_tap0 10.0.1.254/24
echo "Done"

echo -n "Updating host networking to forward traffic from SGX-LKL to the public host interface..."
# Enable packet forwarding
sudo sysctl -w net.ipv4.ip_forward=1 > /dev/null

# Allow forwarding to/from TAP
sudo iptables -I FORWARD -m state -d 10.0.1.0/24 --state NEW,RELATED,ESTABLISHED -j ACCEPT
sudo iptables -I FORWARD -m state -s 10.0.1.0/24 --state NEW,RELATED,ESTABLISHED -j ACCEPT

# If enclave needs establish new connections to external hosts, masquerade
# outgoing traffic from enclave
sudo iptables -t nat -A POSTROUTING -s 10.0.1.0/24 ! -d 10.0.1.0/24 -j MASQUERADE

# Sample forwarding rule: Forward traffic from host's public interface TCP port 60321 to SGX-LKL port 6379
# sudo iptables -t nat -I PREROUTING -p tcp -d `hostname -i` --dport 60321 -j DNAT --to-destination 10.0.1.1:6379

# Configure the port redirection for the TLS benchmarks
sudo iptables -t nat -I PREROUTING -p tcp --dport 11112 -j DNAT --to-destination 10.0.1.1:11112
sudo iptables -t nat -I OUTPUT -p tcp --dport 11112 -j DNAT --to-destination 10.0.1.1:11112

echo "Done"
