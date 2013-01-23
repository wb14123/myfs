
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

#define DEBUG(msg) printk(KERN_ALERT "MYFS: %s\n", msg)

MODULE_LICENSE("GPL");

struct inode *myfs_get_inode(struct super_block *sb,
		const struct inode *dir, umode_t mode, dev_t dev)
{
	struct inode *inode = new_inode(sb);

	if (inode) {
		inode->i_ino = get_next_ino();
		inode_init_owner(inode, dir, mode);

		inode->i_op = &simple_dir_inode_operations;
		inode->i_fop = &simple_dir_operations;
	}

	return inode;
}

static const struct super_operations myfs_ops ={
};

int myfs_file_super(struct super_block *sb, void *data, int slient)
{
	struct inode *inode;

	sb->s_op = &myfs_ops;
	inode = myfs_get_inode(sb, NULL, S_IFDIR, 0);
	sb->s_root = d_make_root(inode);
	if (!sb->s_root)
		DEBUG("cannot get dentry root");
	DEBUG("work done, I have mounted");
	return 0;
}


struct dentry *myfs_mount(struct file_system_type *fs_type,
		int flags, const char *dev_name, void *data)
{
	DEBUG("ok, I will mount");
	return mount_nodev(fs_type, flags, data, myfs_file_super);
}

static struct file_system_type myfs_type = {
	.name         = "myfs",
	.mount        = myfs_mount,
	.owner        = THIS_MODULE,
};


static int __init init_myfs(void)
{
	int err;

	DEBUG("hi, I'm myfs");

	err = register_filesystem(&myfs_type);
	if (err)
		DEBUG("oh, there is something wrong");

	return 0;
}

static void __exit exit_myfs(void)
{
	DEBUG("bye, world");
	unregister_filesystem(&myfs_type);
}

module_init(init_myfs);
module_exit(exit_myfs);
