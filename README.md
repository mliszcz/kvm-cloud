kvm-cloud
=========

### KVM-cloud
This software is a KVM-based IaaS provider. A typical configuration consists of the following elements:
 * *controller* node (one or more) which controls VM instances
 * *manager* node which supervises all controllers in the cluster
 * *client-cli* running on the end-user machine

Clients communicate directly only with the *manager* node. *Manager* instantiates VMs on *controllers*. Next controller is selected in a round-robin manner.

### Building
The build process is controlled by the *Make* program. There are no external dependencies. However, working QEMU installation is required (*/usr/bin/kvm*). Installing *qemu-kvm* (with dependencies) from your distribution's repository should be sufficient.

The *Makefile* comes with three targets:
 * `controller`
 * `manager`
 * `client`

You can also type `make all` to build everything. The executables are stored in the *out/* directory. Modern gcc is required (tested on 4.8.2).

### VM templates
VM template consists of a rootfs image and a kernel. The *templates/cirros* directory contains CirrOS (https://launchpad.net/cirros) image, which can be used to test your installation.

### Examples
Example configuration consists of the following:
 * controller1: 127.0.0.1:8051
 * controller2: 127.0.0.1:8052
 * manager: 127.0.0.1:8050
 * client

First, create working directories and start the controllers (in separate terminals):
```
$ mkdir -p /tmp/s{1,2}

$ ./out/controller 8051 /tmp/s1 1000 2200
$ ./out/controller 8052 /tmp/s2 2000 2300
```
First one will be assigning IDs starting from 1000 and SSH ports from 2200. Second one - from 2000 and 2300 respectively.
You should get output similar to:
```
[INFO] 12:36:32 : VM Controller started!
```

Then, create controllers list and pass it to the manager instance. The expected format:
```
controller1-name controller2-address
controller2-name controller2-address
...
```

```
$ printf 's1 127.0.0.1:8051\ns2 127.0.0.1:8052\n' >> controllers.list

$ ./out/manager 8050 controllers.list
```
If communication works properly and controllers are available, you should get:
```
[INFO] 12:38:59 : manager started
[INFO] 12:38:59 : supervised controllers:
[INFO] 12:38:59 : s1: 127.0.0.1:8051
[INFO] 12:38:59 : s2: 127.0.0.1:8052
```

Finally, run client CLI, passing manager address in argument:
```
$ ./out/client 127.0.0.1:8050
KVM-cloud CLI
type 'help' for list of all available commands

\>
```

Now, create two VMs using Cirros template, both with 64M RAM and 2 CPUs:
```
\> new cirros 64 2
   result: ID: s2/2000, SSH port: 2301

\> new cirros 64 2
   result: ID: s1/1000, SSH port: 2201

\> instances
   ID		template	RAM		CPUs		SSH port	status
 - s1/1000	cirros		64		2		  -  		OFFLINE	
 - s2/2000	cirros		64		2		  -  		OFFLINE	

\> 
```

To run instance, use `run` command. List instances one more time to check what port was assigned:
```
\> run s2/2000
OK

\> instances
   ID		template	RAM		CPUs		SSH port	status
 - s1/1000	cirros		64		2		  -  		OFFLINE	
 - s2/2000	cirros		64		2		2301		ONLINE	
```
Instance *s2/2000* is now ONLINE and port 2301 on s2 controller forwards traffic to port 22 on *s2/2000* instance.

Test connection using SSH. *NOTE: cirros login is 'cirros' and password is 'cubswin:)'.*
```
$ ssh -p 2301 cirros@127.0.0.1
cirros@127.0.0.1's password: 
$ uname -a
Linux cirros 3.2.0-68-virtual #102-Ubuntu SMP Tue Aug 12 22:14:39 UTC 2014 x86_64 GNU/Linux
$ 
```
