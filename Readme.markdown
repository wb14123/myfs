
This is a most simple file system. Maybe too simple to use. I'm writing it for pratice and making it usable step-by-step. Almost all the code are based on `fs/ramfs` now. It is a good point to start.

## How to play with these code:

Install kernel src for you kernel. It will install some kernel source for build modules under `/lib//modules/<kernel_version>/build`.

Make the module:

	make

Insert the module to your kernel:

	sudo insmod ./myfs.ko

Mount with `myfs`:

	sudo mount -t myfs none /mnt

## TODO

* After mount, there will be permission issue.
* Kernel oops will occurs while umount since there is no `kill_sb` function.
