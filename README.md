# override

To setup override, you have to download the ISO from the intra's subject page. Then create a virtual machine using your prefered software (in my case Virtual Box).

To create the VM, you can do this manually or just use 
```sh
./tools/setup_override_vm.sh [iso path]
```

Or create the VM manually and add a port forwarding rule under `Settings > Network > Adapter 1`, set `Attached to` to `NAT` and create a `Port Forwarding` rule with both the `Host Port` and `Guest Port` set to **4242**.

After starting the VM you can log on using `ssh -p 4242 level00@127.0.0.1` and using the password `level00`. Then you're good to go.

You can log in as a specific user (level[00-09]), or latest using the following command
```sh
./override_toolkit.sh open <target user> [file path]
```

This toolkit can also inject or extract a file using
```sh
./override_toolkit.sh <inject|extract> <target user> [file path]
```
