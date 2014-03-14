
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define MYFS_DEBUG(msg) printk(KERN_ALERT "MYFS: %s\n", msg)

#define DEFAULT_MODE      0775

MODULE_LICENSE("GPL");

#define BUFFER_SIZE	102400

static char buffer[BUFFER_SIZE];

/*
 * This part is operations about inode
 */

static struct inode
*myfs_get_inode(struct super_block *, const struct inode *, umode_t, dev_t);

static int myfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev)
{
	struct inode *inode = myfs_get_inode(dir->i_sb, dir, mode, dev);
	int error = -ENOSPC;

	if (inode) {
		d_instantiate(dentry, inode);
		dget(dentry);
		error = 0;
		dir->i_mtime = dir->i_ctime = CURRENT_TIME;
	}

	return error;
}

static int myfs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl)
{
	return myfs_mknod(dir, dentry, mode | S_IFREG, 0);
}

static int myfs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	int retval = myfs_mknod(dir, dentry, mode | S_IFDIR, 0);
	if (!retval)
		inc_nlink(dir);
	return retval;
}

static struct inode_operations myfs_dir_inode_operations = {
	.create = myfs_create,
	.lookup = simple_lookup,
	.mkdir  = myfs_mkdir,
	.mknod  = myfs_mknod,
};

int myfs_file_open(struct inode *inode, struct file *file)
{
	printk(KERN_ALERT "MYFS: i_ino %ld\n", inode->i_ino);
	file -> private_data = &(buffer[inode->i_ino]);
	return 0;
}

ssize_t myfs_file_read(struct file *file, char __user *user_buf,
		size_t count, loff_t *ppos)
{
	loff_t pos = *ppos;
	*ppos = 1;

	if (pos < 0)
		return -EINVAL;
	if (pos > 0)
		return 0;

	if (copy_to_user(user_buf, file -> private_data, 1))
		return -EFAULT;
	else
		return 1;
}

ssize_t myfs_file_write(struct file *file, const char __user *user_buf,
		size_t count, loff_t *ppos)
{
	loff_t pos = *ppos;
	*ppos = pos + count;
	if (pos < 0)
		return -EINVAL;
	if (pos > 0)
		return count;

	if (copy_from_user(file -> private_data, user_buf, 1))
		return -EFAULT;
	else
		return count;
}

static struct file_operations myfs_file_operations = {
	.open = myfs_file_open,
	.read = myfs_file_read,
	.write = myfs_file_write,
};

static struct inode *myfs_get_inode(struct super_block *sb,
		const struct inode *dir, umode_t mode, dev_t dev)
{
	struct inode *inode = new_inode(sb);

	if (inode) {
		inode->i_ino = get_next_ino();
		inode_init_owner(inode, dir, mode);

		inode->i_op = &myfs_dir_inode_operations;
		if (mode & S_IFDIR)
			inode->i_fop = &simple_dir_operations;
		else
			inode->i_fop = &myfs_file_operations;
	}

	return inode;
}

/*
 * This part is operations about super block
 */

static const struct super_operations myfs_ops ={
};

/*
 * This is an intersting function, it makes an inode as a super block's
 * root. This is who combines inode and super block
 */
static int myfs_file_super(struct super_block *sb, void *data, int slient)
{
	struct inode *inode;

	sb->s_op = &myfs_ops;
	inode = myfs_get_inode(sb, NULL, S_IFDIR | DEFAULT_MODE, 0);
	sb->s_root = d_make_root(inode);
	if (!sb->s_root)
		MYFS_DEBUG("cannot get dentry root");
	MYFS_DEBUG("work done, I have mounted");
	return 0;
}

static struct dentry *myfs_mount(struct file_system_type *fs_type,
		int flags, const char *dev_name, void *data)
{
	MYFS_DEBUG("ok, I will mount");
	return mount_nodev(fs_type, flags, data, myfs_file_super);
}

static void myfs_kill_sb(struct super_block *sb)
{
	kfree(sb->s_fs_info);
	kill_litter_super(sb);
	MYFS_DEBUG("umount, i have killed myself...");
}

static struct file_system_type myfs_type = {
	.name         = "myfs",
	.mount        = myfs_mount,
	.owner        = THIS_MODULE,
	.kill_sb      = myfs_kill_sb,
};

/*
 * This part is about linux modules
 */

static int __init init_myfs(void)
{
	int err;

	MYFS_DEBUG("hi, I'm myfs");

	err = register_filesystem(&myfs_type);
	if (err)
		MYFS_DEBUG("oh, there is something wrong");

	return 0;
}

static void __exit exit_myfs(void)
{
	MYFS_DEBUG("bye, world");
	unregister_filesystem(&myfs_type);
}

module_init(init_myfs);
module_exit(exit_myfs);
