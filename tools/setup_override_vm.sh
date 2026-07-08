#! /bin/sh

ISO_PATH="${1:-/home/$USER/sgoinfre/OverRide.iso}"
VM_NAME="override"
VM_PORT="4242"

function separator() {
	echo "===	==	==="
	echo "$1"
	echo "===	==	==="
}

vboxmanage list vms | grep $VM_NAME
if [ $? = 0 ]; then
separator "Removing existing VM";
vboxmanage unregistervm $VM_NAME --delete;
fi;

separator "Creating VM $VM_NAME"
vboxmanage createvm --name "$VM_NAME" --register
vboxmanage modifyvm "$VM_NAME" --ostype "Ubuntu_64"

separator "Binding Storage"
vboxmanage createmedium disk --filename "$HOME/VirtualBox VMs/$VM_NAME/$VM_NAME.vdi" --size 10000
vboxmanage storagectl "$VM_NAME" --name "SATA Controller" --add sata --controller IntelAhci
vboxmanage storageattach "$VM_NAME" --storagectl "SATA Controller" --port 0 --device 0 --type hdd --medium "$HOME/VirtualBox VMs/$VM_NAME/$VM_NAME.vdi"
vboxmanage storagectl "$VM_NAME" --name "IDE Controller" --add ide

separator "Linkin ISO file"
vboxmanage storageattach "$VM_NAME" --storagectl "IDE Controller" --port 0 --device 0 --type dvddrive --medium "$ISO_PATH"

separator "Configuring ports"
vboxmanage modifyvm "$VM_NAME" --nic1 nat
vboxmanage modifyvm "$VM_NAME" --natpf1 "rule1,tcp,,${VM_PORT},,${VM_PORT}"
echo "Ports successfully configured"

separator "Booting VM $VM_NAME"
vboxmanage startvm "$VM_NAME" --type gui
